#include "joystick.h"
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

int joystick_init(int id){
SDL_Init(SDL_INIT_JOYSTICK);
// Initialize the joystick subsystem
signal(SIGINT,SIG_DFL);
// Check for joystick
if (SDL_NumJoysticks() > id) {
    // Open joystick
    joy = SDL_JoystickOpen(id);

    if (joy) {
        printf("Opened Joystick 0\n");
        printf("Name: %s\n", SDL_JoystickNameForIndex(id));
        printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
        printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
        printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
    } else {
        printf("Couldn't open Joystick %i\n",id);
		return -1;
    }
}
return 0;
}
//while(loop&&SDL_JoystickGetAttached(joy)){
//SDL_JoystickUpdate();
//packet.x = SDL_JoystickGetAxis(joy, 1)/256;
//packet.y = SDL_JoystickGetAxis(joy, 2)/256;

//printf("X=%d, Y=%d\n",packet.x,packet.y);
//}
void joystick_release(){ 	 
    // Close if opened
    if (SDL_JoystickGetAttached(joy)) {
        SDL_JoystickClose(joy);
    }

}
