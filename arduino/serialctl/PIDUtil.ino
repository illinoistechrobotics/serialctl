#include "PIDUtil.h"
double pid0P=0, pid0I=0, pid0D=0, pid120P=0, pid120I=0, pid120D=0, pid240P, pid240I, pid240D;
double In0=0, Out0=0, Set0=0, In120=0, Out120=0, Set120=0, In240=0, Out240=0, Set240=0;
extern char comm_ok, enabled_0, enabled_120, enabled_240;

PID PID0(&In0, &Out0, &Set0, pid0P, pid0I, pid0D, DIRECT);
PID PID120(&In120, &Out120, &Set120, pid120P, pid120I, pid120D, DIRECT);
PID PID240(&In240, &Out240, &Set240, pid240P, pid240I, pid240D, DIRECT);

int PIDEncoderCheck(){
  int rv=1;
  #ifdef DEBUGPRINT
  double dummy;
  if(iic_encoder_read(ENCODER_0_ADDR,&dummy)){
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
  if(PID0.NeedsCompute()){
    if(iic_encoder_read(ENCODER_0_ADDR,&In0)){
    //Successfully read the left encoder
      PID0.Compute();
    } else {
      //Bus fault!
      Out0=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(Zero): ");
    SerCommDbg.print(Out0);
    SerCommDbg.println();
    #endif
    drive_0(enabled_0,Out0);
  }
  if(PID120.NeedsCompute()){
    if(iic_encoder_read(ENCODER_120_ADDR,&In120)){
    //Successfully read the left encoder
      PID120.Compute();
    } else {
      //Bus fault!
      Out120=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(120): ");
    SerCommDbg.print(Out120);
    SerCommDbg.println();
    #endif
    drive_120(enabled_120,Out120);
 }
  if(PID240.NeedsCompute()){
    if(iic_encoder_read(ENCODER_240_ADDR,&In240)){
    //Successfully read the left encoder
      PID240.Compute();
    } else {
      //Bus fault!
      Out240=0;
    }
    #ifdef PRINTMOTORS
    SerCommDbg.print("PID(240): ");
    SerCommDbg.print(Out240);
    SerCommDbg.println();
    #endif
    drive_240(enabled_240,Out240);
 } 
}

void PIDInit(){
    PIDLoadTunings(); //Load the PID tunings from the EEPROM
    PIDRefreshTunings();
    PID0.SetMode(MANUAL);
    PID120.SetMode(MANUAL);
    PID240.SetMode(MANUAL);
    PID0.SetSampleTime(PID_SAMPLE_TIME);
    PID120.SetSampleTime(PID_SAMPLE_TIME);
    PID240.SetSampleTime(PID_SAMPLE_TIME);
    PID0.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
    PID120.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
    PID240.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
}

void PIDRefreshTunings(){
  PID0.SetTunings(pid0P, pid0I, pid0D);
  PID120.SetTunings(pid120P, pid120I, pid120D);  
  PID240.SetTunings(pid240P, pid240I, pid240D);  
}

void PIDTuner(){
  //Parse the data relating to the local PID tuner
  static char serialInputBuffer[PID_SERIAL_BUFFER_SIZE];
  static char serialInputBufferIndex = 0;
  static char currentPIDValueIs0 = 1;
  static char currentPIDValueIs120 = 0;
  static char currentPIDValueIs240 = 0;
  static double *currentPIDValueToUpdate = &pid0P;
  //Serial Input for PID configuration
  if (SerCommDbg.available()) {
    int incomingByte = SerCommDbg.read();
    switch(incomingByte) {
      case '0':
        currentPIDValueIs0 = 1;
        currentPIDValueIs120 = 0;
        currentPIDValueIs240 = 0;
        break;
      case '120':
        currentPIDValueIs0 = 0;
        currentPIDValueIs120 = 1;
        currentPIDValueIs240 = 0;        
        break;
      case '240':
        currentPIDValueIs0 = 0;
        currentPIDValueIs120 = 0;
        currentPIDValueIs240 = 1;
        break;
      case 'P':
      case 'p':
        if (currentPIDValueIs0) {
          currentPIDValueToUpdate = &pid0P;
        }
        if (currentPIDValueIs120) {
          currentPIDValueToUpdate = &pid120P;
        }
        if (currentPIDValueIs240) {
          currentPIDValueToUpdate = &pid240P;
        }
        break;
      case 'I':
      case 'i':
        if (currentPIDValueIs0) {
          currentPIDValueToUpdate = &pid0I;
        }
        if (currentPIDValueIs120) {
          currentPIDValueToUpdate = &pid120I;
        }
        if (currentPIDValueIs240) {
          currentPIDValueToUpdate = &pid240I;
        }
        break;
      case 'D':
      case 'd':
        if (currentPIDValueIs0) {
          currentPIDValueToUpdate = &pid0D;
        }
        if (currentPIDValueIs120) {
          currentPIDValueToUpdate = &pid120D;
        }
        if (currentPIDValueIs240) {
          currentPIDValueToUpdate = &pid240D;
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
        SerCommDbg.print("Zero P ");
        SerCommDbg.println(pid0P,8);
        SerCommDbg.print("Zero I ");
        SerCommDbg.println(pid0I,8);
        SerCommDbg.print("Zero D ");
        SerCommDbg.println(pid0D,8);
        SerCommDbg.print("120 P ");
        SerCommDbg.println(pid120P,8);
        SerCommDbg.print("120 I ");
        SerCommDbg.println(pid120I,8);
        SerCommDbg.print("120 D ");
        SerCommDbg.println(pid120D,8);
        SerCommDbg.print("240 P ");
        SerCommDbg.println(pid240P,8);
        SerCommDbg.print("240 I ");
        SerCommDbg.println(pid240I,8);
        SerCommDbg.print("240 D ");
        SerCommDbg.println(pid240D,8);
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
          if (currentPIDValueToUpdate == &pid0P) {
            
          }
          if (currentPIDValueToUpdate == &pid0I) {
            SerCommDbg.print("Zero I ");
          }
          if (currentPIDValueToUpdate == &pid0D) {
            SerCommDbg.print("Zero D ");
          }
          if (currentPIDValueToUpdate == &pid120P) {
            SerCommDbg.print("120 P ");
          }
          if (currentPIDValueToUpdate == &pid120I) {
            SerCommDbg.print("120 I ");
          }
          if (currentPIDValueToUpdate == &pid120D) {
          SerCommDbg.print("120 D ");
          }
          if (currentPIDValueToUpdate == &pid240P) {
            SerCommDbg.print("240 P ");
          }
          if (currentPIDValueToUpdate == &pid240I) {
            SerCommDbg.print("240 I ");
          }
          if (currentPIDValueToUpdate == &pid240D) {
            SerCommDbg.print("240 D ");
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
  EEPROM.put(FS_0_P_4,pid0P);
  EEPROM.put(FS_0_I_4,pid0I);
  EEPROM.put(FS_0_D_4,pid0D);
  EEPROM.put(FS_120_P_4,pid120P);
  EEPROM.put(FS_120_I_4,pid120I);
  EEPROM.put(FS_120_D_4,pid120D);
  EEPROM.put(FS_240_P_4,pid240P);
  EEPROM.put(FS_240_I_4,pid240I);
  EEPROM.put(FS_240_D_4,pid240D);
}

void PIDLoadTunings(){
  //Load the PID tunings from the EEPROM
  //PID library cannot recover from NaN internal state
  EEPROM.get(FS_0_P_4,pid0P);
  if(pid0P == NAN)
    pid0P = 0;
  EEPROM.get(FS_0_I_4,pid0I);
  if(pid0I == NAN)
    pid0I = 0;
  EEPROM.get(FS_0_D_4,pid0D);
  if(pid0D == NAN)
    pid0D = 0;
  EEPROM.get(FS_120_P_4,pid120P);
  if(pid120P == NAN)
    pid120P = 0;
  EEPROM.get(FS_120_I_4,pid120I);
  if(pid120I == NAN)
    pid120I = 0;
  EEPROM.get(FS_120_D_4,pid120D);
  if(pid120D == NAN)
    pid120D = 0;
  EEPROM.get(FS_240_P_4,pid240P);
  if(pid240P == NAN)
    pid240P = 0;
  EEPROM.get(FS_240_I_4,pid240I);
  if(pid240I == NAN)
    pid240I = 0;
  EEPROM.get(FS_240_D_4,pid240D);
  if(pid240D == NAN)
    pid240D = 0;
        
  PIDRefreshTunings();
}
