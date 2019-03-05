// hw-mrdc-2018
// MRDC Manipulator 2018
// Taylor Berg, 3-9-18

#include "hw.h"
#include "globals.h"
 
// keep states so we aren't wasting too many cycles
//int spinwheel_state = 0;
//int rope_state = 0;
int actuator_state = 0;
//int winch_state = 0;

void manipulator_setup() {
  //pinMode(SPINNER_PIN, OUTPUT);
  pinMode(BOWLING_BALL_PIN2, OUTPUT);
  pinMode(BOWLING_BALL_PIN1, OUTPUT);
  pinMode(EXTEND_PIN, OUTPUT);
  pinMode(ACTUATOR_PIN, OUTPUT);
  
  //spinwheel.attach(SPINNER_PIN);
  //spinwheel_aux.attach(AUX_PIN);

  //spinwheel.writeMicroseconds(OFF_SPEED);
  //spinwheel_aux.writeMicroseconds(FORWARD_CMD);

  bowling_ball1.attach(BOWLING_BALL_PIN1);
  bowling_ball2.attach(BOWLING_BALL_PIN2);

  extend.attach(EXTEND_PIN);

  actuator.attach(ACTUATOR_PIN);
}


void manipulator_bowling_ball() {
  if ((get_button(2))/*&& actuator_state != 1*/) {
    //SerComm.write("Moving arm up ");
    bowling_ball1.writeMicroseconds(1000);
    bowling_ball2.writeMicroseconds(1000);
    //actuator_state = 1;
  } else if ((3) /*&& actuator_state != -1*/) {
    //SerComm.write("Moving arm down ");
    bowling_ball1.writeMicroseconds(2000);
    bowling_ball2.writeMicroseconds(2000);
    //actuator_state = -1;
  } else if (/*actuator_state != 0*/ true) {
    bowling_ball1.writeMicroseconds(1500);
    bowling_ball2.writeMicroseconds(1500);
    //actuator_state = 0;
  }
}

void manipulator_extend() {
  if ((get_button(JOYSTICK_PAD_LEFT))/*&& actuator_state != 1*/) {
    //SerComm.write("Moving arm up ");
    extend.writeMicroseconds(1000);
    //actuator_state = 1;
  } else if ((get_button(JOYSTICK_PAD_RIGHT)) /*&& actuator_state != -1*/) {
    //SerComm.write("Moving arm down ");
    extend.writeMicroseconds(2000);
    //actuator_state = -1;
  } else if (/*actuator_state != 0*/ true) {
    extend.writeMicroseconds(1500);
    //actuator_state = 0;
  }
}

void manipulator_actuator() {
  if ((get_button(JOYSTICK_PAD_DOWN))/*&& actuator_state != 1*/) {
    //SerComm.write("Moving arm up ");
    actuator.writeMicroseconds(1000);
    //actuator_state = 1;
  } else if ((get_button(JOYSTICK_PAD_UP)) /*&& actuator_state != -1*/) {
    //SerComm.write("Moving arm down ");
    actuator.writeMicroseconds(2000);
    //actuator_state = -1;
  } else if (/*actuator_state != 0*/ true) {
    actuator.writeMicroseconds(1500);
    //actuator_state = 0;
  }
}
