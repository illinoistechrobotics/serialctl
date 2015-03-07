#pragma once

#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>
//Include ASM optimised version for arduino
#ifdef AVR
#include <util/crc16.h>
//Otherwise my own version
#else
inline uint16_t _crc16_update(uint16_t crc, uint8_t data);
#endif

inline uint16_t compute_crc(char *data, int len);

#ifdef __cplusplus
} // extern "C"
#endif
