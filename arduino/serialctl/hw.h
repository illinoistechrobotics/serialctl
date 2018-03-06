#pragma once


#define CURRENT_0 A0
#define CURRENT_120 A1
#define CURRENT_240 A2

#define SABERTOOTH12 Serial2

#define LINAC_PRECISION 64
#define ARM_LINAC_PRECISION 96

#define PRESSURE_OFFSET 160



float current_offset_0, current_offset_120, current_offset_240;
int offset_measured = 0;

void init_pins();
void measure_offset();
void print_data();
void osmc_init();
void fast_pwm();
void compressor_ctl();
char try_enable_osmc(char enabled, char enablepin, char readypin, char ali, char bli, char ahi, char bhi);
void drive_osmc(char enabled, char enablepin, int rawpower, char brake, char ali, char bli, char ahi, char bhi);

/* Pneumatics */
float psi;
#define P_SENSOR A2
#define MIN_PRESS 75
#define MAX_PRESS 100

#define GRIP_VALVE 40
#define ADC2PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/1.024)

