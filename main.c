#include "serio.h"
#include "crc16.h"
#include "joystick.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
void packet_crc(packet_t *p){
    p->cksum = htons(compute_crc((char *)p,sizeof(packet_t)-sizeof(uint16_t)));
    }

int main(int argc, char ** argv){
        char msg[RECVBUF];
        if(!(argc == 3 || argc == 4)){
            printf("Usage: ./serialctl <serial port> <joystick_num> [data_file]\n");
            return 3;
            }
	int data_file = -1;
	if(argc == 4){
	  if(-1 == (data_file = open(argv[3], O_WRONLY|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))){
	    perror("failed to open data file (does it already exist?)");
	    return 4;
	  }
	}
        short loop=1;
        connection_t c;
        packet_t ctl;
        if(serio_init(&c, argv[1]) != 0){
	  printf("Unable to open serial port\n");
	  return 2;
	}
        if(joystick_init(atoi(argv[2])) != 0){
	  printf("Unable to open joystick\n");
          return 1;
	}
        if(joystick_wait_safe() != 0){
	  printf("Unable to get safe values from the joystick\n");
          return 1;
	}
	init_ui();
        while(loop){
	  int overflow = 0;
	  //TODO: printf("Input buffer overflow!\n");
          if(serio_recv(&c, msg, &overflow) < 0){
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
	  //printf("X: %i, Y: %i, CRC: %i, Resp: %s\n", ctl.stickX, ctl.stickY, ctl.cksum, msg); 
	  if(data_file != -1){
	    char output_buffer[100];
	    int size = snprintf(output_buffer, 100, "%s\n", msg);
	    if(size > 0){
	      write(data_file,output_buffer,size);
	    }
	  }
	  refresh_ui(&ctl, msg, overflow);
        }
        return 0;
}
