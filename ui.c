#include <unistd.h>
#include "ui.h"
static int parent_x, parent_y;

static CDKSLIDER * throttle;
static CDKSLIDER * steer;
int old_throttle = 128, old_steer = 128;


static CDKSCREEN *cdk_master;

static CDKLABEL *status_box;
static CDKLABEL *turbo_box;
static CDKLABEL *precision_box;

static CDKLABEL *estop_box;

static CDKLABEL *overflow_notification;
char * overflow_msg[1] = {"</B/2>Input buffer overflow!<!2>"};

char * turbo_on[1] = {"</B/2>TURBO  ON<!2>"};
char * turbo_off[1] = {"</B/C>TURBO OFF<!C>"};

char * precision_on[1] = {"</B/3>PRECISION  ON<!3>"};
char * precision_off[1] = {"</B/C>PRECISION OFF<!C>"};

char * estop_on[1] = {"</B/2>ESTOP  ON<!2>"};
char * estop_off[1] = {"</B/3>ESTOP OFF<!3>"};

boolean estop = false;

int getButton(packet_t * astate, int num){
        if(num<=7){
                return (astate->btnlo >> num) & 0x01;
        } else if(num>7 && num <= 15){
                return (astate->btnhi >> (num - 8)) & 0x01;
        } else {
                return 0;
        }
}

int init_ui(){
  initscr();
  noecho();
  curs_set(FALSE);
  initCDKColor();

  // set up initial windows
  getmaxyx(stdscr, parent_y, parent_x);
  cdk_master = initCDKScreen(stdscr);

  throttle = newCDKSlider(
			  cdk_master,0,0,
			  "Throttle","",
			  A_REVERSE | COLOR_PAIR (29) | ' ',
			  0,
			  128,
			  0,
			  255,
			  1,
			  1,
			  true,
			  false);
  steer = newCDKSlider(
		       cdk_master,0,4,
		       "Steer","",
		       A_REVERSE | COLOR_PAIR (29) | ' ',
		       0,
		       128,
		       0,
		       255,
		       1,
		       1,
		       true,
		       false);
    drawCDKSlider(throttle, true);
    drawCDKSlider(steer, true);

    //This is the jenkest, but I'm using a big string to get the label big enough :)
  char *mesg[1] = {"                                                                                                                                                                                                                                       "};
  status_box = newCDKLabel(cdk_master, 0, parent_y-3, mesg, 1, true, false);
  turbo_box = newCDKLabel(cdk_master, 0, 8, turbo_off, 1, true, false);
  precision_box = newCDKLabel(cdk_master, 20, 8, precision_off, 1, true, false);

  estop_box = newCDKLabel(cdk_master, 0, 16, estop_off, 1, true, false);
  
  overflow_notification = newCDKLabel(cdk_master,
				      parent_x/2 - 11, parent_y/2-1, //11 is half the width
				      overflow_msg,
				      1, true, false);
  return 0;
}

void refresh_ui(packet_t * ctl, char * msg, int overflow){
  //TODO: unbreak window resize
  if(overflow){
    drawCDKLabel(overflow_notification, true);
  } else {
    eraseCDKLabel(overflow_notification);
  }
  char * mesg[1] = {msg};
  setCDKLabelMessage(status_box,mesg, 1);
  if(getButton(ctl,6)){
    setCDKLabelMessage(turbo_box,turbo_on, 1);
  } else {
    setCDKLabelMessage(turbo_box,turbo_off, 1);
  }
  if(getButton(ctl,4)){
    setCDKLabelMessage(precision_box,precision_on, 1);
  } else {
    setCDKLabelMessage(precision_box,precision_off, 1);
  }
  if(getButton(ctl,7)){
    estop = false;
  }
  if(getButton(ctl,5)){
    estop = true;
  }
  if(estop){
    setCDKLabelMessage(estop_box,estop_on, 1);
  } else {
    setCDKLabelMessage(estop_box,estop_off, 1);
  }

  if(ctl->stickX != old_throttle){
    old_throttle = ctl->stickX;
    setCDKSliderValue(throttle, ctl->stickX);
    drawCDKSlider(throttle, true);
  }

  if(ctl->stickY != old_steer){
    old_steer = ctl->stickY;
    setCDKSliderValue(steer, ctl->stickY);
    drawCDKSlider(steer, true);
  }
}
