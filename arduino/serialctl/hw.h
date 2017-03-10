#pragma once
void init_pins();
void print_data();
void drive_left(int power);
void drive_right(int power);
void move_arm(int8_t cmd);
void manipulator();
void manipulator_ball();
void manipulator_key();
uint8_t getButton(int num);
