#pragma once


#define CURRENT_RIGHT A0
#define CURRENT_LEFT A1

#define SABERTOOTH12 Serial2
#define SABERTOOTH34 Serial3 

#define LINAC_PRECISION 64
#define ARM_LINAC_PRECISION 96
#define PRESSURE_OFFSET 160

#define DOOR_LATCH 5
#define VACUUM_RELAY 35

Servo door_latch;

float current_offset_left, current_offset_right;
int offset_measured = 0;

void init_pins();
void measure_offset();
void print_data();
void osmc_init();
void fast_pwm();
void compressor_ctl();
char try_enable_osmc(char enabled, char enablepin, char readypin, char ali, char bli, char ahi, char bhi);
void drive_osmc(char enabled, char enablepin, int rawpower, char brake, char ali, char bli, char ahi, char bhi);
bool pumping = 0;
bool auto_pump_enable = 0;

/* Pneumatics */
float psi;
#define P_SENSOR A2
#define MIN_PRESS 75
#define MAX_PRESS 100

#define GRIP_VALVE 40
#define ADC2PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/1.024)
