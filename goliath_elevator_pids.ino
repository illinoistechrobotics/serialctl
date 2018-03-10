#include <Servo.h>
#include <PID_v2.h>
#include <EEPROM.h>
#include <util/atomic.h>

#define PID_SERIAL_BUFFER_SIZE 16
#define EEPROM_BASE 0
#define EEPROM_OFFSET_D(x) (EEPROM_BASE + (x)*sizeof(double))
#define FS_P_4 EEPROM_OFFSET_D(0)
#define FS_I_4 EEPROM_OFFSET_D(1)
#define FS_D_4 EEPROM_OFFSET_D(2)

typedef struct pid_vals {
  double kp, ki, kd;
} tunings_t;

Servo motor[2];

#define TICKSPERROTATION 12L // ?

int numMotors = 2;
int ChA[] = {2, 3};
int ChB[] = {10, 5};
#define FASTREADB0 (PINB & (1<<2)) // digital pin 10, port B pin 2
#define FASTREADB1 (PINB & (1<<4)) // digital pin 12, port B pin 4

int enablePin = 9;
int upPin = 13;

int motorPin[] = {6, 11};
#define MAXMOTORSPEED 250 //375
int motorSpeed[] = {20, 20}; // ?
int motorCmd[] = {1500, 1500}; // use writeMicroseconds
int motorOutput[2];
double pid_out;
tunings_t tunings;

/*
long int dist[] = {0, 0};
int lastPos[] = {0, 0};
float targetPos[] = {0, 0};
int lastDist[] = {0, 0};
int overallTarget;
*/

volatile long ticks[] = {0, 0};
volatile double vDifference = 0;
double nvDifference = 0;
double target = 0; /* syncs motor positions, do not change */
long lastTicks[] = {0, 0};
long lastMillis = millis();
double rpm[] = {0, 0};

PID* diff_pid;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(57600);
  pinMode(ChA, INPUT);
  pinMode(ChB, INPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(enablePin, INPUT);
  pinMode(upPin, INPUT);
  
  PIDLoadTunings();

  attachInterrupt(digitalPinToInterrupt(ChA[0]), encoder0Interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ChA[1]), encoder1Interrupt, RISING);

  diff_pid = new PID(&nvDifference, &pid_out, &target, tunings.kp, tunings.ki, tunings.kd, DIRECT);
  diff_pid->SetOutputLimits(-125, 125);
  diff_pid->SetTunings(10, 0, 0);
  diff_pid->SetSampleTime(25);
  diff_pid->SetMode(AUTOMATIC);
  /* probably need to do stuff here to sync motors initially */

  for (int i = 0; i < numMotors; i++) {
    motor[i].attach(motorPin[i]);
  }
}

void loop() {
  if (millis() - lastMillis > 250) {
    for (int m = 0; m < numMotors; m++) {
      long myTicks;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        myTicks = ticks[m];
      }
      rpm[m] = (myTicks - lastTicks[m]) / TICKSPERROTATION;// / (millis() - lastMillis) * 60000L;
      lastTicks[m] = myTicks;

      PIDTuner();

      
      Serial.print("Motor ");
      Serial.print(m);
      Serial.print(": RPM ");
      Serial.print(rpm[m]);
      Serial.print("; ticks ");
      Serial.print(ticks[m]);
      Serial.print("; cmd ");
      Serial.print(motorCmd[m]);
      if (digitalRead(upPin) && digitalRead(enablePin)) Serial.print("; Going up");
      else if (digitalRead(enablePin)) Serial.print("; Going down");
      Serial.print("; PID term ");
      Serial.print(pid_out);
      Serial.println();
      
    }
    lastMillis = millis();
  }

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    nvDifference = vDifference;
  }
  diff_pid->Compute();
  
  if (digitalRead(enablePin)) {
    if (digitalRead(upPin)) goUp();
    else goDown();
  }
  else {
    motorCmd[0] = 1500 + (int)pid_out;
    motorCmd[1] = 1500 - (int)pid_out;
  }
  motor[0].writeMicroseconds(motorCmd[0]);
  motor[1].writeMicroseconds(motorCmd[1]);
}

void goUp() {
  motorCmd[0] = 1500 + MAXMOTORSPEED + (int)pid_out;
  motorCmd[1] = 1500 - MAXMOTORSPEED - (int)pid_out;
}

void goDown() {
  motorCmd[0] = 1500 + MAXMOTORSPEED + (int)pid_out;
  motorCmd[1] = 1500 - MAXMOTORSPEED - (int)pid_out;
}

void goToTarget(int targetTicks) {
  //(20*((float)(abs(targetPos[m] -dist[m])/(float)maxPos))+10)*(pow(-1,m)));
}

double load_controller_tuning(int tuning) {
  double out;
  byte * reassembled = (byte *)&out;
  for(int i = 0; i < sizeof(double); ++i){
    *reassembled = EEPROM.read(sizeof(double)*tuning + i);
    reassembled++;
  }
  return out;
}

void encoder0Interrupt() {
  if (FASTREADB0) ticks[0]--;
  else ticks[0]++;
  vDifference = ticks[1] - ticks[0];
}

void encoder1Interrupt() {
  if (FASTREADB1) ticks[1]++;
  else ticks[1]--;
  vDifference = ticks[1] - ticks[0];
}

void PIDTuner(){
  //Parse the data relating to the local PID tuner
  static char serialInputBuffer[PID_SERIAL_BUFFER_SIZE];
  static char serialInputBufferIndex = 0;
  static char currentPIDValueIsLeft = 1;
  static double *currentPIDValueToUpdate = &tunings.kp;
  //Serial Input for PID configuration
  if (Serial.available()) {
    int incomingByte = Serial.read();
    switch(incomingByte) {
      case 'P':
      case 'p':
        currentPIDValueToUpdate = &tunings.kp;
        break;
      case 'I':
      case 'i':
        currentPIDValueToUpdate = &tunings.ki;
        break;
      case 'D':
      case 'd':
        currentPIDValueToUpdate = &tunings.kd;
        break;
      case 'W':
      case 'w':
        Serial.println("Writing PID tunings to EEPROM");
        PIDWriteTunings();
        serialInputBufferIndex = 0;
        break;
      case 'G':
      case 'g':
        Serial.println("--------------------------------");
        Serial.println("Printing PID tunings:");
        serialInputBufferIndex = 0;
        Serial.print("P ");
        Serial.println(tunings.kp,8);
        Serial.print("I ");
        Serial.println(tunings.ki,8);
        Serial.print("D ");
        Serial.println(tunings.kd,8);
        Serial.println("--------------------------------");
        break;
      case 'M':
      case 'm':
        Serial.println("Reloading PID tunings from EEPROM");
        PIDLoadTunings();
        serialInputBufferIndex = 0;
        break;
      case '\n':
        if(serialInputBufferIndex > 0){
          serialInputBuffer[serialInputBufferIndex] = '\0';
          *currentPIDValueToUpdate = strtod(serialInputBuffer, NULL);
          Serial.print("Setting ");
          if (currentPIDValueToUpdate == &tunings.kp) {
            Serial.print("P ");
          }
          if (currentPIDValueToUpdate == &tunings.ki) {
            Serial.print("I ");
          }
          if (currentPIDValueToUpdate == &tunings.kd) {
            Serial.print("D ");
          }
          Serial.print("to ");
          Serial.println(*currentPIDValueToUpdate);
          PIDRefreshTunings();
        }
        serialInputBufferIndex = 0;
        break;
      case ' ':
        break;
      default:
        if (serialInputBufferIndex < PID_SERIAL_BUFFER_SIZE - 1) {
          serialInputBuffer[serialInputBufferIndex] = incomingByte;
          serialInputBufferIndex++;
        }
        break;
    }
  }
}

void PIDWriteTunings() {
  EEPROM.put(0, tunings);
}

void PIDLoadTunings() {
  EEPROM.get(0, tunings);
}

void PIDRefreshTunings() {
  diff_pid->SetTunings(tunings.kp, tunings.ki, tunings.kd);
}

