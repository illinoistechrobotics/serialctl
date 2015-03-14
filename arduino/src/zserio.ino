#include "base64.h"
#include "crc16.h"
#include "packet.h"
#include "globals.h"

uint16_t crc;
char encstr[2 + B64_ENC_LEN(sizeof(packet_t))];
unsigned int recvcount;
long ptime;
void comm_init() {
  ptime = 0;
  cs = COMM_WAIT;
  astate = &pA;
  incoming = &pB;
  recvcount = 0;
}
void comm_parse() {
  packet_t *tmp;
  char inc;
  while (SerComm.available()) {
    inc = SerComm.read();
    if (inc == SFRAME) {
      cs = COMM_RECV;
      recvcount = 0;
    } else if (inc == EFRAME && cs == COMM_RECV) {
      cs = COMM_COMPLETE;
      //length check
      if(recvcount != B64_ENC_LEN(sizeof(packet_t))){
        cs = COMM_INVALID;
      }
      
      //Check decoded size in case of base64 error
      if(base64_dec_len(encstr, B64_ENC_LEN(sizeof(packet_t))) != sizeof(packet_t)){
        cs = COMM_INVALID;
      }
      
    } else if (cs == COMM_RECV) {
      //populate buffer, preventing overflows from dropped start or end bytes
      if (recvcount >= B64_ENC_LEN(sizeof(packet_t))) {
        cs = COMM_INVALID;
      } else {
        encstr[recvcount] = inc;
        recvcount++;
      }
    }
    
    if(cs==COMM_COMPLETE){
      //SerComm.println("Comm recieve");
      //Base64 decode
      base64_decode((char *)incoming, encstr, B64_ENC_LEN(sizeof(packet_t)));
      //Evaluate CRC16 and flip pointers if valid
      crc = compute_crc((char *)incoming, sizeof(packet_t)-sizeof(uint16_t));
      if(crc == ntohs(incoming->cksum)){
        //SerComm.println("vaild");
        cs=COMM_VALID;
        ptime=millis();
        tmp=astate;
        astate=incoming;
        incoming=tmp;
      } else{
        cs=COMM_INVALID;
       // SerComm.println("Invalid");
      }
    }
  }
  
  if(millis()-ptime > 120){
    //Been too long, copy safe state over active one
    memcpy(astate,&safe,sizeof(packet_t));
    cs=COMM_WAIT;
    recvcount = 0;
    SerComm.println("|-|-[FAILSAFE]-|-|");
  }
}
