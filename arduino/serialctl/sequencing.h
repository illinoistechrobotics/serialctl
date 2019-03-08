#define BUTTONS 4
#define LIGHT_SENSOR_TICKS 3
#define DETECT_THRESHOLD 70 // analogRead returns a number out of 1024
#define USE_SABERTOOTH 255
const int light_sensor_pins[] = {A3, A4, A5, A6};
const int fire_pins[] = {USE_SABERTOOTH, USE_SABERTOOTH, 51, 53};
const int fire_motors[] = {0, 1, 0, 0};
int ticks_detected[BUTTONS]; 

bool play_game = false;

void init_sequencing();
void start_sequencing();
void stop_sequencing();
void tick_sequencing();