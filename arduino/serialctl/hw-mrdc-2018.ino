#include "hw.h"
#include "globals.h"

int spinwheel_state;
int rope_state;

void manipulator_setup() {
  spinwheel.attach(SPINNER_PIN);
  spinwheel_aux.attach(AUX_PIN);

  spinwheel.writeMicroseconds(OFF_SPEED);
  spinwheel_aux.writeMicroseconds(FORWARD_CMD);

  rope.attach(ROPE_PIN);

  winch.attach(WINCH_PIN);
}

void manipulator_spinwheel() {
  if (get_button(1) && !get_button(2) && spinwheel_state != 1) {
    SerComm.write("Spinning Forward ");
    spinwheel.writeMicroseconds(LAUNCH_SPEED);
    spinwheel_aux.writeMicroseconds(FORWARD_CMD);
    // don't waste processor time if we can help it
    spinwheel_state = 1;
  } else if (get_button(2) && !get_button(1) && spinwheel_state != -1) {
    SerComm.write("Spinning Reversed ");
    spinwheel.writeMicroseconds(LAUNCH_SPEED);
    spinwheel_aux.writeMicroseconds(REVERSE_CMD);
    spinwheel_state = -1;
  } else {
    spinwheel.writeMicroseconds(OFF_SPEED);
    spinwheel_state = 0;
  }
}

void manipulator_rope() {
  if (get_button(0) && rope_state == 0) {
    rope.write(150);
    rope_state = 1;
  } else if (rope_state == 1) {
    rope.write(90);
    rope_state = 0;
  }
}

