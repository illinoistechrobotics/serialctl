#pragma once
void init_pins();
void init_servos(HardwareSerial &serial_port, int control_pin, int servo_id);
void print_data();
void drive_left(int power);
void drive_right(int power);
uint8_t get_button(int num);
