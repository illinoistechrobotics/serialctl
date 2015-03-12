#pragma once

#ifdef __cplusplus
extern "C"{
#endif

//Include ASM optimised version for arduino

uint16_t compute_crc(char *data, int len);

#ifdef __cplusplus
} // extern "C"
#endif
