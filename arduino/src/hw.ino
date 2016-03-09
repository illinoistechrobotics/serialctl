#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>
#define LEFT_SPEEDSERVO_ADDR 1
#define RIGHT_SPEEDSERVO_ADDR 2
boolean estop_state = false;
int last_left_speed, last_right_speed;
int current_manipulator_direction = 0;
void init_pins(){
  Wire.begin();
  pinMode(MANIPULATOR_FORWARD_PIN, OUTPUT);
  pinMode(MANIPULATOR_REVERSE_PIN, OUTPUT);
  set_manipulator_dir(0);
}
int read_speed(char address){
  Wire.requestFrom(address,sizeof(int));
  byte buff[sizeof(int)];
  for(int i = 0; i < sizeof(int); ++i){
    unsigned long timeout = millis();
    while((!Wire.available()) && millis() - timeout < 100){};
    if(millis() - timeout > 100){
      SerComm.println("Speed read timeout");
    }
    buff[i] = Wire.read();
  }
  return *((int*) buff);
}
void print_speed(){
  int left_speed = read_speed(LEFT_SPEEDSERVO_ADDR);
  int right_speed = read_speed(RIGHT_SPEEDSERVO_ADDR);

  char speedline[100];
  snprintf(speedline, 100, "% 4d, % 4d, % 4d, % 4d, %lu", 
	   left_speed,last_left_speed,
	   right_speed,last_right_speed,
	   millis());
  SerComm.println(speedline);

}
void print_data(){
  if(estop_state){
    SerComm.println("ESTOP");
  } else {
    if(cs != COMM_WAIT){
      //SerComm.println("Good to roll");
      print_speed();
    }
  }
}
int getButton(int num){
        if(num<=7){
                return (astate->btnlo >> num) & 0x01;
        } else if(num>7 && num <= 15){
                return (astate->btnhi >> (num - 8)) & 0x01;
        } else {
                return 0;
        }
}
void estop(){
  Wire.beginTransmission(LEFT_SPEEDSERVO_ADDR);
  Wire.write(5);
  Wire.endTransmission();
  Wire.beginTransmission(RIGHT_SPEEDSERVO_ADDR);
  Wire.write(5);
  Wire.endTransmission();
}

void run_manipulator() {
  if (getButton(1) && !getButton(3)) { // Manipulator Down
    set_manipulator_dir(-1);
  }
  else if (!getButton(1) && getButton(3)) { // Manipulator Up
    set_manipulator_dir(1);
  }
  else if (((!getButton(1) && !getButton(3)) || (getButton(1) && getButton(3)))) { // Stop Manipulator
    set_manipulator_dir(0);
  }
}

void set_manipulator_dir(int direction) {
  if (direction < 0) {
    current_manipulator_direction = -1;
    digitalWrite(MANIPULATOR_FORWARD_PIN, LOW);
    digitalWrite(MANIPULATOR_REVERSE_PIN, HIGH);
  }
  else if (direction > 0) {
    current_manipulator_direction = 1;
    digitalWrite(MANIPULATOR_FORWARD_PIN, HIGH);
    digitalWrite(MANIPULATOR_REVERSE_PIN, LOW);
  }
  else {
    current_manipulator_direction = 0;
    digitalWrite(MANIPULATOR_FORWARD_PIN, LOW);
    digitalWrite(MANIPULATOR_REVERSE_PIN, LOW);
  }
}

void drive_left(double speed){
  double promoted = speed;
  Wire.beginTransmission(LEFT_SPEEDSERVO_ADDR);
  Wire.write(0);
  Wire.write((byte *) &promoted, 4);
  Wire.endTransmission();
}
void drive_right(double speed){
  double promoted = speed;
  Wire.beginTransmission(RIGHT_SPEEDSERVO_ADDR);
  Wire.write(0);
  Wire.write((byte *) &promoted, 4);
  Wire.endTransmission();
}

void tank_drive() {
  if(getButton(7)){
    estop_state = false;
  }
  if(getButton(5)){
    estop_state = true;
  }
  if(estop_state){
    estop();
    return;
  }

  double left_power = ((double)(astate->stickLY) - 128);
  double right_power = ((double)(astate->stickRY) - 128);
  if (left_power > 0) {
    left_power *= (128.0 / 127.0);
  }
  if (right_power > 0) {
    right_power *= (128.0 / 127.0);
  }

  double multiplier;
  if(getButton(6)){ //turbo
    multiplier = 4;
  }
  else if(getButton(4)){ //precise
    multiplier = 0.5;
  } else {
    multiplier = 1;
  }

  // Square Inputs
  left_power *= -1 * multiplier;
  right_power *= multiplier;

  last_left_speed = (int)left_power;
  last_right_speed = (int)right_power;

  drive_left(left_power);
  drive_right(right_power);
}

// Not currently used
void arcade_drive(){
  if(getButton(7)){
    estop_state = false;
  }
  if(getButton(5)){
    estop_state = true;
  }
  if(estop_state){
    estop();
    return;
  }
  int power_out = 0;
  int turn_out  = 0;
  int zeroed_power =    ((int)(astate->stickLY) - 128);
  int zeroed_turn =     -1*((int)(astate->stickRX) - 128);
  
  if(abs(zeroed_power) > DEADBAND_HALF_WIDTH){
    if(zeroed_power>0){
      power_out = zeroed_power - DEADBAND_HALF_WIDTH;
    } else {
      power_out = zeroed_power + DEADBAND_HALF_WIDTH;
    }
  }
  if(abs(zeroed_turn) > DEADBAND_HALF_WIDTH){
    if(zeroed_turn>0){
      turn_out = zeroed_turn - DEADBAND_HALF_WIDTH;
    } else {
      turn_out = zeroed_turn + DEADBAND_HALF_WIDTH;
    }
  }

  int multiplier;
  if(getButton(6)){ //turbo
    multiplier = 4;
  }
  else if(getButton(4)){ //precise
    multiplier = 1;
  } else {
    multiplier = 2;
  }

  int left_out =        multiplier*(power_out - (turn_out/4));
  int right_out =    -1*multiplier*(power_out + (turn_out/4));

  last_left_speed = left_out;
  last_right_speed = right_out;

  drive_left(left_out);
  drive_right(right_out);
}
