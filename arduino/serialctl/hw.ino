#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>

Servo fl, fr, rl, rr, arm_key, arm_ball, servo_gripper;

boolean estop_state = false;

void init_pins() {
  talon_init();
}
void init_servos(HardwareSerial &serial_port, int control_pin, int servo_id) {

}
void print_data() {
  if (comm_ok == 0) {
    //Print failsafe notice
    SerComm.print("-FS- ");
  }
  SerComm.println("Roslund");
}
uint8_t get_button(int num) {
  if (num <= 7) {
    return (astate->btnlo >> num) & 0x01;
  } else if (num > 7 && num <= 15) {
    return (astate->btnhi >> (num - 8)) & 0x01;
  } else {
    return 0;
  }
}
void talon_init() {
  fl.attach(FRONT_LEFT);
  fr.attach(FRONT_RIGHT);
  rl.attach(REAR_LEFT);
  rr.attach(REAR_RIGHT);
  fl.writeMicroseconds(1500);
  fr.writeMicroseconds(1500);
  rr.writeMicroseconds(1500);
  rl.writeMicroseconds(1500);
}
void drive_left(int power) {
  power = map(constrain(power, -127, 127), -127, 127, 1000, 2000);
  fl.writeMicroseconds(power);
  rl.writeMicroseconds(power);
}

void drive_right(int power) {
  power = map(constrain(power, -127, 127), -127, 127, 1000, 2000);
  fr.writeMicroseconds(power);
  rr.writeMicroseconds(power);
}

bool is_estop() {
  if (getButton(ESTOP_DISABLE)) {
    estop_state = false;
  }
  if (getButton(ESTOP_ENABLE)) {
    estop_state = true;
  }
  if (estop_state) {
    estop();
    return true;
  }
  return false;
}

void estop() {
  fl.writeMicroseconds(1500);
  fr.writeMicroseconds(1500);
  rr.writeMicroseconds(1500);
  rl.writeMicroseconds(1500);
}
