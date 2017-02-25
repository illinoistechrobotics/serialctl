CC=gcc
CFLAGS=-c -Wall -Werror -g
LDFLAGS=-lncurses
SOURCES=base64.c crc16.c serio.c main.c joystick.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=serialctl
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(OBJECTS) -o $@ $(SDL_LDFLAGS) $(LDFLAGS) 
.c.o:
	    $(CC) $< -o $@ $(SDL_CFLAGS) $(CFLAGS) 
clean:
	    rm *.o $(EXECUTABLE)
