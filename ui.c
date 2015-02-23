#include <unistd.h>
#include "ui.h"
void draw_borders(WINDOW *screen) {
        int x, y, i;

        getmaxyx(screen, y, x);

        // 4 corners
        mvwprintw(screen, 0, 0, "+");
        mvwprintw(screen, y - 1, 0, "+");
        mvwprintw(screen, 0, x - 1, "+");
        mvwprintw(screen, y - 1, x - 1, "+");

        // sides
        for (i = 1; i < (y - 1); i++) {
                mvwprintw(screen, i, 0, "|");
                mvwprintw(screen, i, x - 1, "|");
        }

        // top and bottom
        for (i = 1; i < (x - 1); i++) {
                mvwprintw(screen, 0, i, "-");
                mvwprintw(screen, y - 1, i, "-");
        }
}

int init_ui(){
        initscr();
        noecho();
        curs_set(FALSE);

        // set up initial windows
        getmaxyx(stdscr, parent_y, parent_x);

        master = newwin(parent_y - status_size, parent_x, 0, 0);
        status = newwin(status_size, parent_x, parent_y - status_size, 0);

        draw_borders(master);
        draw_borders(status);
        return 0;
}

void refresh_ui(){
        getmaxyx(stdscr, new_y, new_x);

        if (new_y != parent_y || new_x != parent_x) {
                parent_x = new_x;
                parent_y = new_y;

                wresize(master, new_y - status_size, new_x);
                wresize(status, status_size, new_x);
                mvwin(status, new_y - status_size, 0);

                wclear(stdscr);
                wclear(master);
                wclear(status);

                draw_borders(master);
                draw_borders(status);
        }

        // draw to our windows
        mvwprintw(master, 1, 1, "Field");
        mvwprintw(status, 1, 1, "Score");

        // refresh each window
        wrefresh(master);
        wrefresh(status);
}
