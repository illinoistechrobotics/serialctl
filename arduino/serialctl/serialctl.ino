#include <Servo.h>
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
#include "arm.h"
#include "zserio.h"
#include "globals.h"
#include "PIDUtil.h"
#include "iic_encoder_io.h"
#include "MPU6050.h"

packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
char enabled_0 = 0, enabled_120 = 0, enabled_240 = 0;
long last_f = 0, last_s = 0, t_start = 0, usec;
Sabertooth ST12(128, SABERTOOTH12);
Sabertooth ST34(129, SABERTOOTH12);
static uint8_t reset_counter = 0;
static int power_constraint = 0;
int angle = 0;
boolean small_right_flag = false;

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
  safe.leftStickX = 127;
  safe.leftStickY = 127;
  safe.rightStickX = 127;
  safe.rightStickY = 127;
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
  DEBUGPRINT("Initializing extension arm system...");
  arm_setup();
  last_f = millis();
  last_s = millis();
  drive_0(0,0);  //Ensure all motors are stopped, technically redundant
  drive_120(0,0); 
  drive_240(0,0);

  MPUInit();

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
  enabled_0 = try_enable_0(enabled_0);
  enabled_120 = try_enable_120(enabled_120);
  enabled_240 = try_enable_240(enabled_240);
  //Fast loop
  if (millis() - last_f >= 40) {
    //Every line sent to the computer gets us a new state
    angle = MPULoop();
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

  //Gripper
  //check for invalid states
  if ((getButton(DIAMOND_LEFT) ^ getButton(DIAMOND_RIGHT))) {
    //both left and right buttons at same time is invalid
    if (getButton(DIAMOND_LEFT)) {
      //close gripper
      digitalWrite(GRIP_VALVE,HIGH);
    }
    else if (getButton(DIAMOND_RIGHT)) {
      //open gripper
      digitalWrite(GRIP_VALVE,LOW);
    }
  }
  //END gripper

  // Home arm
  if ((homed == 0 || homed == 3) && getButton(SMALL_LEFT)) {
    homed = 1;
  }
  // END arm homing

  //Arm extension
  if (homed == 3) {
    if (getButton(DIAMOND_UP) ^ getButton(DIAMOND_DOWN)) {
      if (getButton(DIAMOND_UP)) move_arm(1, getButton(4));
      else if (getButton(DIAMOND_DOWN)) move_arm(-1,getButton(4));
    }
    else move_arm(0,0);
  }
  arm_loop();
  //END arm extension

  //Main arm linear actuator control
  if (getButton(DPAD_UP) ^ getButton(DPAD_DOWN)) {
    if (getButton(DPAD_UP)) {
      //Up
      if(getButton(4)){
        //Precision
        setMotor(UPPER_ARM_MOTOR, -LINAC_PRECISION);
      } else {
        setMotor(UPPER_ARM_MOTOR, -127);
      }
    }
    else if (getButton(DPAD_DOWN)) {
      //DOWN
      if(getButton(4)){
        //Precision
        setMotor(UPPER_ARM_MOTOR, LINAC_PRECISION);
      } else {
        setMotor(UPPER_ARM_MOTOR, 127);
      }
     }
  }
  else {
    setMotor(UPPER_ARM_MOTOR, 0);
  }
  
} 
void slow_loop() {    
  //2x per second
  //Compressor
  compressor_ctl();
} 

void tank_drive() { // not actually tank drive
  int driftX_out = 0;
  int driftY_out  = 0;
  int rotation_out = 0;
  int power_out = 0;
  int zeroed_driftX = ((int)(astate->leftStickX) - 127);
  int zeroed_driftY = ((int)(astate->leftStickY) - 127);
  int zeroed_rotation = ((int)(astate->rightStickX) - 127);
  int zeroed_power = ((int)(astate->rightStickY) - 127);

  if (abs(zeroed_driftX) > DEADBAND_HALF_WIDTH) {
    if (zeroed_driftX > 0) {
      driftX_out = zeroed_driftX - DEADBAND_HALF_WIDTH;
    } else {
      driftX_out = zeroed_driftX + DEADBAND_HALF_WIDTH;
    }
  }
  if (abs(zeroed_driftY) > DEADBAND_HALF_WIDTH) {
    if (zeroed_driftY > 0) {
      driftY_out = zeroed_driftY - DEADBAND_HALF_WIDTH;
    } else {
      driftY_out = zeroed_driftY + DEADBAND_HALF_WIDTH;
    }
  }
  if (abs(zeroed_rotation) > DEADBAND_HALF_WIDTH) {
    if (zeroed_rotation > 0) {
      rotation_out = zeroed_rotation - DEADBAND_HALF_WIDTH;
    } else {
      rotation_out = zeroed_rotation + DEADBAND_HALF_WIDTH;
    }
  }
  if (abs(zeroed_power) > DEADBAND_HALF_WIDTH) {
    if (zeroed_power > 0) {
      power_out = zeroed_power - DEADBAND_HALF_WIDTH;
    } else {
      power_out = zeroed_power + DEADBAND_HALF_WIDTH;
    }
  }

  // Polar Stick Positions
  int leftHyp = sqrt((driftX_out^2)+(driftY_out^2));
  int rightHyp = sqrt((rotation_out^2)+(power_out^2));
  int leftAng = atan2(driftY_out,driftX_out);
  int rightAng = atan2(power_out,rotation_out);
  
  int out_0 = power_out + (leftHyp / 8);
  int out_120 = power_out + (leftHyp / 8);
  int out_240 = power_out + (leftHyp / 8);
  
 //System reset logic
 #ifdef WATCHDOG_
  if (getButton(SMALL_LEFT)) { // reset
    reset_counter++;
    if (reset_counter == 50) {
      DEBUGPRINT("Performing system reset!");
      enabled_0 = 0;
      enabled_120 = 0;
      enabled_240 = 0;
      drive_0(enabled_0,0);
      drive_120(enabled_120,0);
      drive_240(enabled_240,0);
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
    if(pid_interlock || (leftHyp == 0 && rightHyp == 0)){
      //PID can engage
      Set0 = -2*(power_out+leftHyp);
      Set120 = -2*(power_out+leftHyp);
      Set240 = -2*(power_out+leftHyp);
      PID0.SetMode(AUTOMATIC);
      PID120.SetMode(AUTOMATIC);
      PID240.SetMode(AUTOMATIC);
      /* Allow PID to stay engaged */
      pid_interlock=1;
    } else{
      //PID button down, but NOT safe to engage PID
      Out0 = 0;
      Out120 = 0;
      Out240 = 0;
      /* Ensure that sticks are centered before allowing the PID to engage again! Risk of mechanical damage!*/
      pid_interlock=0;
      PID0.SetMode(MANUAL);
      PID120.SetMode(MANUAL);
      PID240.SetMode(MANUAL);
      drive_0(enabled_0,0);
      drive_120(enabled_120,0);
      drive_240(enabled_240,0);
    }  
    //PID outputs directly to motors at a rate of PID_SAMPLE_TIME
    return;
  } else {
    //Button 4 is up, NO pid
    Out0 = 0;
    Out120 = 0;
    Out240 = 0;
    /* Ensure that sticks are centered before allowing the PID to engage again! Risk of mechanical damage!*/
    pid_interlock=0;
    PID0.SetMode(MANUAL);
    PID120.SetMode(MANUAL);
    PID240.SetMode(MANUAL);
  }
  //apply turbo mode
  if (getButton(SHOULDER_TOP_RIGHT)) {
    power_constraint = min(abs(power_out * 2), 255 - abs(leftHyp));
    if (abs(leftHyp) > 75) {
      power_out = constrain(leftHyp * 2, -power_constraint, power_constraint);
      out_0  =  power_out + (leftHyp);
      out_120 = power_out + (leftHyp);
      out_240 = power_out + (leftHyp);
    } else if (abs(leftHyp) >  20) {
      out_0  =  power_out * 2 + (leftHyp / 4);
      out_120 = power_out * 2 + (leftHyp / 4);
      out_240 = power_out * 2 + (leftHyp / 4);
    } else {
      out_0  =  power_out + (leftHyp);
      out_120 = power_out + (leftHyp);
      out_240 = power_out + (leftHyp);
    }
  } else if (!getButton(SHOULDER_TOP_LEFT)) {
    if (abs(leftHyp) > 75) {
      out_0  =  power_out + (leftHyp);
      out_120 = power_out + (leftHyp);
      out_240 = power_out + (leftHyp);
    } else if (abs(leftHyp) >  20) {
      out_0  =  power_out + (leftHyp / 4);
      out_120 = power_out + (leftHyp / 4);
      out_240 = power_out + (leftHyp / 4);
    }
  }
  #ifdef PRINTMOTORS
  // Debug printing
  SerCommDbg.print("Zero");
  SerCommDbg.print(out_0);
  SerCommDbg.print(" 120");
  SerCommDbg.println(out_120);
  SerCommDbg.print(" 240");
  SerCommDbg.print(out_240);
  #endif
  // Drive motors!
  if (getButton(SMALL_RIGHT)) {
    small_right_flag = !small_right_flag;
  }
  if (small_right_flag) {
    if ((angle == 360 || angle > 0) && angle <= 45) {
      drive_0(0,0);
      drive_120(enabled_120,out_120);
      drive_240(enabled_240,out_240);  
    }
    if (angle > 45 && angle <= 90) {
      drive_0(enabled_0,out_0);
      drive_120(0,0);
      drive_240(enabled_240,out_240);  
    }
    if (angle > 90 && angle <= 135) {
      drive_0(enabled_0,out_0);
      drive_120(0,0);
      drive_240(enabled_240,out_240);  
    }
    if (angle > 135 && angle <= 180) {
      drive_0(enabled_0,out_0);
      drive_120(0,0);
      drive_240(0,0);  
    }
    if (angle > 180 && angle <= 225) {
      drive_0(enabled_0,out_0);
      drive_120(0,0);
      drive_240(0,0);  
    }
    if (angle > 225 && angle <= 270) {
      drive_0(enabled_0,out_0);
      drive_120(enabled_120,out_120);
      drive_240(0,0);  
    }
    if (angle > 270 && angle <= 315) {
      drive_0(enabled_0,out_0);
      drive_120(enabled_120,out_120);
      drive_240(0,0);  
    }
    if (angle > 315 && (angle <= 360 || angle == 0)) {
      drive_0(0,0);
      drive_120(enabled_120,out_120);
      drive_240(enabled_240,out_240);  
    }
  }
  else {
    drive_0(enabled_0,out_0);
    drive_120(enabled_120,out_120);
    drive_240(enabled_240,out_240);
  }
}


