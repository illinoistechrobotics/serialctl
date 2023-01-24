#include <Arduino.h>

#include <Wire.h>

void setup_iic(){
    Wire.begin();        // join i2c bus (address optional for master)
}

//Reads the RPM from an encoder at 'addr' returns 1 for success and 0 if a bus fault is encountered.
//The value of dblptr is only written if the IIC read is successful.
int iic_encoder_read(char addr, double *outdbl){
    char *recvptr;
    char cnt;
    double recvdbl;

    Wire.requestFrom(addr, sizeof(double));    // request a double from slave device 'addr'
    recvptr = (char*)&recvdbl + (sizeof(double)-1); //Point to MSB of double
    cnt=0;

    while (Wire.available()) { // slave may send less than requested
        *recvptr = Wire.read(); // receive a byte as character
        cnt++;
        if(recvptr == (char*)&recvdbl)
          break;
        recvptr--;
    }
    if(cnt != sizeof(double)){
        //Bus fault!
        return 0;
    } else{
        *outdbl = recvdbl;
        return 1;
    }
}


void setup() {
  setup_iic();
  Serial.begin(115200);  // start serial for output
}

void loop() {
  double left, right;
  Serial.println("test:");
  if(iic_encoder_read(6,&left)){
    Serial.print("LEFT (RPM): ");
    Serial.println(left);
  }
  if(iic_encoder_read(5,&right)){
    Serial.print("RIGHT (RPM): ");
    Serial.println(right);
  }
  
  delay(100);
}