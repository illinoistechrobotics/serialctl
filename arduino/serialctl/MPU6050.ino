#include "MPU6050.h"

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int rotationThreshold = 1;
int loopSpeed = 100;      // set this depending on ms to execute loop

void MPUInit() {
  Wire.begin();
  Wire.beginTransmission(GYRO_ACCEL_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  // Serial.begin(9600);
}

int MPULoop() {
  Wire.beginTransmission(GYRO_ACCEL_ADDR);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(GYRO_ACCEL_ADDR,14,true);  // request a total of 14 registers
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  SerCommDbg.print("AcX = "); SerCommDbg.print(AcX);
  SerCommDbg.print(" | AcY = "); SerCommDbg.print(AcY);
  SerCommDbg.print(" | AcZ = "); SerCommDbg.print(AcZ);
  SerCommDbg.print(" | Tmp = "); SerCommDbg.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  SerCommDbg.print(" | GyX = "); SerCommDbg.print(GyX);
  SerCommDbg.print(" | GyY = "); SerCommDbg.print(GyY);
  SerCommDbg.print(" | GyZ = "); SerCommDbg.println(GyZ);

  if (GyZ >= rotationThreshold || GyZ <= -rotationThreshold) {
    GyZ /= loopSpeed;
    angle += GyZ;
  }

  if (angle < 0) {
    angle += 360;
  } else if (angle > 359) {
    angle -= 360;
  }
  
  SerCommDbg.print("Angle of Rotation: "); SerCommDbg.println(angle);
  return angle;
  //delay(333);
}

