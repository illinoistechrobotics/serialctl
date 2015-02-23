CC=gcc
CFLAGS=-c -Wall -Werror -g
LDFLAGS=
SOURCES=base64.c crc16.c serio.c main.c
OBJECTS=$(SOURCES:.c=.o)
	EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	    $(CC) $(CFLAGS) $< -o $@
clean:
	    rm *o $(EXECUTABLE)
