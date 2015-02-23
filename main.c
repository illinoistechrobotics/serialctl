#include "serio.h"
#include "crc16.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char ** argv){
        char message[] = "Hello World";
        connection_t c;
        serio_init(&c, "/dev/pts/3");
        printf("CRC of message is %u\n", compute_crc(message, strlen(message)));
        serio_send(&c, message, strlen(message));
        return 0;
}
