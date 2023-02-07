#include "joystick.h"
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
int minv(int a, int b) {
        return a<b ? a : b;
}
SDL_Joystick *jstick;
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
        ctl->stickX = (SDL_JoystickGetAxis(jstick, 4)/512)+128;
        ctl->stickY = (SDL_JoystickGetAxis(jstick, 1)/512)+128;
        ctl->btnlo = 0;
        ctl->btnhi = 0;
	for(i=0; (i<minv(SDL_JoystickNumButtons(jstick), 12)); i++){ //16 bits available - 4 for dpad
                if(i<8){
                        ctl->btnlo |= (SDL_JoystickGetButton(jstick,i) << i);
                } else if(i>=8 && i<15){
                        ctl->btnhi |= (SDL_JoystickGetButton(jstick,i) << (i-8));
                }
        }
        ctl->btnhi |= (SDL_JoystickGetHat(jstick,0) << 4);
	// fprintf(stderr, "Joystick: %x\n", ctl->btnhi);
	return 0;
}
int joystick_wait_safe(){
        int i, unsafe;
        do{
        	unsafe=0;
        	if(SDL_JoystickGetAttached(jstick) == SDL_FALSE)
                return -1;
         	for(i=0;i<8;i++){
                usleep(1E5);
                SDL_JoystickUpdate();
			}
			if(abs(SDL_JoystickGetAxis(jstick, 1)) > 200){
				unsafe=1;
			}
			if(abs(SDL_JoystickGetAxis(jstick, 4)) > 200){
				unsafe=1;
			}
			for(i=0; (i<minv(SDL_JoystickNumButtons(jstick), 15)); i++){
				if(SDL_JoystickGetButton(jstick,i) != 0){
					unsafe=1;
				}
			}
			printf("Waiting for safe stick position\nunsafe: %d %d", SDL_JoystickGetAxis(jstick, 1), SDL_JoystickGetAxis(jstick, 4));
        }while(unsafe);
        printf("Sticks safe!\n");
        return 0;        

}
void joystick_release(){ 	 
        // Close if opened
        if (SDL_JoystickGetAttached(jstick)) {
                SDL_JoystickClose(jstick);
        }

}
