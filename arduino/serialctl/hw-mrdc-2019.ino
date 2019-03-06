// hw-mrdc-2018
// MRDC Manipulator 2018
// Taylor Berg, 3-9-18

#include "hw.h"
#include "globals.h"
 
// keep states so we aren't wasting too many cycles
int vac_running = 0;
//int rope_state = 0;
int actuator_state = 0;
//int winch_state = 0;

void manipulator_setup() {
  pinMode(VAC_PIN, OUTPUT);
  pinMode(BOWLING_BALL_PIN2, OUTPUT);
  pinMode(BOWLING_BALL_PIN1, OUTPUT);
  pinMode(EXTEND_PIN, OUTPUT);
  pinMode(ACTUATOR_PIN, OUTPUT);
  
  //spinwheel.attach(SPINNER_PIN);
  //spinwheel_aux.attach(AUX_PIN);

  //spinwheel.writeMicroseconds(OFF_SPEED);
  //spinwheel_aux.writeMicroseconds(FORWARD_CMD);
  vac.attach(VAC_PIN);
  bowling_ball1.attach(BOWLING_BALL_PIN1);
  bowling_ball2.attach(BOWLING_BALL_PIN2);

  extend.attach(EXTEND_PIN);

  actuator.attach(ACTUATOR_PIN);
}


void manipulator_bowling_ball() {
  if ((get_button(2))/*&& actuator_state != 1*/) {
    //SerComm.write("Moving arm up ");
    bowling_ball1.writeMicroseconds(2000);
    bowling_ball2.writeMicroseconds(2000);
    //actuator_state = 1;
  } else if (get_button(3) /*&& actuator_state != -1*/) {
    //SerComm.write("Moving arm down ");
    bowling_ball1.writeMicroseconds(1000);
    
    //actuator_state = -1;
  } else if (get_button(0)){

    bowling_ball2.writeMicroseconds(1000);
    }else if (/*actuator_state != 0*/ true) {
    bowling_ball1.writeMicroseconds(1500);
    bowling_ball2.writeMicroseconds(1500);
    //actuator_state = 0;
  }
}

void manipulator_extend() {
  if ((get_button(6))/*&& actuator_state != 1*/) {
    //SerComm.write("Moving arm up ");
    extend.writeMicroseconds(1000);
    //actuator_state = 1;
  } else if ((get_button(4)) /*&& actuator_state != -1*/) {
    //SerComm.write("Moving arm down ");
    extend.writeMicroseconds(2000);
    //actuator_state = -1;
  } else if (/*actuator_state != 0*/ true) {
    extend.writeMicroseconds(1500);
    //actuator_state = 0;
  }
}

void manipulator_actuator() {
  if ((get_button(5))/*&& actuator_state != 1*/) {
    //SerComm.write("Moving arm up ");
    actuator.writeMicroseconds(1000);
    //actuator_state = 1;
  } else if ((get_button(7)) /*&& actuator_state != -1*/) {
    //SerComm.write("Moving arm down ");
    actuator.writeMicroseconds(2000);
    //actuator_state = -1;
  } else if (/*actuator_state != 0*/ true) {
    actuator.writeMicroseconds(1500);
    //actuator_state = 0;
  }
}

void manipulator_vacuum() {
 
  if (get_button(1)/*&& actuator_state != -1*/) {
    //SerComm.write("Moving arm down ");
    digitalWrite(VAC_PIN,HIGH);
    //actuator_state = -1;
  } else{
    digitalWrite(VAC_PIN,LOW);
    //actuator_state = 0;
  }
}
