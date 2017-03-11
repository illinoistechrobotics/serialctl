CC=gcc
CFLAGS=-c -Wall -Werror -g
LDFLAGS=-lncurses
SOURCES=base64.c crc16.c serio.c main.c joystick.c
EXECUTABLEREC=serialctl-record
EXECUTABLEPLAY=serialctl-play
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
all: $(EXECUTABLEREC) $(EXECUTABLEPLAY)
	    
$(EXECUTABLEREC): $(SOURCES) 
	    $(CC) -DRECORD -o $(EXECUTABLEREC) $(SOURCES) $(SDL_CFLAGS) $(SDL_LDFLAGS) $(LDFLAGS) 

$(EXECUTABLEPLAY): $(SOURCES) 
	    $(CC) -DPLAYBACK -o $(EXECUTABLEPLAY) $(SOURCES) $(SDL_CFLAGS) $(SDL_LDFLAGS) $(LDFLAGS) 


clean:
	rm $(EXECUTABLEREC) $(EXECUTABLEPLAY)
