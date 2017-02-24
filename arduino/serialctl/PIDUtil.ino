
double pidLeftP=0, pidLeftI=0, pidLeftD=0, pidRightP=0, pidRightI=0, pidRightD=0;

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
      case 'M':
      case 'm':
        SerCommDbg.println("Reloading PID tunings from EEPROM");
        PIDLoadTunings();
        serialInputBufferIndex = 0;
        break;
      case '\n':
        serialInputBuffer[serialInputBufferIndex] = '\0';
        *currentPIDValueToUpdate = strtod(serialInputBuffer, NULL);
        SerCommDbg.print("Setting ");
        if (currentPIDValueToUpdate == &pidLeftP) {
          SerCommDbg.print("Left P ");
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
  EEPROM.get(FS_LEFT_P_4,pidLeftP);
  EEPROM.get(FS_LEFT_I_4,pidLeftI);
  EEPROM.get(FS_LEFT_D_4,pidLeftD);
  EEPROM.get(FS_RIGHT_P_4,pidRightP);
  EEPROM.get(FS_RIGHT_I_4,pidRightI);
  EEPROM.get(FS_RIGHT_D_4,pidRightD);
}

