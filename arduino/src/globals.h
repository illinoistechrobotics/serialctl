#pragma once

extern packet_t pA, pB, safe;
extern packet_t *astate, *incoming;
extern comm_state cs;
extern long last_p;
extern char comm_ok;
#define SerComm Serial
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#define DEADBAND_HALF_WIDTH 5
#define FRONT_LEFT 6
#define FRONT_RIGHT 9
#define REAR_LEFT 10
#define REAR_RIGHT 11
#define ARM_UP 7
#define ARM_DOWN 8
#define MANIP_KICK 12
#define MANIP_GRIP 13
#define FAILTIME 100
