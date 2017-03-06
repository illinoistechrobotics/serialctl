#include <Wire.h>

void setup() {
  setup_iic();
  Serial.begin(9600);  // start serial for output
}

void loop() {
  double left, right;
  Serial.println("test:");
  if(iic_encoder_read(5,&left)){
    Serial.print("LEFT (RPM): ");
    Serial.println(left);
  }
  if(iic_encoder_read(6,&right)){
    Serial.print("RIGHT (RPM): ");
    Serial.println(right);
  }
  
  delay(100);
}
