#pragma once

extern packet_t pA, pB;
extern const packet_t safe;
extern packet_t *astate, *incoming;
extern comm_state cs;
extern long last_p;
extern int speed;
#define SerComm Serial2
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#define DEADBAND_HALF_WIDTH 0
#define SPINNER_PIN 2
#define ARM_PIN 3
#define WINCH_PIN 4
#define REVERSE_PIN 41
