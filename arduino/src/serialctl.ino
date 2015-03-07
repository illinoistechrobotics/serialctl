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
#include "globals.h"
#include "hw.h"
#include "zserio.h"

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
