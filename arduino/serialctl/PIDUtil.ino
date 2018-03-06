#include "PIDUtil.h"
double pidZeroP=0, pidZeroI=0, pidZeroD=0, pid120P=0, pid120I=0, pid120D=0, pid240P, pid240I, pid240D;
double zeroIn=0, zeroOut=0, zeroSet=0, 120In=0, 120Out=0, 120Set=0, 240In=0, 240Out=0, 240Set=0;
extern char comm_ok, zero_enabled, 120_enabled, 240_enabled;

PID zeroPID(&zeroIn, &zeroOut, &zeroSet, pidZeroP, pidZeroI, pidZeroD, DIRECT);
PID 120PID(&120In, &120Out, &120Set, pid120P, pid120I, pid120D, DIRECT);
PID 240PID(&240In, &240Out, &240Set, pid240P, pid240I, pid240D, DIRECT);

int PIDEncoderCheck(){
  int rv=1;
  #ifdef DEBUGPRINT
  double dummy;
  if(iic_encoder_read(ENCODER_ZERO_ADDR,&dummy)){
    DEBUGPRINT("Zero Encoder Data OK!");
  } else {
    DEBUGPRINT("Zero Encoder Data Bus Fault!");
    rv = -1;
  }
  if(iic_encoder_read(ENCODER_120_ADDR,&dummy)){
    DEBUGPRINT("120 Encoder Data OK!");
  } else {
    DEBUGPRINT("120 Encoder Data Bus Fault!");
    rv = -1;
  }
  if(iic_encoder_read(ENCODER_240_ADDR,&dummy)){
    DEBUGPRINT("240 Encoder Data OK!");
  } else {
    DEBUGPRINT("240 Encoder Data Bus Fault!");
    rv = -1;
  }  
  #endif
  return rv;
}

void PIDDrive(){
  if(zeroPID.NeedsCompute()){
    if(iic_encoder_read(ENCODER_ZERO_ADDR,&zeroIn)){
    //Successfully read the left encoder
      zeroPID.Compute();
    } else {
      //Bus fault!
      zeroOut=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(Zero): ");
    SerCommDbg.print(zeroOut);
    SerCommDbg.println();
    #endif
    drive_zero(zero_enabled,zeroOut);
  }
  if(120PID.NeedsCompute()){
    if(iic_encoder_read(ENCODER_120_ADDR,&120In)){
    //Successfully read the left encoder
      120PID.Compute();
    } else {
      //Bus fault!
      120Out=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(120): ");
    SerCommDbg.print(120Out);
    SerCommDbg.println();
    #endif
    drive_120(120_enabled,120Out);
 }
  if(240PID.NeedsCompute()){
    if(iic_encoder_read(ENCODER_240_ADDR,&240In)){
    //Successfully read the left encoder
      240PID.Compute();
    } else {
      //Bus fault!
      240Out=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(240): ");
    SerCommDbg.print(240Out);
    SerCommDbg.println();
    #endif
    drive_240(240_enabled,240Out);
 } 
}

void PIDInit(){
    PIDLoadTunings(); //Load the PID tunings from the EEPROM
    PIDRefreshTunings();
    zeroPID.SetMode(MANUAL);
    120PID.SetMode(MANUAL);
    240PID.SetMode(MANUAL);
    zeroPID.SetSampleTime(PID_SAMPLE_TIME);
    120PID.SetSampleTime(PID_SAMPLE_TIME);
    240PID.SetSampleTime(PID_SAMPLE_TIME);
    zeroPID.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
    120PID.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
    240PID.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
}

void PIDRefreshTunings(){
  zeroPID.SetTunings(pidZeroP, pidZeroI, pidZeroD);
  120PID.SetTunings(pid120P, pid120I, pid120D);  
  240PID.SetTunings(pid240P, pid240I, pid240D);  
}

void PIDTuner(){
  //Parse the data relating to the local PID tuner
  static char serialInputBuffer[PID_SERIAL_BUFFER_SIZE];
  static char serialInputBufferIndex = 0;
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

