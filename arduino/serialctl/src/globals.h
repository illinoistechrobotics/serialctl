#pragma once
#include "packet.h"

extern packet_t pA, pB, safe;
extern packet_t *astate, *incoming;
extern comm_state cs;
extern long last_p;
extern char comm_ok, left_enabled, right_enabled;
extern double pidLeftP, pidLeftI, pidLeftD, pidRightP, pidRightI, pidRightD;
extern double leftIn, leftOut, leftSet, rightIn, rightOut, rightSet;

/*Configuration globals */
#define PRINTMOTORS  //Should all motor power power values be printed to SerCommDbg?
#define WATCHDOG_   //Should the hardware watchdog timer be set and used (strongly advised for safety!)?
#define SerComm Serial1   //Serial port connected to Xbee
#define SerCommDbg Serial   //Serial port for debugging info
#define BOOTSTR "--ITR Goliath 4.2023, Copyright 2023 (c) Illinois Tech Robotics--"   //String to print on SerCommDbg on initialization


/* 2017 PID tunings:
 *  Printing PID tunings:
* Left P 0.03500000
* Left I 0.40000000
* Left D 0.00003500
* Right P 0.03000000
* Right I 0.40000000
* Right D 0.00003500

 */

#define RESET_BUTTON 9  //Button to use for system reset, requires watchdog timer to function

/* Sabertooth input mappings */
#define WINCH_MOTOR 1
#define COMPRESSOR_MOTOR 2


// pins for motor controller 1 (right)
#define ALI1 7
#define BLI1 8
#define AHI1 22
#define BHI1 23
#define DENABLE1 28
#define DREADY1 30

// and 2 (left)
#define ALI2 11
#define BLI2 12
#define AHI2 24
#define BHI2 25
#define DENABLE2 29
#define DREADY2 31

#define DEADBAND_HALF_WIDTH 10  // Control input deadband radius
#define FAILTIME 200    //Failsafe timeout in milliseconds


// Input button nmubering
#define DIAMOND_LEFT 0
#define DIAMOND_DOWN 1
#define DIAMOND_RIGHT 2
#define DIAMOND_UP 3
#define SHOULDER_TOP_LEFT 4
#define SHOULDER_TOP_RIGHT 5
#define SHOULDER_BOTTOM_LEFT 6
#define SHOULDER_BOTTOM_RIGHT 7
#define SMALL_LEFT 8
#define SMALL_RIGHT 9
//10 and 11 are probably the stick buttons
//but we haven't checked recently
#define DPAD_UP 12
#define DPAD_RIGHT 13
#define DPAD_DOWN 14
#define DPAD_LEFT 15

/* Internal macros, do not modify for config purposes */
#define PID_SERIAL_BUFFER_SIZE 20

#define try_enable_right(e) try_enable_osmc(e,DENABLE1,DREADY1,ALI1,BLI1,AHI1,BHI1)
#define try_enable_left(e) try_enable_osmc(e,DENABLE2,DREADY2,ALI2,BLI2,AHI2,BHI2)
#define drive_right(e,x) drive_osmc(e,DENABLE1,x,0,ALI1,BLI1,AHI1,BHI1)
#define drive_left(e,x) drive_osmc(e,DENABLE2,x,0,ALI2,BLI2,AHI2,BHI2)

#define DEBUGPRINT(x) SerCommDbg.println(x)
