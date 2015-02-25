#include "serio.h"
#include "crc16.h"
#include "joystick.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

void packet_crc(packet_t *p){
    p->cksum = htons(compute_crc((char *)p,sizeof(packet_t)-sizeof(uint16_t)));
    }

int main(int argc, char ** argv){
        char msg[RECVBUF];
        if(argc != 3){
            printf("Usage: ./serialctl <serial port> <joystick_num>\n");
            return 3;
            }
        short loop=1;
        connection_t c;
        packet_t ctl;
        if(joystick_init(atoi(argv[2])) != 0)
          return 1;
        if(serio_init(&c, argv[1]))
            return 2;
        while(loop){
        if(serio_recv(&c, msg) < 0){
            printf("Error reading data!\n");
            return 2;
            }
        if(joystick_update(&ctl) != 0){
           return 1;
            }
        packet_crc(&ctl); 
        if(serio_send(&c, &ctl, sizeof(packet_t)) < 0){
            printf("Unable to send data!\n");
            return 2;
            }
        printf("x: %i, y: %i, CRC: %i, Remote end said: %s\n", ctl.stickX, ctl.stickY, ctl.cksum, msg); 
//        usleep(150E3);
        }
        return 0;
}
