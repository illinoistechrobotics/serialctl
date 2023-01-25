#Illinois Tech Robotics `serialctl`

`serialctl` is ITR's fourth robot control software. The previous versions are (in chronological order): https://github.com/illinoistechrobotics/ITR-Gumstix-Controller https://github.com/illinoistechrobotics/robots https://github.com/illinoistechrobotics/robots_v2 

It is written in C. `ITR-Gumstix-Controller` is also written in C, while `robots` and `robots_v2` are written in Java.

It is split into three components: PC input, communications, and Arduino robot controller code.

The branches of the repository contain the source needed for each robot ITR runs with this software. For example, the Fenrir branch has the modifications to the Arduino code to interface with the PID speed servos on the robot. Goliath has code for the pneumatics on board.

For ITR's convenience, this repo includes a copy of the Sabertooth library. It is not modified from the original source. 

##PC

The PC code for `serialctl` acts as the interface between the input device (typically a 'PS2 style' game controller), the communications channel, and a display. It has a simple data flow. The code reads the state of the game controller, packs it into a struct, transmits it over the comm channel, and writes the state to the display.

The PC code is currently most sophisticated in the Fenrir and master branch, the display has an ncurses 'GUI' and data logging from the robot.

It is run by: 

`serialctl /path/to/serial joysticknumber` 

##Communications

Communication works over two underlying communications protocols: a USB serial connection or an X-Bee serial link.

Its wire format is a base64 encoded struct, with ints for two axes of joystick(s), and a packed int bitfield representation of the buttons. It also has a CRC.

The format is defined in `packet.h`

The communications code will send the robot into failsafe mode if no valid packets are received for 100 milliseconds.

##Arduino

All the work is done on the Arduino. 

###File by file
####`base64.c`, `base64.h`, `crc16.c`, and `crc16.h`
Libs for base64 and crc. Used for encoding/decoding `packet_t`s.

####`globals.h`
Used to define any global variables or `#define` statements

####`hw.h` and `hw.ino`
The header file defines any hardware specific changes to the code, the .ino implements those functions. For example, Goliath's version has an `osmc_init` function.

####`packet.h`
Defines the wire format. Symlinked to with PC side header of the same name.

####`serialctl.ino`
Where `setup()`, `loop()`, and a very basic scheduler reside

####`zserio.h` and `zserio.ino`
Define and implement the communication interface. The salient feature of the comm interface is the use of two buffers, with a pointer to the current. This pointer always points to the safe buffer, while writing into the other.
