#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>
#include <Dynamixel_Serial.h>

Servo fl, fr, rl, rr, arm_key, arm_ball, servo_gripper;

#define SERVO_ID_BALL_L 0x04
#define SERVO_ID_BALL_R 0x05
#define SERVO_ID_KEY 0x03
#define SERVO_ControlPin_Ball_L 2
#define SERVO_ControlPin_Ball_R 24
#define SERVO_ControlPin_Key 26
#define SERVO_SET_Baudrate 19200
#define SERVO_DEF_Baudrate 1000000
#define SERVO_PWM_PIN 13

void init_pins(){
        arm_key.attach(ARM_KEY_PIN);
	arm_ball.attach(ARM_BALL_PIN);
        arm_key.writeMicroseconds(1500);
	arm_ball.writeMicroseconds(1500);
        talon_init();
        pinMode(MANI_INTAKE, OUTPUT);
        pinMode(MANI_OUTPUT, OUTPUT);
        digitalWrite(MANI_INTAKE, LOW);
        digitalWrite(MANI_OUTPUT, LOW);
}
void init_servos(HardwareSerial &serial_port, int control_pin, int servo_id){
  servo_gripper.attach(SERVO_PWM_PIN);
  Dynamixel.begin(serial_port,SERVO_SET_Baudrate,control_pin);
  Dynamixel.ledState(servo_id, ON);
  delay(5);
  Dynamixel.setMode(servo_id, SERVO, 0x0000, 0x3FF);
  delay(5);
  Dynamixel.setMaxTorque(servo_id, 0x2FF);
  delay(5);
  Dynamixel.setHoldingTorque(servo_id, 0x1);
}
void print_data(){
   if(comm_ok==0){
    //Print failsafe notice
    SerComm.print("-FS- ");
  }
        SerComm.println("Roslund");
}
uint8_t getButton(int num){
        if(num<=7){
                return (astate->btnlo >> num) & 0x01;
        } else if(num>7 && num <= 15){
                return (astate->btnhi >> (num - 8)) & 0x01;
        } else {
                return 0;
        }
}
void talon_init(){
        fl.attach(FRONT_LEFT);
        fr.attach(FRONT_RIGHT);
        rl.attach(REAR_LEFT);
        rr.attach(REAR_RIGHT);
        fl.writeMicroseconds(1500);
        fr.writeMicroseconds(1500);
        rr.writeMicroseconds(1500);
        rl.writeMicroseconds(1500);
}
void move_arm_key(int8_t cmd){
   switch(cmd){
      case 1:
        SerComm.print("Arm Up ");
         arm_key.writeMicroseconds(1000);
         break;
      case -1:
          SerComm.print("Arm Down ");
         arm_key.writeMicroseconds(2000);
         break;
      default:
        SerComm.print("Arm Center ");
         arm_key.writeMicroseconds(1500);
   }
}
void move_arm_ball(int8_t cmd){
   switch(cmd){
      case 1:
          SerComm.print("Ball Up ");
         arm_ball.writeMicroseconds(1000);
         break;
      case -1:
          SerComm.print("Ball Down ");
         arm_ball.writeMicroseconds(2000);
         break;
      default:
          SerComm.print("Ball Center ");
         arm_ball.writeMicroseconds(1500);
   }
}
void manipulator()
{
   if (getButton(0) && !getButton(2))
   {
      digitalWrite(MANI_INTAKE, HIGH);
      digitalWrite(MANI_OUTPUT, LOW);
   }
   else if (!getButton(0) && getButton(2))
   {
      digitalWrite(MANI_INTAKE, LOW);
      digitalWrite(MANI_OUTPUT, HIGH);
   }
   else
   {
      digitalWrite(MANI_INTAKE, LOW);
      digitalWrite(MANI_OUTPUT, LOW);
   }
}
void manipulator_ball() {
  if (getButton(5) ^ getButton(7)) 
     if (getButton(5)) {
      Dynamixel.servo(SERVO_ID_BALL_L,0x0001,0x0100);
      Dynamixel.servo(SERVO_ID_BALL_R,0x0001,0x0100);
      delay(4000);
     } else if(getButton(7)) {
      Dynamixel.servo(SERVO_ID_BALL_L,0x0001,0x0100);
      Dynamixel.servo(SERVO_ID_BALL_R,0x0001,0x0100);
      delay(4000);
     }
  }  
}
void manipulator_key() {
  if (getButton(JOYSTICK_PAD_UP) ^ getButton(JOYSTICK_PAD_DOWN)) {
    if (getButton(JOYSTICK_PAD_UP)) {
      Dynamixel.servo(SERVO_ID_KEY,0x0001,0x0100);
      delay(4000);
    } else if(getButton(JOYSTICK_PAD_DOWN)){
      Dynamixel.servo(SERVO_ID_KEY,0x0001,0x0100);
      delay(4000);
    } 
  }
  if (getButton(JOYSTICK_PAD_LEFT) ^ getButton(JOYSTICK_PAD_RIGHT)) {
    if (getButton(JOYSTICK_PAD_LEFT)) {
      servo_gripper.write();
    } else if(JOYSTICK_PAD_RIGHT) {
      servo_gripper.write();
    }
  }
}
void drive_left(int power){
        power = map(constrain(power,-127,127),-127,127,1000,2000);
        fl.writeMicroseconds(power);
        rl.writeMicroseconds(power);
}

void drive_right(int power){
        power = map(constrain(power,-127,127),-127,127,1000,2000);
        fr.writeMicroseconds(power);
        rr.writeMicroseconds(power);
}
