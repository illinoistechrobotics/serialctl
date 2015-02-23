#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct{
        int fd;
        struct termios spconfig;
        const char *device;
} connection_t;


int serio_init(connection_t *ctx, const char *serdev);
ssize_t serio_send(connection_t *ctx, void *data, size_t len);
void serio_close(connection_t *ctx);
