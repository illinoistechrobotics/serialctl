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
typedef struct{
uint8_t leftStickX;
uint8_t leftStickY;
uint8_t rightStickX;
uint8_t rightStickY;
uint8_t btnhi;
uint8_t btnlo;
uint16_t cksum;
} packet_t;
