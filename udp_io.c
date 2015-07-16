#include "udp_io.h"
#include "packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "base64.h"
#include "crc16.h"
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
int iidx;
int udpio_init(ip_connection_t *c, const char *port){
        char* pp;
        char parsedarg[64];
        unsigned int nport;
        bzero(parsedarg,64);
        strncpy(parsedarg,port,63);

        if((pp=strchr(parsedarg, ':')) == NULL){
                printf("Missing port number!");
                return 1;
        }

        pp[0]=0x00;
        pp++;
        nport = atoi(pp);
        if(nport <= 0 ){
                printf("Invalid port number\n");
                return 1;
        }
        printf("IP = %s, PORT = %i\n", parsedarg, nport);
        //Client socket
        c->c_sockfd=socket(AF_INET,SOCK_DGRAM,0);
        bzero(&(c->cliaddr),sizeof(struct sockaddr_in));
        (c->cliaddr).sin_family = AF_INET;
        (c->cliaddr).sin_addr.s_addr=inet_addr(parsedarg);
        (c->cliaddr).sin_port=htons(nport);
        fcntl(c->c_sockfd, F_SETFL, fcntl(c->c_sockfd, F_GETFL) | O_NONBLOCK);
        //Recieve/server socket
        c->s_sockfd=socket(AF_INET,SOCK_DGRAM,0);
        bzero(&(c->servaddr),sizeof(struct sockaddr_in));
        (c->servaddr).sin_family = AF_INET;
        (c->servaddr).sin_addr.s_addr=htonl(INADDR_ANY);
        (c->servaddr).sin_port=htons(nport);
        if (bind((c->s_sockfd), (struct sockaddr *)&(c->servaddr), sizeof(struct sockaddr_in)) < 0) {
                perror("bind failed");
                return 1;
        }
        //nonblocking sender
        fcntl(c->s_sockfd, F_SETFL, fcntl(c->c_sockfd, F_GETFL) | O_NONBLOCK);


        sendto(c->c_sockfd,"[",1,0,(struct sockaddr *)&(c->cliaddr),sizeof(c->cliaddr));

        return 0;
}

ssize_t udpio_recv(ip_connection_t *ctx, char *buf)
{ 
        static char ba[RECVBUF];
        static unsigned int fc=0;
        char *next, *t;
        do { 
                if(iidx == RECVBUF-1){
                        iidx=0;
                        printf("Input buffer overflow!\n");
                        memset(ba,0x00,RECVBUF);
                }
                int n = recv(ctx->s_sockfd, ba+iidx, RECVBUF-(iidx+1), 0);  // read as much as possible
                if( n==-1 && errno != EWOULDBLOCK){
                        return -1;    // couldn't read
                }
                if( n==0 || (n==-1 && errno == EWOULDBLOCK)) {
                        if(fc > 100){
                                printf("Sending noop, no data for 500ms!\n");
                                sendto(ctx->c_sockfd,"[",1,0,(struct sockaddr *)&(ctx->cliaddr),sizeof(ctx->cliaddr));
                                fc=0;
                        }
                        else{
                                fc++;
                        }
                        usleep( 5 * 1000 ); // wait 5 msec try again
                } else {
                        iidx += n;
                        ba[iidx]=0x00;
                }
                next=strchr(ba,'\n'); 
        } while(next == NULL);
        //Clobber newline and hop over it
        next[0] = 0x00;
        next++;
        //Heartbeat or data?
#ifdef DEBUG
        printf("%x\n",ba[0]);
#endif
        if(ba[0] != '$'){
                strcpy(buf,ba);
                if((t = strchr(buf,'\r')) != NULL){
                        t[0]=0x00;
                }
        } else {
                buf[0]=0x00;
        }
        //Move rest including null to front
        memmove(ba,next,strlen(next)+1);
        //update length
        iidx = strlen(ba);
        return strlen(buf);
}
ssize_t udpio_send(ip_connection_t *c, void *data, size_t len){
        int rv;
        char datap[2+B64_ENC_LEN(len)];
        if(len==0 || data==NULL){
                return -3;
        }
        //Add 2 bytes overhead, encode and send
        datap[0] = SFRAME;
        rv = base64_encode(datap+1,data,len);
        if(B64_ENC_LEN(len) != rv){
                return -2;
        }
        datap[1+B64_ENC_LEN(len)] = EFRAME;
        return sendto(c->c_sockfd,datap,2+B64_ENC_LEN(len),0,(struct sockaddr *)&(c->cliaddr),sizeof(c->cliaddr));
}
