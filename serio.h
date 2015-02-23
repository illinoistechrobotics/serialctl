#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#define SFRAME 0x5b
#define EFRAME 0x5d

typedef struct{
        int fd;
        struct termios spconfig;
        const char *device;
} connection_t;


int serio_init(connection_t *ctx, const char *serdev);
ssize_t serio_send(connection_t *ctx, char *data, size_t len);
void serio_close(connection_t *ctx);
