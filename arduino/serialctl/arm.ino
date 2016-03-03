#define CONTROLLER ST12
#define PIN 1
#define POWER 40
#define THRESHOLD 150
#define MIN_EXTEND 50
#define MAX_EXTEND 20000

volatile long count, last_movement;
long lag;

void arm_setup() {
  arm_safe();
  homed = 0;
}

void arm_loop() {
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

void move_arm(int8_t dir) {
  if (homed == 3) {
    switch(dir) {
      case -1: // backward
        if (count > MIN_EXTEND) { CONTROLLER.motor(PIN, -127); }
        else { CONTROLLER.motor(PIN, 0); }
        break;
      case 1: // forward
        if (count < MAX_EXTEND) { CONTROLLER.motor(PIN, 127); }
        else { CONTROLLER.motor(PIN, 0); }
        break;
      default: // stop
        CONTROLLER.motor(PIN, 0);
        break; 
    }
  }
}
