int serio_init(connection_t *ctx, const char *serdev){
    ctx->fd=open(device, O_RDWR | O_NOCTTY);
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
        char packbuf[len+(len/7)+1];
        int i, pidx=0, didx=0, bytes;
        if(ctx->fd==-1 || len=0 || data==NULL){
                return -1;
        }
        while((didx-len)>0){
                bytes=min(didx-len,7);
                memcpy(&(packbuf[pidx]), data+didx, bytes);
                pidx+=bytes;
                for(i=0;i<bytes;i++){
                    packbuf[pidx] |= (((*(char *)(data+didx+i)) & 0x01) << i);
                }
                pidx++;
                didx+=bytes;

        }








