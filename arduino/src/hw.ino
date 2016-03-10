#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>
Servo fl, fr, rl, rr;
void init_pins(){
        pinMode(MANIP_GRIP, OUTPUT);
        pinMode(MANIP_KICK, OUTPUT);
        pinMode(ARM_UP,OUTPUT);
        pinMode(ARM_DOWN,OUTPUT);
        digitalWrite(MANIP_GRIP,LOW);
        digitalWrite(MANIP_KICK,LOW);
        digitalWrite(ARM_UP,LOW);
        digitalWrite(ARM_DOWN,LOW);
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
