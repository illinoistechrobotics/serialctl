CC=gcc
CFLAGS=-c -Wall -Werror -g
#LDFLAGS=-static
SOURCES=base64.c crc16.c serio.c main.c joystick.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=serialctl
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
#SDL_LDFLAGS += -static
all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(OBJECTS) -o $@ $(SDL_LDFLAGS) $(LDFLAGS) 
.c.o:
	    $(CC) $< -o $@ $(SDL_CFLAGS) $(CFLAGS) 
clean:
	    rm *.o $(EXECUTABLE)

# Compiling for Windows
# Install Cygwin https://www.cygwin.com/setup-x86_64.exe
# Add packages when prompted: libSDL2-devel libSDL2_2.0_0 make git gcc-core
# Use latest stable package versions.

# Open Cygwin64 Terminal
# git clone https://github.com/illinoistechrobotics/serialctl -b goliath2023
# cd serialctl
# make windows
# the folder `release` now contains a portable windows build with the necessary DLLs. Run from commandline like on Linux
# note for the COM port, if it is e.g. COM5 in device manager, which translates to /dev/com5 in Cygwin. So an example commmand to run is `serialctl.exe /dev/com5 0`

windows: $(EXECUTABLE)
	@echo "Creating release directory..."
	mkdir -p release
	@echo "Copying executable to release folder..."
	cp "$(EXECUTABLE).exe" release/
	@echo "Copying required DLLs to release folder..."
	@echo "Ignore errors from Adoptium, those files are unnecessary anyway"
	@for dll in $$(cygcheck ./$(EXECUTABLE).exe | grep '.dll' | grep -v '\\WINDOWS\\system32\\'); do \
		echo "$$dll"; \
		cp "$$dll" release/; \
	done
	@echo "Built serialctl & DLLs in ./release/"
