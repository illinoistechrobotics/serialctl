#include <ncurses.h>
void draw_borders(WINDOW *screen);
int init_ui();
void refresh_ui();

static int parent_x, parent_y, new_x, new_y;
static WINDOW *master; 
static WINDOW *status;
const static int status_size = 3;


