#include "eeprom_fs.h"
#include <EEPROM.h>
#define PID_OUTPUT_LIMIT 255
#define PID_SAMPLE_TIME 20

void PIDWriteTunings();
void PIDLoadTunings();
void PIDTuner();
void PIDInit();
void PIDRefreshTunings();
extern PID leftPID;
extern PID rightPID;
