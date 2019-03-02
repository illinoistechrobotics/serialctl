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

void tick_sequencing() {
	if (!play_game) {
		for (int i = 0; i < BUTTONS; i++) {
			digitalWrite(fire_pins[i], LOW);
		}
	} else {
		int fireable_pins = 0;

		for (int i = 0; i < BUTTONS; i++) {
			if (analogRead(light_sensor_pins[i]) > DETECT_THRESHOLD) {
				ticks_detected[i]++;
				if (ticks_detected[i] >= LIGHT_SENSOR_TICKS)
					fireable_pins++;
			} else {
				ticks_detected[i] = 0;
			}
		}
		
		for (int i = 0; i < BUTTONS; i++) {
			if (fireable_pins == 1 && ticks_detected[i] >= LIGHT_SENSOR_TICKS)
				digitalWrite(fire_pins[i], HIGH);
			else
				digitalWrite(fire_pins[i], LOW);
		}
	}
}
