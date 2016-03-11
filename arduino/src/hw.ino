#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>
Servo fl, fr, rl, rr, arm;
void init_pins(){
        arm.attach(ARM_PIN);
        arm.writeMicroseconds(1500);
        talon_init();
}
void print_data(){
   if(comm_ok==0){
    //Print failsafe notice
    SerComm.print("-FS- ");
  }
        SerComm.println("Roslund");
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
void move_arm(int8_t cmd){
   switch(cmd){
      case 1:
         arm.writeMicroseconds(1000);
         break;
      case -1:
         arm.writeMicroseconds(2000);
         break;
      default:
         arm.writeMicroseconds(1500);
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
