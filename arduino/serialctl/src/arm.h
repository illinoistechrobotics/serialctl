#include <Arduino.h>
#include <util/atomic.h>

#define A_INT 0
#define B_PIN 3
#define POWER 50
#define THRESHOLD 150
#define MIN_EXTEND 60
#define MAX_EXTEND 40000
extern char comm_ok;
extern char homed;
extern long armcount;
void arm_setup();
void arm_loop();
void arm_safe();
void move_arm(int8_t, bool);
