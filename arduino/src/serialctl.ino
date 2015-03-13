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
Servo spinner, arm, winch;
packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
int speed,av;
long last_p,last_s=0,usec;

#define SerComm Serial1
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#define ALI1 7
#define BLI1 8
#define AHI1 22
#define BHI1 23
#define ALI2 11
#define BLI2 12
#define AHI2 24
#define BHI2 25
#define drive_right(x) drive_osmc(x,0,ALI1,BLI1,AHI1,BHI1)
#define drive_left(x) drive_osmc(x,0,ALI2,BLI2,AHI2,BHI2)
#define DEADBAND_HALF_WIDTH 5

#define WATCHDOG_

#ifdef WATCHDOG_
#include <avr/wdt.h>      //watchdog library timer loop resets the watch dog
#endif
int getButton(int num){
        if(num<=7){
                return (astate->btnlo >> num) & 0x01;
        } else if(num>7 && num <= 15){
                return (astate->btnhi >> (num - 8)) & 0x01;
        } else {
                return 0;
        }
}
void setup() {
  #ifdef WATCHDOG_
  wdt_enable(WDTO_250MS);  //Set 250ms WDT 
  wdt_reset();             //watchdog timer reset 
  #endif
  spinner.attach(SPINNER_PIN);
  spinner.writeMicroseconds(0);
  arm.attach(ARM_PIN);
  arm.writeMicroseconds(1500);
  winch.attach(WINCH_PIN);
  winch.writeMicroseconds(1500);
  //Initialize safe to safe values!!
  safe.stickX = 127;
  safe.stickY = 127;
  safe.btnhi = 0;
  safe.btnlo = 0;
  safe.cksum = 0b1000000010001011;
  SerComm.begin(115200);
  comm_init();
  init_pins();
  last_p = millis();
  pinMode(REVERSE_PIN, OUTPUT);
  pinMode(43, INPUT);
  pinMode(42, INPUT);
  digitalWrite(43, HIGH);
  digitalWrite(42, HIGH);
  pinMode(13, OUTPUT);
  drive_left(0);
  drive_right(0);
  speed=0;
  //copy safe values over the current state
  memcpy(astate, &safe, sizeof(packet_t));
}
void loop(){
  //Every line sent to the computer gets us a new state
  wdt_reset();
  print_data();
  comm_parse();
//arm
  av=1500;
  //check for invalid states
  if((getButton(5) ^ getButton(7))){
    //both up and down buttons at same time is invalid
    if(getButton(5) && get_arm_interlock() == 1){
        av=1000;
    }
    else if(getButton(7)){
        av=2000;
        }
    }
  //write out value
  if(get_arm_interlock() == -1){
    arm.writeMicroseconds(1500);
  }else{
    arm.writeMicroseconds(av);
    }
    //winch
 if((getButton(0) ^ getButton(2))){
    //both up and down buttons at same time is invalid
    if(getButton(0)){
        winch.writeMicroseconds(1250);
        }
    if(getButton(2)){
        winch.writeMicroseconds(1750);
        }
    } else{
    winch.writeMicroseconds(1500);
    }


    if((millis()-last_s > 500) && (getButton(3) || getButton(1))){
        if(getButton(1)){
	  speed--;
        }
        if(getButton(3)){
	  speed++;
        }
        speed=constrain(speed,-3,3);
        last_s = millis();
        }
        //SerComm.println(speed);
   if(cs != COMM_WAIT){
            if(speed > 0){
            digitalWrite(REVERSE_PIN,HIGH);
            } else if(speed<0){
            digitalWrite(REVERSE_PIN,LOW);
            }
           switch(abs(speed)){
                   case 1:
                           usec = 1200;
                           break;
                   case 2:
                           usec = 1700;
                           break;
                   case 3:
                           usec = 2200;
                           break;
                   default:
                           usec = 0;
            }
   }else{
   usec=0;
   }
    spinner.writeMicroseconds(usec);
    
  //Runs this every 500ms
  if(millis()-last_p >= 500){
    //no compressor
    
    last_p=millis();
  }
  tank_drive();
  
  //limits data rate
  delay(75);
}
void tank_drive(){
  int power_out = 0;
  int turn_out  = 0;
  int zeroed_power =    ((int)(astate->stickX) - 127);
  int zeroed_turn =     -1*((int)(astate->stickY) - 127);
  
  if(abs(zeroed_power) > DEADBAND_HALF_WIDTH){
    if(zeroed_power>0){
      power_out = zeroed_power - DEADBAND_HALF_WIDTH;
    } else {
      power_out = zeroed_power + DEADBAND_HALF_WIDTH;
    }
  }
  if(abs(zeroed_turn) > DEADBAND_HALF_WIDTH){
    if(zeroed_turn>0){
      turn_out = zeroed_turn - DEADBAND_HALF_WIDTH;
    } else {
      turn_out = zeroed_turn + DEADBAND_HALF_WIDTH;
    }
  }
  int left_out =     power_out + (turn_out/8);
  int right_out = -1*power_out + (turn_out/8);

  //apply turbo mode
  if(getButton(6)){
    left_out  *= 2;
    right_out *= 2;
  }
  if(getButton(4)){
    left_out  /= 2;
    right_out /= 2;
  }

  drive_left(left_out);
  drive_right(right_out);
}
