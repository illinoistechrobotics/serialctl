CC=gcc
CFLAGS=-c -Wall -Werror -g
LDFLAGS=-lncurses -lcdk
SOURCES=base64.c crc16.c serio.c main.c joystick.c ui.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(SDL_LDFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	    $(CC) $(SDL_CFLAGS) $(CFLAGS) $< -o $@
clean:
	    rm *.o $(EXECUTABLE)
