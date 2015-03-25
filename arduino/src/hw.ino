#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>
#define LEFT_SPEEDSERVO_ADDR 1
#define RIGHT_SPEEDSERVO_ADDR 2
boolean estop_state = false;
int last_left_speed, last_right_speed;
void init_pins(){
  Wire.begin();
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
  snprintf(speedline, 100, "%d,%d,%d,%d,%d,%d", 
	   left_speed,last_left_speed,
	   right_speed,last_right_speed,
	   left_speed-last_left_speed,
	   right_speed-last_right_speed);
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
void drive_left(int speed){
  double promoted = speed;
  Wire.beginTransmission(LEFT_SPEEDSERVO_ADDR);
  Wire.write(0);
  Wire.write((byte *) &promoted, 4);
  Wire.endTransmission();
}
void drive_right(int speed){
  double promoted = speed;
  Wire.beginTransmission(RIGHT_SPEEDSERVO_ADDR);
  Wire.write(0);
  Wire.write((byte *) &promoted, 4);
  Wire.endTransmission();
}
void tank_drive(){
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
  int zeroed_power =    ((int)(astate->stickX) - 127);
  int zeroed_turn =     -1*((int)(astate->stickY) - 127);
  
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