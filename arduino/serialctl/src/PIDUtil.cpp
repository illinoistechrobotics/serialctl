#include <Arduino.h>
#include "globals.h"
#include "hw.h"
#include "iic_encoder_io.h"
#include "PIDUtil.h"
double pidLeftP=0, pidLeftI=0, pidLeftD=0, pidRightP=0, pidRightI=0, pidRightD=0;
double leftIn=0, leftOut=0, leftSet=0, rightIn=0, rightOut=0, rightSet=0;
extern char comm_ok, left_enabled, right_enabled;
extern uint32_t left_tick_count, right_tick_count;

PID leftPID(&leftIn, &leftOut, &leftSet, pidLeftP, pidLeftI, pidLeftD, REVERSE);
PID rightPID(&rightIn, &rightOut, &rightSet, pidRightP, pidRightI, pidRightD, DIRECT);

int PIDEncoderCheck(){
  int rv=1;
  #ifdef DEBUGPRINT
  double dummy_dbl;
  uint32_t dummy_int;
  if(iic_encoder_read(ENCODER_LEFT_ADDR,&dummy_dbl,&dummy_int)){
    DEBUGPRINT("Left Encoder Data OK!");
  } else {
    DEBUGPRINT("Left Encoder Data Bus Fault!");
    rv = -1;
  }
  if(iic_encoder_read(ENCODER_RIGHT_ADDR,&dummy_dbl,&dummy_int)){
    DEBUGPRINT("Right Encoder Data OK!");
  } else {
    DEBUGPRINT("Right Encoder Data Bus Fault!");
    rv = -1;
  }
  #endif
  return rv;
}

void PIDDrive(){
  if(leftPID.NeedsCompute()){
    uint32_t left_read_ticks;
    if(iic_encoder_read(ENCODER_LEFT_ADDR,&leftIn,&left_read_ticks)){
    //Successfully read the left encoder
      leftPID.Compute();
      left_tick_count += left_read_ticks;
    } else {
      //Bus fault!
      leftOut=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(L): ");
    SerCommDbg.print(leftOut);
    SerCommDbg.print(", ");
    SerCommDbg.print(leftIn);
    SerCommDbg.print(", Total ticks: ");
    SerCommDbg.print(left_tick_count);
    SerCommDbg.println();
    #endif
    drive_left(left_enabled,leftOut);
  }
  if(rightPID.NeedsCompute()){
    uint32_t right_read_ticks;
    if(iic_encoder_read(ENCODER_RIGHT_ADDR,&rightIn,&right_read_ticks)){
    //Successfully read the left encoder
      rightPID.Compute();
      right_tick_count += right_read_ticks;
    } else {
      //Bus fault!
      rightOut=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(R): ");
    SerCommDbg.print(rightOut);
    SerCommDbg.print(", ");
    SerCommDbg.print(rightIn);
    SerCommDbg.print(", Total ticks: ");
    SerCommDbg.print(right_tick_count);
    SerCommDbg.println();
    #endif
    drive_right(right_enabled,rightOut);
 }
}

void PIDInit(){
    PIDLoadTunings(); //Load the PID tunings from the EEPROM
    PIDRefreshTunings();
    leftPID.SetMode(MANUAL);
    rightPID.SetMode(MANUAL);
    leftPID.SetSampleTime(PID_SAMPLE_TIME);
    rightPID.SetSampleTime(PID_SAMPLE_TIME);
    leftPID.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
    rightPID.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
}

void PIDRefreshTunings(){
  leftPID.SetTunings(pidLeftP, pidLeftI, pidLeftD);
  rightPID.SetTunings(pidRightP, pidRightI, pidRightD);  
}

void PIDTuner(){
  //Parse the data relating to the local PID tuner
  static char serialInputBuffer[PID_SERIAL_BUFFER_SIZE];
  static uint8_t serialInputBufferIndex = 0;
  static char currentPIDValueIsLeft = 1;
  static double *currentPIDValueToUpdate = &pidLeftP;
  //Serial Input for PID configuration
  if (SerCommDbg.available()) {
    int incomingByte = SerCommDbg.read();
    switch(incomingByte) {
      case 'L':
      case 'l':
        currentPIDValueIsLeft = 1;
        break;
      case 'R':
      case 'r':
        currentPIDValueIsLeft = 0;
        break;
      case 'P':
      case 'p':
        if (currentPIDValueIsLeft) {
          currentPIDValueToUpdate = &pidLeftP;
        }
        else {
          currentPIDValueToUpdate = &pidRightP;
        }
        break;
      case 'I':
      case 'i':
        if (currentPIDValueIsLeft) {
          currentPIDValueToUpdate = &pidLeftI;
        }
        else {
          currentPIDValueToUpdate = &pidRightI;
        }
        break;
      case 'D':
      case 'd':
        if (currentPIDValueIsLeft) {
          currentPIDValueToUpdate = &pidLeftD;
        }
        else {
          currentPIDValueToUpdate = &pidRightD;
        }
        break;
      case 'W':
      case 'w':
        SerCommDbg.println("Writing PID tunings to EEPROM");
        PIDWriteTunings();
        serialInputBufferIndex = 0;
        break;
      case 'G':
      case 'g':
        SerCommDbg.println("Printing PID tunings:");
        serialInputBufferIndex = 0;
        SerCommDbg.print("Left P ");
        SerCommDbg.println(pidLeftP,8);
        SerCommDbg.print("Left I ");
        SerCommDbg.println(pidLeftI,8);
        SerCommDbg.print("Left D ");
        SerCommDbg.println(pidLeftD,8);
        SerCommDbg.print("Right P ");
        SerCommDbg.println(pidRightP,8);
        SerCommDbg.print("Right I ");
        SerCommDbg.println(pidRightI,8);
        SerCommDbg.print("Right D ");
        SerCommDbg.println(pidRightD,8);
        SerCommDbg.println("--------------------------------");
        break;
      case 'M':
      case 'm':
        SerCommDbg.println("Reloading PID tunings from EEPROM");
        PIDLoadTunings();
        serialInputBufferIndex = 0;
        break;
      case '\n':
        if(serialInputBufferIndex > 0){
          serialInputBuffer[serialInputBufferIndex] = '\0';
          *currentPIDValueToUpdate = strtod(serialInputBuffer, NULL);
          SerCommDbg.print("Setting ");
          if (currentPIDValueToUpdate == &pidLeftP) {
            
          }
          if (currentPIDValueToUpdate == &pidLeftI) {
            SerCommDbg.print("Left I ");
          }
          if (currentPIDValueToUpdate == &pidLeftD) {
            SerCommDbg.print("Left D ");
          }
          if (currentPIDValueToUpdate == &pidRightP) {
            SerCommDbg.print("Right P ");
          }
          if (currentPIDValueToUpdate == &pidRightI) {
            SerCommDbg.print("Right I ");
          }
          if (currentPIDValueToUpdate == &pidRightD) {
          SerCommDbg.print("Right D ");
          }
          SerCommDbg.print("to ");
          SerCommDbg.println(*currentPIDValueToUpdate);
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

void PIDWriteTunings(){
  //Write the current value of the PID tunings to the EEPROM, which is nonvolatile
  EEPROM.put(FS_LEFT_P_4,pidLeftP);
  EEPROM.put(FS_LEFT_I_4,pidLeftI);
  EEPROM.put(FS_LEFT_D_4,pidLeftD);
  EEPROM.put(FS_RIGHT_P_4,pidRightP);
  EEPROM.put(FS_RIGHT_I_4,pidRightI);
  EEPROM.put(FS_RIGHT_D_4,pidRightD);
}

void PIDLoadTunings(){
  //Load the PID tunings from the EEPROM
  //PID library cannot recover from NaN internal state
  EEPROM.get(FS_LEFT_P_4,pidLeftP);
  if(pidLeftP == NAN)
    pidLeftP = 0;
  EEPROM.get(FS_LEFT_I_4,pidLeftI);
  if(pidLeftI == NAN)
    pidLeftI = 0;
  EEPROM.get(FS_LEFT_D_4,pidLeftD);
  if(pidLeftD == NAN)
    pidLeftD = 0;
  EEPROM.get(FS_RIGHT_P_4,pidRightP);
  if(pidRightP == NAN)
    pidRightP = 0;
  EEPROM.get(FS_RIGHT_I_4,pidRightI);
  if(pidRightI == NAN)
    pidRightI = 0;
  EEPROM.get(FS_RIGHT_D_4,pidRightD);
  if(pidRightD == NAN)
    pidRightD = 0;
    
  PIDRefreshTunings();
}
