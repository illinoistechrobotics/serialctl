#include "base64.h"
#include "crc16.h"
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)


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
      //SerComm.println("Comm recieve");
      //Base64 decode
      base64_decode((char *)incoming, encstr, recvcount);
      //Evaluate CRC16 and flip pointers if valid
      crc = compute_crc((char *)incoming, sizeof(packet_t)-sizeof(uint16_t));
      if(crc = ntohs(incoming->cksum)){
        //SerComm.println("vaild");
        cs=COMM_VALID;
        tmp=active;
        active=incoming;
        incoming=tmp;
      } else{
        cs=COMM_INVALID;
       // SerComm.println("Invalid");
      }
    }
  }
}
