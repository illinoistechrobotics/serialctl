#pragma once
#include <Wire.h>

void init_pins();
void print_data();
void tank_drive();
void arcade_drive();
void estop();
int getButton(int num);
