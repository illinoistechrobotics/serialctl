#include "serio.h"
#include <unistd.h>
#include <fcntl.h>
#include "base64.h"
#include "crc16.h"
#include "packet.h"
#include <string.h>
int bidx;
int serio_init(connection_t *ctx, const char *serdev){
        int opts;
        bidx=0;
        ctx->device=serdev;
        ctx->fd=open(ctx->device, O_RDWR | O_NOCTTY | O_NDELAY);
        printf("opened\n");
        if(ctx->fd == -1 || !isatty(ctx->fd) || tcgetattr(ctx->fd, &(ctx->spconfig)) < 0) {
                printf( "failed to open port %s \n",ctx->device);
                close(ctx->fd);
                return -1;
        }
        // 8N1
        (ctx->spconfig).c_cflag &= ~PARENB;
        (ctx->spconfig).c_cflag &= ~CSTOPB;
        (ctx->spconfig).c_cflag &= ~CSIZE;
        (ctx->spconfig).c_cflag |= CS8;
        // no flow control
        (ctx->spconfig).c_cflag &= ~CRTSCTS;

        (ctx->spconfig).c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
        (ctx->spconfig).c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

        cfmakeraw(&(ctx->spconfig)); // make raw

        // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
        (ctx->spconfig).c_cc[VMIN]  = 1;
        (ctx->spconfig).c_cc[VTIME]  = 0;

        cfsetispeed(&(ctx->spconfig), B57600);
        cfsetospeed(&(ctx->spconfig), B57600);
        tcsetattr(ctx->fd,TCSANOW,&(ctx->spconfig));
        opts = fcntl(ctx->fd,F_GETFL);
        if (opts < 0) {
                perror("fcntl(F_GETFL)");
                return -1;
        }
        opts = (opts & ~(O_NDELAY));
        if (fcntl(ctx->fd,F_SETFL,opts) < 0) {
                perror("fcntl(F_SETFL)");
                return -1;
        }
        printf("serial port opened\n");
        tcflush(ctx->fd,TCIOFLUSH);
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
ssize_t serio_recv(connection_t *ctx, char *buf)
{ 
        static char ba[RECVBUF];
        char *next;
        do { 
                if(bidx == RECVBUF-1){
                        bidx=0;
                        printf("Input buffer overflow!\n");
                        memset(ba,0x00,RECVBUF);
                }
                int n = read(ctx->fd, ba+bidx, RECVBUF-(bidx+1));  // read as much as possible
                if( n==-1) return -1;    // couldn't read
                if( n==0 ) {
                        usleep( 5 * 1000 ); // wait 5 msec try again
                        continue;
                }
                #ifdef DEBUG
                printf("read(): %i, bidx: %i\n",n, bidx);
                #endif
                bidx += n;
                ba[bidx]=0x00;
        } while(strchr(ba,'\n') == NULL);
        buf[0]=0x00;
        next = strchr(ba,'\n');
        next[0] = 0x00;
        next++;
        //Heartbeat or data?
        #ifdef DEBUG
        printf("%x\n",ba[0]);
        #endif
        if(ba[0] != '$'){
                strcpy(buf,ba);
        }
        //Move rest including null to front
        memmove(ba,next,strlen(next)+1);
        //update length
        bidx = strlen(ba);
        return strlen(buf);
}

