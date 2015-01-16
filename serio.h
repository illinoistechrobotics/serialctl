#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
typedef struct{
        short int status;
        int fd;
        struct termios spconfig;
        char *device;
} connection_t;


int serio_init(connection_t *ctx, const char *serdev);
void serio_error(const char *errorstr);
