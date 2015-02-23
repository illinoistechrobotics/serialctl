int serio_init(connection_t *ctx, const char *serdev){
    ctx->fd=open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(fd == -1 || !isatty(fd) || tcgetattr(fd, &spconfig) < 0) {
         printf( "failed to open port %s \n",device);
         close(ctx->fd);
         return -1;
         }
    //TODO full config
    cfmakeraw(ctx.spconfig);
    cfsetispeed(ctx.spconfig, B115200);
    cfsetospeed(ctx.spconfig, B115200);
    tcsetattr(fd,TCSANOW,ctx.spconfig);
}
ssize_t serio_send(connection_t *ctx, char *data, size_t len){
        // Add 4 bytes overhead, base64 encode and send
        if(ctx->fd==-1 || len=0 || data==NULL){
                return -1;
        }
               


        }
}
