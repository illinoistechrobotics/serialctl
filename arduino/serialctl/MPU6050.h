#include <Wire.h>
#include <Math.h>

#define GYRO_ACCEL_ADDR 0x68 // I2C address of the MPU-6050

void MPUInit();
void MPULoop();
