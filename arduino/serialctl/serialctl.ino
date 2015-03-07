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
#define WATCHDOG_

#ifdef WATCHDOG_
#include <avr/wdt.h>      //watchdog library timer loop resets the watch dog
#endif
packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
long last_p;

#define SerComm Serial
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

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
  //Control logic goes here
  
  if (astate->stickX < 100) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  //limits data rate
  delay(10);
}
