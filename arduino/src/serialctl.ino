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

packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
long last_p;

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

void setup() {
  #ifdef WATCHDOG_
  wdt_enable(WDTO_250MS);  //Set 250ms WDT 
  wdt_reset();             //watchdog timer reset 
  #endif
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
  pinMode(13, OUTPUT);
  drive_left(0);
  drive_right(0);
  //copy safe values over the current state
  memcpy(astate, &safe, sizeof(packet_t));
}
void loop(){
  //Every line sent to the computer gets us a new state
  wdt_reset();
  print_data();
  comm_parse();
  //Runs this every 500ms
  if(millis()-last_p >= 500){
    pumpAir();
    last_p=millis();
  }
  tank_drive();
  
  //limits data rate
  delay(75);
}
void tank_drive(){
  int right_out = 0;
  int left_out  = 0;
  int zeroed_left =     ((int)(astate->stickX)) - 127;
  int zeroed_right = -1*((int)(astate->stickY)  - 127);
  if(abs(zeroed_left) > DEADBAND_HALF_WIDTH){
    if(zeroed_left>0){
      left_out = zeroed_left - DEADBAND_HALF_WIDTH;
    } else {
      left_out = zeroed_left + DEADBAND_HALF_WIDTH;
    }
  }
  if(abs(zeroed_right) > DEADBAND_HALF_WIDTH){
    if(zeroed_left>0){
      right_out = zeroed_right - DEADBAND_HALF_WIDTH;
    } else {
      right_out = zeroed_right + DEADBAND_HALF_WIDTH;
    }
  }
  drive_left(left_out);
  drive_right(right_out);
}
