#include <sys/types.h>
uint16_t crc16_update(uint16_t crc, uint8_t data);
uint16_t compute_crc(char *data, int len);
