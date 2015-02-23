#include "base64.h"
#include "crc16.h"
uint16_t crc;
char encstr[1 + B64_ENC_LEN(sizeof(packet_t))];
int recvcount;
void comm_init() {
  cs = COMM_WAIT;
  active = &pA;
  incoming = &pB;
}
void comm_parse() {
  packet_t *tmp;
  char inc;
  while (SerComm.available()) {
    inc = SerComm.read();
    if (inc == SFRAME) {
      cs = COMM_RECV;
      recvcount = 0;
    } else if (inc == EFRAME) {
      cs = COMM_COMPLETE;
      //length check
      if(recvcount != B64_ENC_LEN(sizeof(packet_t))){
        cs = COMM_INVALID;
      }
      encstr[recvcount]= 0x00;
      
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
      //Base64 decode
      base64_decode((char *)incoming, encstr, recvcount);
      //Evaluate CRC16 and flip pointers if valid
      crc = compute_crc((char *)incoming, sizeof(packet_t)-sizeof(uint16_t));
      if(crc = incoming->cksum){
        cs=COMM_VALID;
        tmp=active;
        active=incoming;
        incoming=tmp;
      } else{
        cs=COMM_INVALID;
      }
    }
  }
}
