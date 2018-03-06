#include "eeprom_fs.h"
#include <EEPROM.h>
#include <PID_v2.h>

#define PID_OUTPUT_LIMIT 64.0
#define PID_SAMPLE_TIME 20
#define ENCODER_0_ADDR 5
#define ENCODER_120_ADDR 6
#define ENCODER_240_ADDR 7

void PIDWriteTunings();
void PIDLoadTunings();
void PIDTuner();
void PIDInit();
void PIDDrive();
void PIDRefreshTunings();
int PIDEncoderCheck();
extern PID PID0;
extern PID PID120;
extern PID PID240;
