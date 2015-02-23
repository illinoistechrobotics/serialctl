#include "serio.h"
#include "base64.h"
#include "crc16.h"

int serio_init(connection_t *ctx, const char *serdev){
        ctx->device=serdev;
        ctx->fd=open(ctx->device, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if(ctx->fd == -1 || !isatty(ctx->fd) || tcgetattr(ctx->fd, &(ctx->spconfig)) < 0) {
                printf( "failed to open port %s \n",ctx->device);
                close(ctx->fd);
                return -1;
        }
        //TODO full config
        cfmakeraw(&(ctx->spconfig));
        cfsetispeed(&(ctx->spconfig), B115200);
        cfsetospeed(&(ctx->spconfig), B115200);
        tcsetattr(ctx->fd,TCSANOW,&(ctx->spconfig));
        return 0;
}
void serio_close(connection_t *ctx){
        close(ctx->fd);
}

ssize_t serio_send(connection_t *ctx, void *data, size_t len){
        int rv;
        char datap[2+B64_ENC_LEN(len)];
        if(ctx->fd==-1 || len==0 || data==NULL){
                return -3;
        }
        //Add 2 bytes overhead, encode and send
        datap[0] = SFRAME;
        rv = base64_encode(datap+1,data,len);
        if(B64_ENC_LEN(len) != rv){
                return -2;
        }
        datap[1+B64_ENC_LEN(len)] = EFRAME;
        return write(ctx->fd,datap,2+B64_ENC_LEN(len));
}

