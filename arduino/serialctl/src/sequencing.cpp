#include "sequencing.h"
#include "globals.h"
#include "serialctl.h"

bool play_game = false;

void init_sequencing() {
	for (int i = 0; i < BUTTONS; i++) {
		pinMode(light_sensor_pins[i], INPUT);
		pinMode(fire_pins[i], OUTPUT);
	}
}

void start_sequencing() {
	play_game = true;
}

void stop_sequencing() {
	play_game = false;
}

void toggle_sequencing() {
  play_game = !play_game;
}

void tick_sequencing() {
	if (!play_game) {
    #ifdef PRINTMOTORS
    SerCommDbg.print("Sequencing OFF: ");
    for (int i = 0; i < BUTTONS; i++) {
        SerCommDbg.print(i);
        SerCommDbg.print(": ");
        SerCommDbg.print(analogRead(light_sensor_pins[i]));
        SerCommDbg.print("; ");
        SerComm.print(i);
        SerComm.print(": ");
        SerComm.print(analogRead(light_sensor_pins[i]));
        SerComm.print("| ");
    }
    #endif
    if (getButton(DPAD_DOWN)) {
      digitalWrite(fire_pins[0], HIGH);
      SerComm.print("FIRE 0 ");
    } else if (getButton(DPAD_UP)) {
      digitalWrite(fire_pins[1], HIGH);
      SerComm.print("FIRE 1 ");
    } else if (getButton(DPAD_LEFT)) {
      digitalWrite(fire_pins[2], HIGH);
      SerComm.print("FIRE 2 ");
    } else if (getButton(DPAD_RIGHT)) {
      digitalWrite(fire_pins[3], HIGH);
      SerComm.print("FIRE 3 ");
    }
    /*if (getButton(DPAD_DOWN) && getButton(DPAD_LEFT)) {
      digitalWrite(fire_pins[0], HIGH);
    } else if (getButton(DPAD_UP) && getButton(DPAD_LEFT)) {
      digitalWrite(fire_pins[1], HIGH);
    } else if (getButton(DPAD_DOWN) && getButton(DPAD_RIGHT)) {
      digitalWrite(fire_pins[2], HIGH);
    } else if (getButton(DPAD_UP) && getButton(DPAD_RIGHT)) {
      digitalWrite(fire_pins[3], HIGH);
    } */ else {
  		for (int i = 0; i < BUTTONS; i++) { 
  			digitalWrite(fire_pins[i], LOW);
  		}
    }
	} else {
		int fireable_pins = 0;
    #ifdef PRINTMOTORS
    SerCommDbg.print("Sequencing  ON: ");
    #endif
		for (int i = 0; i < BUTTONS; i++) {  
      #ifdef PRINTMOTORS
      SerCommDbg.print(i);
      SerCommDbg.print(": ");
      SerCommDbg.print(analogRead(light_sensor_pins[i]));
      SerCommDbg.print("; ");
      SerComm.print(i);
      SerComm.print(": ");
      SerComm.print(analogRead(light_sensor_pins[i]));
      SerComm.print("| ");
      #endif
			if (analogRead(light_sensor_pins[i]) > DETECT_THRESHOLD) {
				ticks_detected[i]++;
				if (ticks_detected[i] >= LIGHT_SENSOR_TICKS)
					fireable_pins++;
			} else {
				ticks_detected[i] = 0;  
			}
		}
		for (int i = 0; i < BUTTONS; i++) {
			if (fireable_pins == 1 && ticks_detected[i] >= LIGHT_SENSOR_TICKS) {
        #ifdef PRINTMOTORS
        SerCommDbg.print("Sequencing FIRING ");
        SerCommDbg.print(i);
        SerCommDbg.println();
        SerComm.print(" FIRING ");
        SerComm.print(i);
        SerCommDbg.println();
        #endif
        digitalWrite(fire_pins[i], HIGH);
		  } else {
        digitalWrite(fire_pins[i], LOW);
		  }
		}
    #ifdef PRINTMOTORS
    SerCommDbg.println();
    #endif
	}
}
