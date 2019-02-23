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
long last_s=0;
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
	SerComm.begin(9600);
	comm_init();
	init_drive();
	// Copy safe values over the current state
	memcpy(astate, &safe, sizeof(packet_t));
	// Set the motors to those safe values
	tank_drive();
	// Dont send data until we recieve something
	pinMode(12, OUTPUT);
	digitalWrite(12, HIGH);
	while(1){
		if(SerComm.available()){
			if(SerComm.read()=='[')
			break;
		}
		wdt_reset();
	}
	digitalWrite(12, LOW);
}

void loop(){
	//Every line sent to the computer gets us a new state
	wdt_reset();
	print_data();
	comm_parse();
	
	tank_drive();

	// limits data rate
	delay(TICK_RATE);
}

void arcade_drive() {
	int zeroed_power = ((int)(astate->stickRX) - 128);
	int zeroed_turn =  ((int)(astate->stickLY) - 128);

	// Square Inputs
	zeroed_power = (zeroed_power * abs(zeroed_power)) / 127;
	zeroed_turn =  (zeroed_turn * abs(zeroed_turn)) / 127;

	int left_out =  (zeroed_power + (zeroed_turn));
	int right_out = -1* (zeroed_power - (zeroed_turn));

	drive_left(left_out);
	drive_right(right_out);
}

void tank_drive(){
	int left_out =  ((int)(astate->stickLY) - 128);
	int right_out = -1* ((int)(astate->stickRY) - 128);
	
	// Square inputs
	left_out = (left_out * abs(left_out)) / 127;
	right_out = (right_out * abs(right_out)) / 127;
	
	drive_left(left_out);
	drive_right(right_out);
}
