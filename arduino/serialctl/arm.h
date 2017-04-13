#define A_INT 0
#define B_PIN 3
#define POWER 50
#define THRESHOLD 150
#define MIN_EXTEND 50
#define MAX_EXTEND 42000
extern char homed;
void arm_setup();
void arm_loop();
void arm_safe();
void move_arm(int8_t, bool);
