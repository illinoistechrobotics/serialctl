#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#define SFRAME 0xf0
#define EFRAME 0xe0

typedef struct{
        int fd;
        struct termios spconfig;
        char *device;
} connection_t;


int serio_init(connection_t *ctx, const char *serdev);
void serio_close(connection_t *ctx);
