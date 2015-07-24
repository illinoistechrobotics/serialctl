#include <ncurses.h>
#include <cdk.h>
#include "packet.h"
void draw_borders(WINDOW *screen);
int init_ui();
void refresh_ui(packet_t * ctl, char * msg, int overflow);
