#pragma once


#define CURRENT_RIGHT A0
#define CURRENT_LEFT A1

#define SABERTOOTH12 Serial2
#define SerCommDynamixel Serial3
#define DynamixelDataDir 6
#define DynamixelBaud 19200
#define GRIP_SERVO_ID 1
#define WRIST_SERVO_ID 2
#define WRIST_SPEED 0x0FF
#define CLAW_SERVO_PIN 2
#define CLAW_MAX 75
#define LINAC_PRECISION 32
#define SEC_LINAC_PRECISION 64



float current_offset_left, current_offset_right;
int offset_measured = 0;

void init_pins();
void measure_offset();
void print_data();
void osmc_init();
void fast_pwm();
char try_enable_osmc(char enabled, char enablepin, char readypin, char ali, char bli, char ahi, char bhi);
void drive_osmc(char enabled, char enablepin, int rawpower, char brake, char ali, char bli, char ahi, char bhi);

//Unused but kept for documentation purposes
/*
 * float psi;
 * #define P_SENSOR A2
 * #define GRIP_VALVE 40
 * #define ADC2PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/1.024)
 */

