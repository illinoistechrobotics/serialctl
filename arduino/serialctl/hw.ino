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
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT); 

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
  char speedline[100];
  if (DISABLE_I2C) {
    snprintf(speedline, 100, "% 4d, % 4d, %lu", last_left_speed, last_right_speed, millis());
  }
  else {
    int left_speed = read_speed(LEFT_SPEEDSERVO_ADDR);
    int right_speed = read_speed(RIGHT_SPEEDSERVO_ADDR);
    
    snprintf(speedline, 100, "% 4d, % 4d, % 4d, % 4d, %lu", 
        left_speed,last_left_speed,
        right_speed,last_right_speed,
        millis());
  }
  SerComm.print(speedline);
  
}
void print_data(){
  if(estop_state){
    SerComm.println("ESTOP");
  } else {
    if(cs != COMM_WAIT){
      //SerComm.println("Good to roll");
      print_speed();
    }
    SerComm.println("Fenrir");
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
double processStick(int stickInput) {
  stickInput -= 128;
  double topMultiplier = 128.0 / (127 - DEADBAND_HALF_WIDTH);
  double bottomMultiplier = 128.0 / (128 - DEADBAND_HALF_WIDTH);
  if (abs(stickInput) <= DEADBAND_HALF_WIDTH) {
    return 0.0;
  }
  if (stickInput > 0) {
    return (stickInput - DEADBAND_HALF_WIDTH) * topMultiplier;
  }
  else {
    return (stickInput + DEADBAND_HALF_WIDTH) * bottomMultiplier;
  }
}
double getMultiplier() {
  if(getButton(6)){ //turbo
    return 1;
  }
  else if(getButton(4)){ //precise
    return 0.125;
  } else {
    return 0.25;
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
 if (getButton(0) && !getButton(2))
 {
  digitalWrite(0, LOW);
  digitalWrite(1, HIGH);
 }
else if (!getButton(0) && getButton(2))
 {
  digitalWrite(0, HIGH);
  digitalWrite(1, LOW);
 }
else
 {
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
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

bool is_estop() {
  if(getButton(7)){
    estop_state = false;
  }
  if(getButton(5)){
    estop_state = true;
  }
  if(estop_state){
    estop();
    return true;
  }
  return false;
}

void tank_drive() {
  if (is_estop()) {
    return;
  }

  double left_power = processStick(astate->stickLY);
  double right_power = processStick(astate->stickRY);

  double multiplier = getMultiplier();

  // Square Inputs
  left_power *= -1 * multiplier;
  right_power *= multiplier;

  last_left_speed = (int)left_power;
  last_right_speed = (int)right_power;

  drive_left(left_power);
  drive_right(right_power);
}

void arcade_drive(){
  if (is_estop()) {
    return;
  }
  
  double power_out = processStick(astate->stickLY);
  double turn_out  = -1 * processStick(astate->stickRX);

  double multiplier = getMultiplier();

  double left_out =  -1*multiplier*(power_out - (turn_out/4));
  double right_out =    multiplier*(power_out + (turn_out/4));

  last_left_speed = (int)left_out;
  last_right_speed = (int)right_out;

  drive_left(left_out);
  drive_right(right_out);
}
