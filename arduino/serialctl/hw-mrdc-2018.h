#pragma once

#define SPINNER_PIN 50
#define AUX_PIN 51

#define WINCH_PIN 49

#define ROPE_PIN 48

#define LAUNCH_SPEED 1250
#define OFF_SPEED 1000
#define FORWARD_CMD 1000
#define REVERSE_CMD 1900

Servo spinwheel;
Servo spinwheel_aux;

Servo winch;
Servo rope;

void manipulator_setup();
void manipulator_spinwheel();
