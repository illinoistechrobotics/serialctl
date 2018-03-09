#include <Wire_ng.h>
#include <math.h>

#define GYRO_ACCEL_ADDR 0x68 // I2C address of the MPU-6050

void MPUInit();
int MPULoop();
