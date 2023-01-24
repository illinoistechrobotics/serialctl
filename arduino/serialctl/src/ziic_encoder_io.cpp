#include <Arduino.h>
#include <Wire.h>
#include "iic_encoder_io.h"
//Prepares for IIC communication with encoders
void setup_iic(){
    Wire.begin();        // join i2c bus (address optional for master)
}

//Reads the RPM from an encoder at 'addr' returns 1 for success and 0 if a bus fault is encountered.
//The value of dblptr is only written if the IIC read is successful.
int iic_encoder_read(char addr, double *outdbl, uint32_t *outticks){
    char *recvptr;
    char cnt;
    double recvdbl;
    uint32_t recvticks;

    Wire.requestFrom(addr, sizeof(double)); //+ sizeof(uint32_t)    // request a double from slave device 'addr'
    recvptr = (char*)&recvdbl + (sizeof(double)-1); //Point to MSB of double
    cnt=0;

    // TODO: Implementation is technically broken here! Rewrite this to not use pointers as vars.
    while (Wire.available()) { // slave may send less than requested
        *recvptr = Wire.read(); // receive a byte as character
        cnt++;
        if(recvptr == (char*)&recvdbl) // if we're done reading the double
        //  recvptr = (char*)&recvticks + (sizeof(uint32_t)-1); // start reading the tick counter
        //if(recvptr == (char*)&recvticks)
          break;
        recvptr--;
    }
    if(cnt != (sizeof(double) + sizeof(uint32_t))){
        if (cnt == sizeof(double)) {
            // V1.0
            *outdbl = recvdbl;
            *outticks = 0;
            return 1;
        } else {
        //Bus fault!
            return 0;
        }
    } else{
        *outdbl = recvdbl;
        *outticks = recvticks;
        return 1;
    }
}
