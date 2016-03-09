#pragma once

extern packet_t pA, pB, safe;
extern packet_t *astate, *incoming;
extern comm_state cs;
extern long last_p;
extern char leftMotorDebug[5], rightMotorDebug[5];
extern int leftOutDebug;
#define SerComm Serial
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#define TICK_RATE 50
#define DEADBAND_HALF_WIDTH 5
#define FRONT_LEFT 9
#define FRONT_RIGHT 8
#define REAR_LEFT 11
#define REAR_RIGHT 10
#define CLAW_PIN 6
#define CLAW_OPEN_MICROSECS 1500
#define CLAW_CLOSED_MICROSECS 1000