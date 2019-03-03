#pragma once



#define SPINNER_PIN 2
#define AUX_PIN 3

#define WINCH_PIN 4

#define ROPE_PIN 6

#define ACTUATOR_PIN 6

#define LAUNCH_SPEED 1250
#define OFF_SPEED 1000
#define FORWARD_CMD 1000
#define REVERSE_CMD 1900

Servo spinwheel;
Servo spinwheel_aux;

Servo winch;
Servo rope;
Servo actuator;

void manipulator_setup();
void manipulator_spinwheel();
void manipulator_rope();
void manipulator_winch();
void manipulator_actuator();
