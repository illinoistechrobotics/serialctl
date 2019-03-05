#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>
Servo lm,rm,bs1,bs2,as;
void print_data(){
	SerComm.print("ITR Icarus ");
	SerComm.print(leftMotorDebug);
	SerComm.print(", ");
	SerComm.println(rightMotorDebug);
}
void init_drive(){
	lm.attach(2);
	rm.attach(3);
	bs1.attach(4);
	bs2.attach(5);
	/*fl.writeMicroseconds(1500);
	fr.writeMicroseconds(1500);
	rr.writeMicroseconds(1500);
	rl.writeMicroseconds(1500);*/
}

void drive_left(int power){
	power = map(constrain(power,-127,127),-127,127,2000,1000);
	lm.write(power);
}

void drive_right(int power){
	power = map(constrain(power,-127,127),127,-127,1000,2000);
	rm.write(power);
}
void door_down(){
  bs1.write(100);
}
void door_up(){
  bs1.write(0);
}
void up_down(int power){
  power = map(constrain(power,-127,127),127,-127,1000,2000);
  bs2.write(power);
}
