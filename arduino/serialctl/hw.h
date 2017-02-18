#pragma once
#define P_SENSOR A2
#define PRESSURE_OFFSET 161
#define MIN_PRESS 60    
#define MAX_PRESS 80
#define CURRENT_RIGHT A0
#define CURRENT_LEFT A1
#define SABERTOOTH12 Serial2
#define SABERTOOTH34 Serial3
#define GRIP_VALVE 40

#define ADC2PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/1.024)

float current_offset_left, current_offset_right;
int offset_measured = 0;

void init_pins();
void measure_offset();
void print_data();
void osmc_init();
void fast_pwm();
void drive_osmc(int rawpower, unsigned short brake, unsigned short ali, unsigned short bli, unsigned short ahi, unsigned short bhi);

