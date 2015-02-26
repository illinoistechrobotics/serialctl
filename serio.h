#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

//Serial debug output
#ifdef DEBUG
#undef DEBUG
#endif

#define RECVBUF 256
typedef struct{
        int fd;
        struct termios spconfig;
        const char *device;
} connection_t;


int serio_init(connection_t *ctx, const char *serdev);
ssize_t serio_send(connection_t *ctx, void *data, size_t len);
ssize_t serio_recv(connection_t *ctx, char *buf);
void serio_close(connection_t *ctx);
