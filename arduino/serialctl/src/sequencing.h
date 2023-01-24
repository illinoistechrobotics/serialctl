#include <Arduino.h>

#define BUTTONS 4
#define LIGHT_SENSOR_TICKS 3
#define DETECT_THRESHOLD 70 // analogRead returns a number out of 1024
#define USE_SABERTOOTH 255
// Back left, Front left, Back right, Front right
const int light_sensor_pins[] = {A3, A4, A5, A6};
const int fire_pins[] = {9, 10, 26, 27};
extern int ticks_detected[BUTTONS];

extern bool play_game;

void init_sequencing();
void toggle_sequencing();
void tick_sequencing();
