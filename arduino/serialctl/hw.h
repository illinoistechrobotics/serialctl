#pragma once

#define CURRENT_OFFSET -2.5

#define CURRENT_RIGHT A0
#define CURRENT_LEFT A1
#define SABERTOOTH12 Serial2
#define SABERTOOTH34 Serial3

void init_pins();
void print_data();
void osmc_init();
void fast_pwm();
void drive_osmc(int rawpower, unsigned short brake, unsigned short ali, unsigned short bli, unsigned short ahi, unsigned short bhi);
int get_arm_interlock();
