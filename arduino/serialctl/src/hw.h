#pragma once
#include <Arduino.h>
#include <Servo.h>
#include <Sabertooth.h>


#define CURRENT_RIGHT A0
#define CURRENT_LEFT A1

#define SABERTOOTH12 Serial2
#define SABERTOOTH34 Serial3 

#define LINAC_PRECISION 64
#define ARM_LINAC_PRECISION 96
#define PRESSURE_OFFSET 160

#define DOOR_LATCH 5
#define VACUUM_RELAY 35

extern Servo door_latch;

extern bool play_game;
extern Sabertooth ST12;
extern Sabertooth ST34;

extern float current_offset_left, current_offset_right;
extern int offset_measured;

void init_pins();
void measure_offset();
void print_data();
void osmc_init();
void fast_pwm();
void compressor_ctl();
char try_enable_osmc(char enabled, char enablepin, char readypin, char ali, char bli, char ahi, char bhi);
void drive_osmc(char enabled, char enablepin, int rawpower, char brake, char ali, char bli, char ahi, char bhi);

/* Pneumatics */
extern bool pumping, autp_pump_enable;
extern float psi;
#define P_SENSOR A2
#define MIN_PRESS 75
#define MAX_PRESS 100

#define GRIP_VALVE 40
#define ADC2PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/1.024)
