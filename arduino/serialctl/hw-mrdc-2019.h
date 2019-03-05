#pragma once



#define SPINNER_PIN 2
#define AUX_PIN 3

#define EXTEND_PIN 7

#define BOWLING_BALL_PIN1 8
#define BOWLING_BALL_PIN2 9

#define ACTUATOR_PIN 6


#define LAUNCH_SPEED 1250
#define OFF_SPEED 1000
#define FORWARD_CMD 1000
#define REVERSE_CMD 1900

Servo spinwheel;
Servo bowling_ball2;

Servo extend;
Servo bowling_ball1;
Servo actuator;

void manipulator_setup();
void manipulator_spinwheel();
void manipulator_bowling_ball();
void manipulator_extend();
void manipulator_actuator();
