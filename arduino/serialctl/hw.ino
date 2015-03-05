#define COMPRESS 43
#define P_SENSOR A2
#define CURRENT_OFFSET -2.5
#define PRESSURE_OFFSET 161
#define MIN_PRESS 80
#define MAX_PRESS 100
#define CURRENT_LEFT A0
#define CURRENT_RIGHT A1
#define ALI1 7
#define BLI1 8
#define AHI1 22
#define BHI1 23
#define ALI2 11
#define BLI2 12
#define AHI2 24
#define BHI2 25
#define drive_left(x) drive_osmc(x,0,ALI1,BLI1,AHI1,BHI1)
#define drive_right(x) drive_osmc(x,0,ALI2,BLI2,AHI2,BHI2)
#define PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/2.048)

void init_pins(){
//Compressor
pinMode(COMPRESS,OUTPUT);
digitalWrite(COMPRESS,LOW);
//Main motors
osmc_init();
}

void pumpAir(){
  Serial.println(analogRead(P_SENSOR));
  if(PSI(P_SENSOR)>=MAX_PRESS){
    digitalWrite(COMPRESS,LOW);
  } else if((PSI(P_SENSOR)<MIN_PRESS)&&(cs!=COMM_WAIT)){
    digitalWrite(COMPRESS,HIGH);
  }
}

void pumpNow(){
  if((PSI(P_SENSOR)<MAX_PRESS)){
    digitalWrite(COMPRESS,HIGH);
  }
}

void print_data(){
   float i1,i2,psi;
   i1=(analogRead(CURRENT_LEFT)-512.0-CURRENT_OFFSET)/1.28;
   i2=(analogRead(CURRENT_RIGHT)-512.0-CURRENT_OFFSET)/1.28;
   psi=PSI(P_SENSOR);
     SerComm.print("Left Current:");
     SerComm.print(i1);
     SerComm.print("A   Right Current:");
     SerComm.print(i2);
     SerComm.print("A   Air:");
     SerComm.print(psi);
     SerComm.println("PSI");
}
void osmc_init(){
  digitalWrite(13,LOW);
  digitalWrite(ALI1,LOW);
  digitalWrite(BLI1,LOW);
  digitalWrite(AHI1,LOW);
  digitalWrite(BHI1,LOW);
  digitalWrite(ALI2,LOW);
  digitalWrite(BLI2,LOW);
  digitalWrite(AHI2,LOW);
  digitalWrite(BHI2,LOW);
  pinMode(ALI1, OUTPUT);
  pinMode(AHI1, OUTPUT);
  pinMode(BLI1, OUTPUT);
  pinMode(BHI1, OUTPUT);
  pinMode(ALI2, OUTPUT);
  pinMode(AHI2, OUTPUT);
  pinMode(BLI2, OUTPUT);
  pinMode(BHI2, OUTPUT);
  pinMode(13, OUTPUT);
  fast_pwm();
}
void fast_pwm(){
  TCCR1B = TCCR1B & 0b11111000 | 0x04;
  TCCR4B = TCCR4B & 0b11111000 | 0x04;
}
// OSMC motor controller stuff
// Low side outputs must be PWM capable and NOT 5 or 6 (on Uno)
// Do not change timer0,
// Pins 7 and 8 use timer4 in phase correct mode
// Pins 11 and 12 use timer1 in phase correct mode
void drive_osmc(int rawpower, unsigned short brake, unsigned short ali, unsigned short bli, unsigned short ahi, unsigned short bhi){
 int power = constrain(rawpower, -255,255);
 //Stop!
  if(abs(power)<2){
    digitalWrite(ali,LOW);
    digitalWrite(bli,LOW);
    if(brake!=0){
      digitalWrite(ahi,HIGH);
      digitalWrite(bhi,HIGH);
     }else{
       digitalWrite(ahi,LOW);
       digitalWrite(bhi,LOW); 
     }
  }
   //Forward!
   if(power>0){
     digitalWrite(ahi,HIGH);
     digitalWrite(bhi,LOW);
     digitalWrite(ali,LOW);
     analogWrite(bli, power);
   }
   //Reverse!
   if(power<0){
     digitalWrite(ahi,LOW);
     digitalWrite(bhi,HIGH);
     digitalWrite(bli,LOW);
     analogWrite(ali, abs(power));
   }
}
