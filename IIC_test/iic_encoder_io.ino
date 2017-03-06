#include <Wire.h>
//Prepares for IIC communication with encoders
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

