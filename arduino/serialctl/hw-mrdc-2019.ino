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
  //pinMode(AUX_PIN, OUTPUT);
  //pinMode(ROPE_PIN, OUTPUT);
  pinMode(EXTEND_PIN, OUTPUT);
  pinMode(ACTUATOR_PIN, OUTPUT);
  
  //spinwheel.attach(SPINNER_PIN);
  //spinwheel_aux.attach(AUX_PIN);

  //spinwheel.writeMicroseconds(OFF_SPEED);
  //spinwheel_aux.writeMicroseconds(FORWARD_CMD);

  //rope.attach(ROPE_PIN);

  extend.attach(EXTEND_PIN);

  actuator.attach(ACTUATOR_PIN);
}

void manipulator_spinwheel() {
  if (get_button(1) && !get_button(2) /*&& spinwheel_state != 1*/) {
    //SerComm.write("Spinning Forward ");
    spinwheel.writeMicroseconds(LAUNCH_SPEED);
    spinwheel_aux.writeMicroseconds(FORWARD_CMD);
    // don't waste processor time if we can help it
    //spinwheel_state = 1;
  } else if (get_button(2) && !get_button(1) /*&& spinwheel_state != -1*/) {
    //SerComm.write("Spinning Reversed ");
    spinwheel.writeMicroseconds(LAUNCH_SPEED);
    spinwheel_aux.writeMicroseconds(REVERSE_CMD);
    //spinwheel_state = -1;
  } else if (/*spinwheel_state != 0*/ true) {
    spinwheel.writeMicroseconds(OFF_SPEED);
    spinwheel_aux.writeMicroseconds(OFF_SPEED);
    //spinwheel_state = 0;
  }
}

void manipulator_rope() {
  if (get_button(0) /*&& rope_state == 0*/) {
    //SerComm.write("Pulling rope ");
    rope.writeMicroseconds(1700);
    //rope_state = 1;
  } else if (/*rope_state == 1*/ true) {
    rope.writeMicroseconds(1500);
    //rope_state = 0;
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
