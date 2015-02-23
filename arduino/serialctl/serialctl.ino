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

packet_t pA, pB;
packet_t *active, *incoming;
comm_state cs;

#define SerComm Serial

void setup(){
  SerComm.begin(115200);
  comm_init();
  pinMode(13,OUTPUT);
}
void loop(){
  comm_parse();
  SerComm.println("Hello");
  delay(10);
  if(active->stickX < 100){
    digitalWrite(13,HIGH);
  } else {
    digitalWrite(13,LOW);
  }
}
