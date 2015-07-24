#pragma once
#include <Servo.h>
/*
 * This file is where robot specific hardware code should be defined.
 * For example, below is the code to interface with the pneumatics and
 * Open Source Motor Controller (OSMC) on Goliath
 *
 */

#define P_SENSOR A2
#define CURRENT_OFFSET -2.5
#define PRESSURE_OFFSET 161
#define MIN_PRESS 80
#define MAX_PRESS 100
#define CURRENT_RIGHT A0
#define CURRENT_LEFT A1

#define PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/2.048)

#define ALI1 7
#define BLI1 8
#define AHI1 22
#define BHI1 23
#define ALI2 11
#define BLI2 12
#define AHI2 24
#define BHI2 25
#define drive_right(x) drive_osmc(x,1,ALI1,BLI1,AHI1,BHI1)
#define drive_left(x) drive_osmc(x,1,ALI2,BLI2,AHI2,BHI2)

extern Servo spinner, arm, winch;

void init_pins();
void print_data();
void osmc_init();
void fast_pwm();
void drive_osmc(int rawpower, unsigned short brake, unsigned short ali, unsigned short bli, unsigned short ahi, unsigned short bhi);
int get_arm_interlock();
void hw_init();
