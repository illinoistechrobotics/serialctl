//    serialctl
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#include "packet.h"
#include "hw.h"
#include "zserio.h"
#include <Servo.h>
#include "globals.h"
#include "Servo.h"
#include <SoftwareSerial.h>

packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
int laser, armpos, fire;
long last_s=0;
Servo arm, feed, spinner;
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#define DEADBAND_HALF_WIDTH 5

#define WATCHDOG_

#ifdef WATCHDOG_
#include <avr/wdt.h>      //watchdog library timer loop resets the watch dog
#endif

int getButton(int num){
	if(num<=7) {
		return (astate->btnlo >> num) & 0x01;
	}
	else if(num>7 && num <= 15){
		return (astate->btnhi >> (num - 8)) & 0x01;
	}
	else {
		return 0;
	}
}

SoftwareSerial left(2,3);
SoftwareSerial right(4,5);
char rightMotorDebug[5];
char leftMotorDebug[5];
int leftOutDebug;

void setup() {
	#ifdef WATCHDOG_
	wdt_enable(WDTO_250MS);  //Set 250ms WDT 
	wdt_reset();             //watchdog timer reset 
	#endif
	//Initialize safe to safe values!!
	safe.stickRX = 128;
	safe.stickRY = 128;
	safe.stickLX = 128;
	safe.stickLY = 128;
	safe.btnhi = 0;
	safe.btnlo = 0;
	safe.cksum = 0b1000000010001011;
	SerComm.begin(57600);
	comm_init();
	init_pins();
	laser=0;
	fire=0;
	armpos=1000;
	drive_left(0);
	drive_right(0);
	arm.attach(ARM_PIN);
	arm.writeMicroseconds(armpos);
	feed.attach(FEED_PIN);
	feed.writeMicroseconds(2300);
	spinner.attach(SPINNER_PIN);
	spinner.writeMicroseconds(1500); 
	// Start software serial
	left.begin(115200);
	right.begin(115200);
	// Copy safe values over the current state
	memcpy(astate, &safe, sizeof(packet_t));
	// Set the motors to those safe values
	tank_drive();
	// Dont send data until we recieve something
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	while(1){
		if(SerComm.available()){
			if(SerComm.read()=='[')
			break;
		}
		wdt_reset();
	}
	digitalWrite(13, LOW);
}

void fire_fsm(){
	if(fire==0){
		spinner.writeMicroseconds(1500);
		return;
	}
	//Increase state counter
	fire++;
	//Start spinners
	spinner.writeMicroseconds(1700);
	//T = 1 second, FIRE!!
	if(fire == (1000/TICK_RATE)) {
		feed.write(0);
	}
	else if(fire == (1750/TICK_RATE)) {
		//T = 1.5 seconds, retract and spin down
		feed.write(180);
	}
	else if(fire == (2500/TICK_RATE)) {
		//Reset
		fire=0;
		spinner.writeMicroseconds(1500);
	}
}

void loop(){
	//Every line sent to the computer gets us a new state
	wdt_reset();
	print_data();
	comm_parse();
	if(getButton(7)) {
		//arm up
		armpos+=2;
	}
	if(getButton(5)) {
		//arm down
		armpos-=2;
	}
	//Limit value before setting
	armpos = constrain(armpos,1000,2000);
	arm.writeMicroseconds(armpos);

	if(getButton(1) && fire==0){
		fire=1;
	}
	fire_fsm();
	if(getButton(3) && (millis()-last_s > 500)){
		//laser
		last_s=millis();
		laser=(laser+1)%2;
		digitalWrite(LASER_PIN,laser);
	}

	tank_drive();

	
	// //limits data rate
	delay(TICK_RATE);
}

void arcade_drive() {
	int zeroed_power = ((int)(astate->stickRX) - 128);
	int zeroed_turn =  ((int)(astate->stickLY) - 128);

	// Square Inputs
	zeroed_power = (zeroed_power * abs(zeroed_power)) / 127;
	zeroed_turn =  (zeroed_turn * abs(zeroed_turn)) / 127;

	int left_out = (zeroed_power + (zeroed_turn));
	int right_out = -1 * (zeroed_power - (zeroed_turn));

	write_motors(left_out, right_out);
}

void tank_drive(){
	int left_out = ((int)(astate->stickLY) - 128);
	int right_out = -1 * ((int)(astate->stickRY) - 128);
	
	// Square inputs
	left_out = (left_out * abs(left_out)) / 127;
	right_out = (right_out * abs(right_out)) / 127;
	
	write_motors(left_out, right_out);
}

void write_motors(int left_out, int right_out) {
	leftOutDebug = left_out;
	write_serial_motors(&left, left_out, '1');
	write_serial_motors(&right, right_out, '1');
	// If both motors on one controller are written to write after the other,
	// the controller will miss the second write.  If you want to disable one side,
	// uncomment the delay here.
	// delay(1);
	write_serial_motors(&left, left_out, '2');
	write_serial_motors(&right, right_out, '2');
	// delay(1);
}

// Speed is -128 to 127 where 0 is stopped.
void write_serial_motors(SoftwareSerial *controller, int speed, int motorID) {
	char controlStr[5];
	strcpy(controlStr, "1f0\r");
	controlStr[0] = motorID;
	if (speed < 0) {
		controlStr[1] = 'r';
		speed = -1 * speed;
	}
	if (speed > 127) {
		speed = 127;
	}
	controlStr[2] = (uint8_t)map(speed, 0, 127, '0', '9');
	controller->print(controlStr);
	if (controller == &left) {
		strcpy(leftMotorDebug, controlStr);
		leftMotorDebug[3] = '\0';
	}
	else {
		strcpy(rightMotorDebug, controlStr);
		rightMotorDebug[3] = '\0';
	}
}
