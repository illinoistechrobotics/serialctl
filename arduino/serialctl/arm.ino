#define CONTROLLER ST12
#define PIN 1
#define POWER 50
#define THRESHOLD 150
#define MIN_EXTEND 50
#define MAX_EXTEND 42000

volatile long count, last_movement;
long lag;

void arm_setup() {
  arm_safe();
  homed = 0;
  attachInterrupt(A_INT,isrA,RISING);
}

void arm_loop() {
  if(comm_ok){
  switch (homed) {
    case 1:
      CONTROLLER.motor(PIN, -POWER);
      homed = 2;
      last_movement = millis();
      break;
    case 2:
      lag = millis() - last_movement;
      if (lag > THRESHOLD) { // we've stopped
        homed = 3;
        count = 0;
        CONTROLLER.motor(PIN, 0);
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
  CONTROLLER.motor(PIN, 0);
}

void isrA() {
  if (digitalRead(B)) { // B leading
    count++;
  } 
  else { // A leading
    count--;
  }
  last_movement = millis();
}

void move_arm(int8_t dir, bool precision) {
  if (homed == 3) {
    if (abs(dir) != 1) {
        CONTROLLER.motor(PIN, 0);
        return; 
    }
    speed = (precision ? LINAC_PRECISION : 127)*dir;
    if (dir < 0 && count < MIN_EXTEND ||
        dir > 0 && count > MAX_EXTEND )
        return;
    CONTROLLER.motor(PIN, speed);
  }
}
