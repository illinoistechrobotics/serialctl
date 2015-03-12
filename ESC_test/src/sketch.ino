#include <Servo.h>
#define SPINNER_PIN 2

Servo spinner;

void setup()
{
  Serial.begin(9600);
  spinner.attach(SPINNER_PIN);
  pinMode(13,OUTPUT);
  pinMode(41,OUTPUT);
}
int dir = 0;
void loop()
{
  if(Serial.available()){
    char command = Serial.read();
    int usec = 0;
    switch(command){
    case '1':
      usec = 1200;
      break;
    case '2':
      usec = 1700;
      break;
    case '3':
      usec = 2200;
      break;
    case 'r':
      dir = !dir;
      digitalWrite(41, dir?HIGH:LOW);
    }
    if(usec){
      digitalWrite(13,HIGH);
    } else {
      digitalWrite(13,LOW);
    }
      
    spinner.writeMicroseconds(usec);
  }
  delay(100);
}
