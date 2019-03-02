#define BUTTONS 4
#define LIGHT_SENSOR_TICKS 3
#define DETECT_THRESHOLD 70 // analogRead returns a number out of 1024
const int light_sensor_pins[] = {A0, A1, A2, A3};
const int fire_pins[] = {47, 49, 51, 53};
int ticks_detected[BUTTONS]; 

bool play_game = false;

void init_sequencing();
void start_sequencing();
void stop_sequencing();
void tick_sequencing();
