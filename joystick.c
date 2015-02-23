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
                jstick = SDL_JoystickOpen(id);

                if (jstick) {
                        printf("Opened Joystick 0\n");
                        printf("Name: %s\n", SDL_JoystickNameForIndex(id));
                        printf("Number of Axes: %d\n", SDL_JoystickNumAxes(jstick));
                        printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(jstick));
                        printf("Number of Balls: %d\n", SDL_JoystickNumBalls(jstick));
                } else {
                        printf("Couldn't open Joystick %i\n",id);
                        return -1;
                }
        }
        return 0;
}
int joystick_update(packet_t *ctl){
        int i;
        if(SDL_JoystickGetAttached(jstick) == SDL_FALSE)
                return -1;
        SDL_JoystickUpdate();
        //populate controller struct
        ctl->stickX = (SDL_JoystickGetAxis(jstick, 1)/256)+128;
        ctl->stickY = (SDL_JoystickGetAxis(jstick, 2)/256)+128;
        ctl->btnlo = 0;
        for(i=0; (i<SDL_JoystickNumButtons(jstick) && i < 7); i++){
                ctl->btnlo |= (SDL_JoystickGetButton(jstick,i) << i);
        }
        return 0;
}
void joystick_release(){ 	 
        // Close if opened
        if (SDL_JoystickGetAttached(jstick)) {
                SDL_JoystickClose(jstick);
        }

}
