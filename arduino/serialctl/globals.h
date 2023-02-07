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
#define FRONT_LEFT 4
#define FRONT_RIGHT 5
#define REAR_LEFT 6
#define REAR_RIGHT 7
#define FAILTIME 100

// Inputs
#define JOYSTICK_PAD_UP 12
#define JOYSTICK_PAD_RIGHT 13
#define JOYSTICK_PAD_DOWN 14
#define JOYSTICK_PAD_LEFT 15

//Configurable ESTOP 
//MUST ALSO UPDATE IN ui.c

#define ESTOP_ENABLE 8
#define ESTOP_DISABLE 9
