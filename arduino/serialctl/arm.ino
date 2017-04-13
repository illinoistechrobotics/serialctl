#include <util/atomic.h>
#include "arm.h"

volatile long count, last_movement;
long lag;
char homed=0;

void arm_setup() {
  arm_safe();
  homed = 0;
  attachInterrupt(A_INT,isrA,RISING);
}

void arm_loop() {
  if(comm_ok){
  switch (homed) {
    case 1:
      setMotor(EXT_ARM_MOTOR,-POWER);
      homed = 2;
      /* Play it safe, these types are 4 words long! */
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        last_movement = millis();
      }
      break;
    case 2:
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        lag = millis() - last_movement;
      }
      if (lag > THRESHOLD) { // we've stopped
        homed = 3;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
          count = 0;
        }
        setMotor(EXT_ARM_MOTOR,0);
      }
      break;
    default:
      break;
  }
  } else {
    arm_safe();
  }
}

void arm_safe() {
  setMotor(EXT_ARM_MOTOR,0);
}

void isrA() {
  if (digitalRead(B_PIN)) { // B leading
    count++;
  } 
  else { // A leading
    count--;
  }
  last_movement = millis();
}

void move_arm(int8_t dir, bool precision) {
  unsigned long rcount;
  int m_speed;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    rcount=count;
  }
  if (homed == 3) {
    if (abs(dir) != 1) {
        setMotor(EXT_ARM_MOTOR,0);
        return; 
    }
    m_speed = (precision ? ARM_LINAC_PRECISION : 127)*dir;
    if (dir < 0 && rcount < MIN_EXTEND ||
        dir > 0 && rcount > MAX_EXTEND )
        return;
    setMotor(EXT_ARM_MOTOR,m_speed);
  }
}
