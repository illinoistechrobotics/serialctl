#include "hw.h"
#include "packet.h"
#include "globals.h"

void init_pins() {
  //Main motors
  osmc_init();
  //Linear Actuators
  SABERTOOTH12.begin(9600);
  //SABERTOOTH34.begin(9600);
  delay(10);
  //failsafes
  for (int i = 0; i < 3; i++) {
    ST12.setTimeout(1000);
    ST34.setTimeout(1000);
    ST12.motor(1, 0);
    ST34.motor(1, 0);
    ST12.motor(2, 0);
    ST34.motor(2, 0);
  }

  //Gen purpose I/O
  pinMode(13, OUTPUT);
  pinMode(GRIP_VALVE,OUTPUT);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);
  digitalWrite(GRIP_VALVE,LOW);
}

void measure_offset() {
  //can only be called once--
  //should be during setup() after drive is stopped
  if (offset_measured) return;
  int n = 10;
  current_offset_0 = current_offset_120, current_offset_240 = 0;
  for (int i = 0; i < n; i++) {
    current_offset_0 += analogRead(CURRENT_0);
    current_offset_120 += analogRead(CURRENT_120);
    current_offset_240 += analogRead(CURRENT_240);
    delay(10);
  }
  current_offset_0 /= n;
  current_offset_0 -= 512;
  current_offset_120 /= n;
  current_offset_120 -= 512;
  current_offset_240 /= n;
  current_offset_240 -= 512;
  offset_measured = 1;
}

void print_data() {
  float i1, i2, i3;
  i1 = (analogRead(CURRENT_0) - 512.0 - current_offset_0) / 1.28;
  i2 = (analogRead(CURRENT_120) - 512.0 - current_offset_120) / 1.28;
  i3 = (analogRead(CURRENT_240) - 512.0 - current_offset_240) / 1.28;
  
  if(comm_ok==0){
    //Print failsafe notice
    SerComm.print("-FS- ");
  }
  SerComm.print("Zero: V=");
  if(enabled_0)
    SerComm.print("+");
   else
    SerComm.print("0");
  SerComm.print(", I=");
  SerComm.print(i1);
  SerComm.print("A | 120: V=");
  if(enabled_120)
    SerComm.print("+");
   else
    SerComm.print("0");
  SerComm.print(", I=");
  SerComm.print(i2);
  SerComm.print("A | 240: V=");
  if(enabled_240)
    SerComm.print("+");
   else
    SerComm.print("0");
  SerComm.print(", I=");
  SerComm.print(i3);  
  SerComm.print("A | AIR:");
  SerComm.print(psi);
  SerComm.print("PSI | X(arm): ");
  SerComm.print(armcount);
  SerComm.println();
}

void osmc_init() {
  digitalWrite(13, LOW);
  digitalWrite(ALI1, LOW);
  digitalWrite(BLI1, LOW);
  digitalWrite(AHI1, LOW);
  digitalWrite(BHI1, LOW);
  digitalWrite(ALI2, LOW);
  digitalWrite(BLI2, LOW);
  digitalWrite(AHI2, LOW);
  digitalWrite(BHI2, LOW);
  digitalWrite(ALI3, LOW);
  digitalWrite(BLI3, LOW);
  digitalWrite(AHI3, LOW);
  digitalWrite(BHI3, LOW);
  digitalWrite(DENABLE1, LOW);
  digitalWrite(DENABLE2, LOW);
  digitalWrite(DENABLE3, LOW);
  
  pinMode(ALI1, OUTPUT);
  pinMode(AHI1, OUTPUT);
  pinMode(BLI1, OUTPUT);
  pinMode(BHI1, OUTPUT);
  pinMode(ALI2, OUTPUT);
  pinMode(AHI2, OUTPUT);
  pinMode(BLI2, OUTPUT);
  pinMode(BHI2, OUTPUT);
  pinMode(ALI3, OUTPUT);
  pinMode(AHI3, OUTPUT);
  pinMode(BLI3, OUTPUT);
  pinMode(BHI3, OUTPUT);
  pinMode(DENABLE1, OUTPUT);
  pinMode(DENABLE2, OUTPUT);
  pinMode(DENABLE3, OUTPUT);
  pinMode(DREADY1, INPUT);
  pinMode(DREADY2, INPUT);
  pinMode(DREADY3, INPUT);

  pinMode(13, OUTPUT);
  fast_pwm();
}
void fast_pwm() {
  TCCR1B = (TCCR1B & 0b11111000) | 0x03;
  TCCR3B = (TCCR3B & 0b11111000) | 0x03;
  TCCR4B = (TCCR4B & 0b11111000) | 0x03;
}

// returns new enable state
char try_enable_osmc(char enabled, char enablepin, char readypin,
                     char ali, char bli, char ahi, char bhi) {
  // ready signal from osmc controller indicates that we are OK to provide input
  if (digitalRead(readypin)) {
    if (!enabled){
      delay(10); //"Short" delay required in order to prevent blowout! 10ms is conservative.
      digitalWrite(enablepin, HIGH);
    }
    return 1;
  }
  else { // controller has no power; zero inputs in case we power it again
    digitalWrite(enablepin, LOW);
    digitalWrite(ali, LOW);
    digitalWrite(bli, LOW);
    digitalWrite(ahi, LOW);
    digitalWrite(bhi, LOW);
    return 0;
  }
}

// OSMC motor controller stuff
// Low side outputs must be PWM capable and NOT 5 or 6 (on Uno)
// Do not change timer0,
// Pins 2 and 3 use timer3 in phase correct mode
// Pins 6 and 7 use timer4 in phase correct mode
// Pins 11 and 12 use timer1 in phase correct mode
// OSMC ALI and BLI are the low side driver inputs and must ALWAYS be low/zero when the ready signal is not provided
// OSMC AHI and BHI are the high side driver inputs.
/*
 * ----------- Vdd
 *   |     |
 *   AHI   BHI
 *   |     |
 *   ---M---
 *   |     |
 *   ALI   BLI
 *   |     |
 *   --------- GND
 */
void drive_osmc(char enabled, char enablepin, int rawpower, char brake,
                char ali, char bli, char ahi, char bhi) {
  int power = constrain(rawpower, -255, 255);
  if (!enabled) {
    digitalWrite(ali, LOW);
    digitalWrite(bli, LOW);
    digitalWrite(ahi, LOW);
    digitalWrite(bhi, LOW);
    digitalWrite(enablepin, LOW);
    return;
  }
  //Stop!
  if (abs(power) < 2) {
    digitalWrite(ali, LOW);
    digitalWrite(bli, LOW);
    delayMicroseconds(63);
    if (brake != 0) {
      digitalWrite(ahi, HIGH);
      digitalWrite(bhi, HIGH);
    } else {
      digitalWrite(ahi, LOW);
      digitalWrite(bhi, LOW);
    }
    return;
  }
  //Forward!
  if (power > 0) {
    digitalWrite(bhi, LOW);
    digitalWrite(ali, LOW);
    delayMicroseconds(63);
    digitalWrite(ahi, HIGH);
    analogWrite(bli, power);
  }
  //Reverse!
  if (power < 0) {
    digitalWrite(ahi, LOW);
    digitalWrite(bli, LOW);
    delayMicroseconds(63);
    digitalWrite(bhi, HIGH);
    analogWrite(ali, abs(power));
  }
}

void compressor_ctl(){
  static char pumping=0;
  int i;
  psi=0;
  for(i=0;i<5;i++){
    //5x multisampling
    psi += ADC2PSI(P_SENSOR);
  }
  psi = psi/5;
  DEBUGPRINT(psi);
  if(comm_ok == 1){
    //Read pressures and pump if needed
    if(psi < MIN_PRESS){
      pumping = 1;
    } else if(psi >= MAX_PRESS){
      pumping = 0;
    }
  } else{
    //No control link, stop compressor
    pumping=0;
  }
  
  if(pumping == 1){
    //Go!
    setMotor(COMPRESSOR_MOTOR,127);
  } else{
    //Stop!
    setMotor(COMPRESSOR_MOTOR,0);
  }
}

