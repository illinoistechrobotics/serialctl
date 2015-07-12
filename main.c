#include "serio.h"
#include "crc16.h"
#include "joystick.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include "udp_io.h"
packet_t ctl;
void packet_crc(packet_t *p){
        p->cksum = htons(compute_crc((char *)p,sizeof(packet_t)-sizeof(uint16_t)));
}

int run_udp(const char *port){
        ip_connection_t c;
        char msg[RECVBUF];
        short loop=1;
        if(udpio_init(&c, port))
                return 2;
        while(loop){
                if(udpio_recv(&c, msg) < 0){
                        printf("Error reading data!\n");
                        return 2;
                }
                if(joystick_update(&ctl) != 0){
                        return 1;
                }
                packet_crc(&ctl); 
                if(udpio_send(&c, &ctl, sizeof(packet_t)) < 0){
                        printf("Unable to send data!\n");
                        return 2;
                }
                printf("UDP:[X: %i, Y: %i, CRC: %i, Resp: %s]\n", ctl.stickX, ctl.stickY, ctl.cksum, msg); 
                //        usleep(150E3);
        }
        return 0;
}

int run_serial(const char *port){
        connection_t c;
        char msg[RECVBUF];
        short loop=1;
        if(serio_init(&c, port))
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
                printf("Serial:[X: %i, Y: %i, CRC: %i, Resp: %s]\n", ctl.stickX, ctl.stickY, ctl.cksum, msg); 
                //        usleep(150E3);
        }
        return 0;
}
int main(int argc, char ** argv){
        if(argc != 4){
                printf("Usage: ./control serial|udp <portspec> <joystick_num>\n");
                return 3;
        }
        if(joystick_init(atoi(argv[3])) != 0)
                return 1;
        if(joystick_wait_safe() != 0)
                return 1;
        if(strstr(argv[1], "serial") != NULL)
                return run_serial(argv[2]);
        else if(strstr(argv[1], "udp") != NULL)
                return run_udp(argv[2]);
        else{
                printf("Invalid portspec, must be serial or udp!\n");
                return 3;
        }

}
