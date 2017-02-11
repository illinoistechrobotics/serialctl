#pragma once

#include <stdint.h>
#define SFRAME 0x5b
#define EFRAME 0x5d
enum comm_state {
  COMM_WAIT,
  COMM_RECV,
  COMM_COMPLETE,
  COMM_VALID,
  COMM_INVALID };
typedef struct p_t_struct{
  #ifdef __cplusplus
p_t_struct(): stickRX(128), stickRY(128), stickLX(128), stickLY(128), btnhi(0), btnlo(0), cksum(0){}
  #endif
  uint8_t stickRX;
  uint8_t stickRY;
  uint8_t stickLX;
  uint8_t stickLY;
  uint8_t btnhi;
  uint8_t btnlo;
  uint16_t cksum;
} packet_t;
