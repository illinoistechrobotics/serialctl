#include <Arduino.h>
#include <Servo.h>

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

// #include "packet.h"
#include "hw.h"
#include "arm.h"
#include "zserio.h"
#include "globals.h"
#include "PIDUtil.h"
#include "iic_encoder_io.h"
#include "sequencing.h"
#include "serialctl.h"

packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
char left_enabled = 0, right_enabled = 0;
long last_f = 0, last_s = 0, t_start = 0, usec;
uint32_t left_tick_count = 0, right_tick_count = 0;
static uint8_t reset_counter = 0;
static int power_constraint = 0;

/* Interlock to only allow engaging PID while sticks are at zero,
 * 0 indicates PID was not used in the last iteration, 1 indicates the PID was used in the last iteration
 */
static unsigned char pid_interlock=0;

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
  t_start = millis(); //Save the start time
  SerCommDbg.begin(115200);
  DEBUGPRINT(BOOTSTR); 
#ifdef WATCHDOG_
  DEBUGPRINT("Enabling Watchdog timer...");
  wdt_enable(WDTO_250MS);  //Set 250ms WDT
  wdt_reset();             //watchdog timer reset
#endif

  //Initialize safe to safe values!!
  safe.stickX = 127;
  safe.stickY = 127;
  safe.btnhi = 0;
  safe.btnlo = 0;
  safe.cksum = 0b1000000010001011;
  DEBUGPRINT("Initializing I2C communication subsystems...");
  setup_iic();
  DEBUGPRINT("Testing wheel encoder communication...");
  PIDEncoderCheck();
  DEBUGPRINT("Initializing GPIO and PWM pins...");
  init_pins(); //Initialize pins and motor controllers (refer to hw.ino)
  DEBUGPRINT("Initializing drive PID subsystem...");
  PIDInit(); //Initialize PID subsystem
  DEBUGPRINT("Initializing RC communication subsystems...");
  SerComm.begin(57600);
  comm_init(); //Initialize the communication FSM
  //DEBUGPRINT("Initializing sequencing game...");
  //init_sequencing();
  last_f = millis();
  last_s = millis();
  drive_left(0,0);  //Ensure both motors are stopped, technically redundant
  drive_right(0,0); 

  //copy safe values over the current state
  memcpy(astate, &safe, sizeof(packet_t));
  pid_interlock=0;
  
#ifdef WATCHDOG_
  DEBUGPRINT("Disabling watchdog timer...");
  wdt_disable();  //long delay follows
#endif
  //wait at least ten seconds (TC of filters)
  //before measuring offset (variable delay so
  //as not to waste time spent during setup)
  DEBUGPRINT("Waiting for drive current sense filters to stabilize...");
  if ((t_start = millis() - t_start) < 10000) {
    delay(10000 - t_start);
  }
  DEBUGPRINT("Calibrating drive current sensors...");
  measure_offset();
#ifdef WATCHDOG_
  DEBUGPRINT("Enabling Watchdog timer...");
  wdt_enable(WDTO_250MS);  //Set 250ms WDT
  wdt_reset();             //watchdog timer reset
#endif
  DEBUGPRINT("---Initialization complete!---.");
}

void loop() {
  //Main loop runs at full speed
  #ifdef WATCHDOG_
  wdt_reset();
  #endif
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
  // compute PIDs and drive motors if in
  // automatic (precision) mode
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
  //tick_sequencing();

  /*static bool lastSmallRightState = false;
  if (getButton(SMALL_RIGHT) != lastSmallRightState) {
    if (getButton(SMALL_RIGHT))
      toggle_sequencing();
    lastSmallRightState = getButton(SMALL_RIGHT);
  }*/

  if (getButton(DIAMOND_UP)) {
    door_latch.write(0);
  } else {
    door_latch.write(90);
  }

  if (getButton(DIAMOND_DOWN)) {
    digitalWrite(VACUUM_RELAY, LOW);
  } else {
    digitalWrite(VACUUM_RELAY, HIGH);
  }

  /*if (getButton(SMALL_LEFT)) {
    auto_pump_enable = !auto_pump_enable;
  }

  if (!auto_pump_enable && getButton(SHOULDER_BOTTOM_RIGHT)) {
    pumping = 1;
  } else if (!auto_pump_enable) {
    pumping = 0;
  }*/
}
void slow_loop() {
  //2x per second
  //compressor_ctl();
} 

void tank_drive() { // not actually tank drive
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
  // Turn power scaling here
  int left_out =     power_out + (turn_out / 8);
  int right_out = -1 * power_out + (turn_out / 8);
  
 // System reset logic
 // Set fastest watchdog timeout and spin forever
 #ifdef WATCHDOG_
  if (getButton(SMALL_LEFT)) { // reset
    reset_counter++;
    if (reset_counter == 50) {
      DEBUGPRINT("Performing system reset!");
      left_enabled = 0;
      right_enabled = 0;
      // Vital: runaway robot possible if buggy optiboot,
      // as Arduino will not reboot!
      drive_left(left_enabled,0);
      drive_right(right_enabled,0);
      wdt_enable(WDTO_15MS);
      while (1);
    }
  }
  else
    reset_counter = 0;
 #endif
    /* Arm the PID if button 4 is down and either the sticks are currently centered or the PID is already armed */
   if(getButton(SHOULDER_TOP_LEFT)){
    //PID button down
    if(pid_interlock || (power_out == 0 && turn_out == 0)){
      //PID can engage
      // Setpoints are in terms of motor shaft RPM, max 4800RPM (note 8.3:1 speed reducer)
      // 8 is probably OK
      leftSet = -3*(power_out+turn_out);
      rightSet = -3*(power_out-turn_out);
      leftPID.SetMode(AUTOMATIC);
      rightPID.SetMode(AUTOMATIC);
      /* Allow PID to stay engaged */
      pid_interlock=1;
    } else{
      //PID button down, but NOT safe to engage PID
      leftOut = 0;
      rightOut = 0;
      /* Ensure that sticks are centered before allowing the PID to engage again! Risk of mechanical damage!*/
      pid_interlock=0;
      leftPID.SetMode(MANUAL);
      rightPID.SetMode(MANUAL);
      drive_left(left_enabled,0);
      drive_right(right_enabled,0);
    }  
    //PID outputs directly to motors at a rate of PID_SAMPLE_TIME
    return;
  } else {
    //Button 4 is up, NO pid
    leftOut = 0;
    rightOut = 0;
    /* Ensure that sticks are centered before allowing the PID to engage again! Risk of mechanical damage!*/
    pid_interlock=0;
    leftPID.SetMode(MANUAL);
    rightPID.SetMode(MANUAL);
  }
  //apply turbo mode
  if (getButton(SHOULDER_TOP_RIGHT)) {
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
  } else if (!getButton(SHOULDER_TOP_LEFT)) {
    if (abs(power_out) > 75) {
      left_out  =    power_out + (turn_out);
      right_out = -1 * power_out + (turn_out);
    } else if (abs(power_out) >  20) {
      left_out  =    power_out + (turn_out / 4);
      right_out = -1 * power_out + (turn_out / 4);
    }
  }
  #ifdef PRINTMOTORS
  // Debug printing
  SerCommDbg.print("L");
  SerCommDbg.print(left_out);
  SerCommDbg.print(" R");
  SerCommDbg.println(right_out);
  #endif
  // Drive motors!
  drive_left(left_enabled,left_out);
  drive_right(right_enabled,right_out);
}
