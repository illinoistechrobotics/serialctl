#include <Sabertooth.h>

//    serialctl
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#include "packet.h"
#include "hw.h"
#include "zserio.h"
#include "globals.h"
#include "PIDUtil.h"
#include "iic_encoder_io.h"

packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
char left_enabled = 0, right_enabled = 0;
long last_f = 0, last_s = 0, t_start = 0, usec;
Sabertooth ST12(128, SABERTOOTH12);
Sabertooth ST34(129, SABERTOOTH12);
static uint8_t reset_counter = 0;
static int power_constraint = 0;

#define WATCHDOG_

#ifdef WATCHDOG_
#include <avr/wdt.h>      //watchdog library timer loop resets the watch dog
#endif
unsigned int getButton(unsigned int num) {
  if (num <= 7) {
    return (astate->btnlo >> num) & 0x01;
  } else if (num > 7 && num <= 15) {
    return (astate->btnhi >> (num - 8)) & 0x01;
  } else {
    return 0;
  }
}
unsigned int getDPad() {
  // four bits: left down right up
  return (astate->btnhi >> 4);
}

void setMotor(int motorID, int output) {
  if (motorID <= 0) {
    return;
  }
  else if (motorID <= 2) {
    ST12.motor(motorID, output);
  }
  else if (motorID <= 4) {
    ST34.motor(motorID - 2, output);
  }
}

void setup() {
#ifdef WATCHDOG_
  wdt_enable(WDTO_250MS);  //Set 250ms WDT
  wdt_reset();             //watchdog timer reset
#endif

  //Initialize safe to safe values!!
  safe.stickX = 127;
  safe.stickY = 127;
  safe.btnhi = 0;
  safe.btnlo = 0;
  safe.cksum = 0b1000000010001011;
  SerComm.begin(57600);
  SerCommDbg.begin(115200);
  PIDInit(); //Initialize PID subsystem 
  comm_init(); //Initialize the communication FSM
  setup_iic();
  init_pins(); //Initialize pins and motor controllers (refer to hw.ino)
  last_f = millis();
  last_s = millis();
  drive_left(0,0);  //Ensure both motors are stopped
  drive_right(0,0); 
  t_start = millis(); //Save the start time
  //copy safe values over the current state
  memcpy(astate, &safe, sizeof(packet_t));

  //arm_setup();

#ifdef WATCHDOG_
  wdt_disable();  //long delay follows
#endif
  //wait at least ten seconds (TC of filters)
  //before measuring offset (variable delay so
  //as not to waste time spent homing arm)
  if ((t_start = millis() - t_start) < 10000) {
    delay(10000 - t_start);
  }
  measure_offset();
#ifdef WATCHDOG_
  wdt_enable(WDTO_250MS);  //Set 250ms WDT
  wdt_reset();             //watchdog timer reset
#endif
}
void loop() {
  //Main loop runs at full speed
  wdt_reset();
  comm_parse();
  left_enabled = try_enable_left(left_enabled);
  right_enabled = try_enable_right(right_enabled);
  //Fast loop
  if (millis() - last_f >= 40) {
    //Every line sent to the computer gets us a new state
    tank_drive();
    fast_loop();
    print_data();
    last_f = millis();
  }
  //Compute the PIDs if needed
  PIDDrive();
  //Slow loop
  //Runs this every ~500ms
  if (millis() - last_s >= 500) {
    slow_loop();
    last_s = millis();
  }
}
void fast_loop() {
  //About 25 iterations per sec
  PIDTuner();

  //arm
  //check for invalid states
  if ((getButton(5) ^ getButton(7))) {
    //both up and down buttons at same time is invalid
    if (getButton(7)) {
      setMotor(LOWER_ARM_MOTOR, -127);
    }
    else if (getButton(5)) {
      setMotor(LOWER_ARM_MOTOR, 127);
    }
  } else {
    setMotor(LOWER_ARM_MOTOR, 0);
  }
  
  if (getButton(JOYSTICK_PAD_UP) ^ getButton(JOYSTICK_PAD_DOWN)) {
    if (getButton(JOYSTICK_PAD_UP)) {
      setMotor(UPPER_ARM_MOTOR, 127);
    }
    else if (getButton(JOYSTICK_PAD_DOWN)) {
      setMotor(UPPER_ARM_MOTOR, -127);
    }
  }
  else {
    setMotor(UPPER_ARM_MOTOR, 0);
  }
} 
void slow_loop() {
  //2x per second
  //Compressor
 // compressor_ctl();
} 

void tank_drive() {
  int power_out = 0;
  int turn_out  = 0;
  int zeroed_power =    ((int)(astate->stickX) - 127);
  int zeroed_turn =     -1 * ((int)(astate->stickY) - 127);

  if (abs(zeroed_power) > DEADBAND_HALF_WIDTH) {
    if (zeroed_power > 0) {
      power_out = zeroed_power - DEADBAND_HALF_WIDTH;
    } else {
      power_out = zeroed_power + DEADBAND_HALF_WIDTH;
    }
  }
  if (abs(zeroed_turn) > DEADBAND_HALF_WIDTH) {
    if (zeroed_turn > 0) {
      turn_out = zeroed_turn - DEADBAND_HALF_WIDTH;
    } else {
      turn_out = zeroed_turn + DEADBAND_HALF_WIDTH;
    }
  }
  int left_out =     power_out + (turn_out / 8);
  int right_out = -1 * power_out + (turn_out / 8);
  
 //System reset logic
  if (getButton(9)) {
    if (reset_counter == 10) {
      drive_left(left_enabled,0);
      drive_right(right_enabled,0);
      wdt_enable(WDTO_15MS);
      while (1);
    }
  }
  else
    reset_counter = 0;
  
   if(getButton(4)){
    leftSet = 2*power_out;
    rightSet = -2*power_out;
    leftPID.SetMode(AUTOMATIC);
    rightPID.SetMode(AUTOMATIC);
    
    //PID outputs directly to motors at a rate of PID_SAMPLE_TIME
    return;
  } else{
    leftOut = 0;
    rightOut = 0;
    leftPID.SetMode(MANUAL);
    rightPID.SetMode(MANUAL);
  }
  //apply turbo mode
  if (getButton(6)) {
    power_constraint = min(abs(power_out * 2), 255 - abs(turn_out));
    if (abs(power_out) > 75) {
      power_out = constrain(power_out * 2, -power_constraint, power_constraint);
      left_out  =  power_out + (turn_out);
      right_out = -1 * power_out + (turn_out);
    } else if (abs(power_out) >  20) {
      left_out  =    power_out * 2 + (turn_out / 4);
      right_out = -1 * power_out * 2 + (turn_out / 4);
    } else {
      left_out  =    power_out + (turn_out);
      right_out = -1 * power_out + (turn_out);
    }
  } else if (getButton(4)) { //precision mode
    
    /*
    if (abs(power_out) > 75) {
      left_out  =    power_out / 2 + (turn_out / 2);
      right_out = -1 * power_out / 2 + (turn_out / 2);
    } else if (abs(power_out) >  20) {
      left_out  =    power_out / 2 + (turn_out / 8);
      right_out = -1 * power_out / 2 + (turn_out / 8);
    } else {
    */
      //left_out  /= 2;
      //right_out /= 2;
    //}
  } else {
    if (abs(power_out) > 75) {
      left_out  =    power_out + (turn_out);
      right_out = -1 * power_out + (turn_out);
    } else if (abs(power_out) >  20) {
      left_out  =    power_out + (turn_out / 4);
      right_out = -1 * power_out + (turn_out / 4);
    }
  }
  // Debug printing
  SerCommDbg.print("L");
  SerCommDbg.print(left_out);
  SerCommDbg.print(" R");
  SerCommDbg.println(right_out);
  // Drive motors!
  drive_left(left_enabled,left_out);
  drive_right(right_enabled,right_out);
}

/* Gripper
  //check for invalid states
  if ((getButton(0) ^ getButton(2))) {
    //both up and down buttons at same time is invalid
    if (getButton(2)) {
      //close gripper
      digitalWrite(GRIP_VALVE,LOW);
    }
    else if (getButton(0)) {
      //open gripper
      digitalWrite(GRIP_VALVE,HIGH);
    }
  }
  // Home arm
/*  if ((homed == 0 || homed == 3) && getButton(8)) {
   homed = 1;
  }
  if (homed == 3) {
    if (getButton(3) ^ getButton(1)) {
      if (getButton(3)) move_arm(1);
      else if (getButton(1)) move_arm(-1);
    }
    else move_arm(0);
  }
  arm_loop();
  */
