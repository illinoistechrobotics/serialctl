#include <Arduino.h>
#include "base64.h"
#include "crc16.h"
#include "globals.h"
#include "zserio.h"

char comm_ok;
long ptime;
void comm_init() {
  ptime = 0;
  cs = COMM_WAIT;
  astate = &pA;
  incoming = &pB;
  comm_ok=0;
}
void comm_parse() {
  packet_t *tmp;
  static char encstr[2 + B64_ENC_LEN(sizeof(packet_t))];
  static unsigned int recvcount=0;
  char inc;
  uint16_t crc;
  while (SerComm.available()) {
    inc = SerComm.read();
   // SerCommDbg.print(cs,DEC);
    if (inc == SFRAME) {
   //   SerCommDbg.println("Sframe");
      cs = COMM_RECV;
      recvcount = 0;
    //  SerCommDbg.print(cs,DEC);
    } else if (inc == EFRAME && cs == COMM_RECV) {
      cs = COMM_COMPLETE;
      //SerCommDbg.println("Eframe"); 
      //length check
      if(recvcount != B64_ENC_LEN(sizeof(packet_t))){
        //SerCommDbg.println("Length");
        cs = COMM_INVALID;
      }
      
      //Check decoded size in case of base64 error
      if(base64_dec_len(encstr, B64_ENC_LEN(sizeof(packet_t))) != sizeof(packet_t)){
        //SerCommDbg.println("B64");
        cs = COMM_INVALID;
      }
      
    } else if (cs == COMM_RECV) {
      //populate buffer, preventing overflows from dropped start or end bytes
      if (recvcount >= B64_ENC_LEN(sizeof(packet_t))) {
        //SerCommDbg.println("Overflow");
        cs = COMM_INVALID;
      } else {
        encstr[recvcount] = inc;
        recvcount++;
      }
    }
    
    if(cs==COMM_COMPLETE){
      //SerCommDbg.println("Comm Complete");
      //Base64 decode
      base64_decode((char *)incoming, encstr, B64_ENC_LEN(sizeof(packet_t)));
      //Evaluate CRC16 and flip pointers if valid
      crc = compute_crc((char *)incoming, sizeof(packet_t)-sizeof(uint16_t));
      if(crc == ntohs(incoming->cksum)){
//        SerCommDbg.println("vaild");
        cs=COMM_VALID;
        ptime=millis();
        tmp=astate;
        astate=incoming;
        incoming=tmp;
        comm_ok=1;
        digitalWrite(13,HIGH);
      } else{
        cs=COMM_INVALID;
        SerCommDbg.println("Invalid RC packet received!");
      }
    }
  }
  
  if(millis()-ptime > FAILTIME){
    digitalWrite(13,LOW);
    //Been too long, copy safe state over active one
    memcpy(astate,&safe,sizeof(packet_t));
    comm_ok=0;
  }
}
