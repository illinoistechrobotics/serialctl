#include "SDL.h"
#include "packet.h"

int joystick_init(int id);
int joystick_update(packet_t *ctl);
int joystick_wait_safe();
void joystick_release();
