#include "eeprom_fs.h"
#include <EEPROM.h>
#include <PID_v2.h>

#define PID_OUTPUT_LIMIT 96.0
#define PID_SAMPLE_TIME 20
#define ENCODER_LEFT_ADDR 6
#define ENCODER_RIGHT_ADDR 5

void PIDWriteTunings();
void PIDLoadTunings();
void PIDTuner();
void PIDInit();
void PIDDrive();
void PIDRefreshTunings();
int PIDEncoderCheck();
extern PID leftPID;
extern PID rightPID;
