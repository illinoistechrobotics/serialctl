#pragma once
void init_pins();
void init_servos(HardwareSerial &serial_port, int control_pin, int servo_id);
void print_data();
void drive_left(int power);
void drive_right(int power);
void move_arm(int8_t cmd);
void manipulator();
void manipulator_ball();
void manipulator_key();
uint8_t getButton(int num);
