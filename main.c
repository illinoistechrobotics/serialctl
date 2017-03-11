#include "serio.h"
#include "crc16.h"
#include "joystick.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>

void packet_crc(packet_t *p){
    p->cksum = htons(compute_crc((char *)p,sizeof(packet_t)-sizeof(uint16_t)));
    }

int main(int argc, char ** argv){
        char msg[RECVBUF];
        if(argc != 4){
            printf("Usage: ./serialctl <serial port> <joystick_num> <file>\n");
            return 3;
            }
#ifdef RECORD
	int fd = open(argv[3], O_WRONLY | O_CREAT, 0444);
	if (fd < 0) {
#else
	char* line; size_t blen = 256;
	FILE* f = fopen(argv[3], "r");
	if (!f) {
#endif
		printf("Couldn't open or create file '%s'\n", argv[3]);
		return 4;
	}
        short loop=1;
        connection_t c;
        packet_t ctl;
        if(serio_init(&c, argv[1]))
        	return 2;
        if(joystick_init(atoi(argv[2])) != 0)
        	return 1;
        if(joystick_wait_safe() != 0)
        	return 1;
        while(loop){
        	if(serio_recv(&c, msg) < 0){
        		printf("Error reading data!\n");
        		return 2;
        	}
#ifdef PLAYBACK
		if (strstr(msg, "-FS-")) {
			ctl.stickX = 127;
			ctl.stickY = 127;
			ctl.btnlo = 0;
			ctl.btnhi = 0;
			packet_crc(&ctl);
		}
		else {
			int read = getline(&line, &blen, f);
			if (read == -1) break; // eof
		}
        		if (write(c.fd, line, (size_t)(read - 1)) < 0){ // -1 for trailing \n
#else
        	if (joystick_update(&ctl) != 0) return 1;
        	packet_crc(&ctl); 
        	if (serio_send(&c, &ctl, sizeof(packet_t), fd) < 0){
#endif
        		    printf("Unable to send/write data!\n");
        		    return 2;
        	    }
        	printf("X: %i, Y: %i, CRC: %i, Resp: %s\n", ctl.stickX, ctl.stickY, ctl.cksum, msg); 
//		usleep(150E3);
        }
        return 0;
}
