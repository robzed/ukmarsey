# UKMARSEY Command Language

This is a real-time command language for the UKMARSBOT Robot. 

The commands are terse but intended to be entered either by human via a 
terminal or via machine, e.g. a Pi Zero. It's intention is to allow real-time 
command loops of low level operations, or high-level control for less experienced users.

NOTE: Originally this project was intended as a wall follder for the UKMARSBOT robot, but it only got as far as a hardware test command line via a simple command line interpreter before it was repurposed.

* Github project: https://github.com/robzed/ukmarsey
* Hardware Designed by UK Micromouse and Robotics Society http://ukmars.org
* Hardware details: https://github.com/ukmars/ukmarsbot
* Examples code sketches: https://github.com/ukmars/ukmarsbot-examples

NOTE: This project is related to the Pi Zero for UKMARSbot project https://github.com/robzed/pizero_for_ukmarsbot

This project is licensed under the MIT license.

# Getting Started

Loads into Arduino via serial lead from the Arduino IDE. You can issue commands to get it to do things.

# Summary of Interpereter

NOTICE: This reflects version 1.1 of the interpreter.

## Serial Connection and Command Entry

Baud rate is 115200, 8N1 bit format (8-bits, no parity, and 1-stop bit).

Once connected you should get a prompt like on reset:

    Hello from ukmarsey

No spaces, tabs or other control characters (below value 32 space) are allowed in commands, except for those mentioed below in 'Special Control Characters' below - the use. (Technically UTF-8 characters are ok, but none are currently used in commands).

All serial commands are case sensitive. Each command needs to have a LF (10, 0x0D) at the end of it. 

Values are in decimal, but if the value is out of range then interpreter will either issue an error, ignore extra values or interpret this in an undefined way. This last two options are considered undefined operation - and future changes of the interpreter might change the behaviour. Examples are D11= (no value), D11=2 (out of range setting of an I/O port, might throw an error.), D11=100 (out of range, might ignore 00) or D1=-3 (unexpected minus, probably error).

Any commands that return values is done on a seperate line per command.

## Special Control Characters

Control-X (0x18) - Soft-Reset - same as Control-C, but stops motors. 
Control-C (0x03) - Abort entry of line. NOTICE: Character subsequent to this character will be 

Line Feed (LF, 0x13) - Finish line entry and send to the interpreter. 


## Serial Commands List

Note: 'x' command stops the motors in the case of runaway, but requires a newline afterwards. If you are using a serial terminal emulator (as opposed to Arduino Serial Monitor) you can also use Control-X to stop a runaway.


### Interpreter Control Commands

| Cmd | Action    |
|:---:|-----------|
| ^ | reset state | 
| v | show version |
| V | Verbose error code, 1=verbose, 0=numeric - see below |
| E | Serial echo of input 0=off, 1=on (default). Generally should be turned off for machines |


### Low Level I/O Control Commands

| Cmd | Action    |
|:---:|-----------|
| D | set or read digital pin |
| A | read analogue pin, or write PWM pin |
| M | motor control, using PWM value. 1 = left motor, 0 or 2 = right motor. PWM value -255 to 255, with 0 as stop. |
| N | Dual motor control specifying a battery voltage | 

Examples:

Setting digital output pins requires no response. It is assumed to simply 'work'.

    D6=1            set pin 3 to logic one
    D13=0           set pin 13 to logic zero

Reading digital input pins returns a single character '0' or '1' for the pin state

    D4              might return '0'

Reading an analogue pin returns an integer in the range 0..1023

    A2              might return '76'
    A0              read analogue pin 0

Write to a PWM output. NOTE: PWM use digital I/O numbering! Range 0..255.

    A9=255          write PWM pin 3, with 255.

Output a motor PWM duty cycle from the range 0..1023. (Or 0..99 or whatever range is agreed)

    M1=45           returns nothing
    M1=255          full PWM forwards left motor
    M2=0            PWM = 0, effectively off
    M2=-128         Half PWM backwards, right motor
    M0=128          Right motor

N command also controls the motors, however it's referenced to battery which means we can avoid differnt speeds as the battery discharges - and instead specify voltages rather than PWM (which is in fact a proportion of the current battery voltage). The general format is 'Nf,g' where f and g are signed floats representing voltage levels, negative backwards, positive forward. 

    N3.5,-3.5       Left motor 3.5 volts, right motor backwards 3.5 volts
    N0,0            Stop both motors

The advantage of this command is that it will go the same speed regardless of battery level, assuming you keep values to the range underneath the minimum battery voltage expected. 

### Motor Count commands

Reading an encoder counter might be more involved. It is the total so far and the range is int32 (+/- 2,147m even at 1000 counts per mm!). Result or parameter is signed.

| Cmd | Action    |
|:---:|-----------|
| C0 | Read right wheel counter, also possible to use C2 |
| C1 | Read Left wheel counter. Might return the left wheel counter as '-3752901'. |
| C*n*=*m* | Set the Wheel count value, usually. e.g. C1=0 where n=1 and m=0. |
| z | zero encoders. No return. | 
| e | print encoder current info - human readable NOT for machine parsing! |
| r | print encoder setup - human readable NOT for machine parsing! | 

Setting a counter using 'C' command is usually to zero but could be any legal value. Therefore a quick 'z' command is provided.

### Parameter Commands

Parameters select specific characteristics of high level commands. They are stored in EEPROM so are preserved on power off, especially for human users. 

| Cmd | Action    |
|:---:|-----------|
| $*n* | Read parameter *n* |    
| $*n*=*f* | Write parameter *n* with value *f*. E.g. $0=1.1 |
| $a   | Read all parameters, in the format *f*,*f*,*f* on a single line. NOTE: Will incur serial buffering and block until complete |
| $d   | Default all parameters. |

Currently all parameters are floating point values. 

A List of specific usage of each parameters is here.

| Param | Action    |
|:-----:|-----------|
| 0 | *Undefined* |
| 1 | *Undefined*  |
| 2 | *Undefined*  |
| 3 | *Undefined*  |
| 4 | *Undefined*  |
| 5 | *Undefined*  |
| 6 | *Undefined*  |
| 7 | *Undefined*  |
| 8 | *Undefined*  |
| 9 | *Undefined*  |
| 10 | *Undefined*  |
| 11 | *Undefined*  |
| 12 | *Undefined*  |
| 13 | *Undefined*  |
| 14 | *Undefined*  |
| 15 | *Undefined*  |


### High Level I/O Control
 
(to be added) 

### General Commands

| Cmd | Action    |
|:---:|-----------|
| l |  (lower case L) Led on/off, l0 = led off, l1 = led on. (Short version of D13=x) |
| ? | just prints 'OK' |
| h | just prints 'OK' |
| s | shows the state of the switches. Returns a single number. NOTE: the Button is '16', and overrides the 4 switches | 
| b | shows the voltage of the battery. Example return '7.421' |
| w | shows wall sensor readings - human readable NOT for machine parsing! |
| m | motor tests (see below) | 
| x | Motor stop (no parameters, no return.) - and cancels any actions |


### Test commands

m = motor tests, runs for 2 seconds or until button is pressed.
**NOTE: These are for human testing and are NOT for machine control!**

|Command|Function| Action                              |
|:------:|:------:|-------------------------------------|
|  m0  |    0   | Motors Off                          |
|  m1  |    1   | Forward 25%                         |
|  m2  |    2   | Forward 50%                         |
|  m3  |    3   | Forward 75%                         |
|  m4  |    4   | Reverse 25%                         |
|  m5  |    5   | Reverse 50%                         |
|  m6  |    6   | Reverse 75%                         |
|  m7  |    7   | Spin Left 25%                       |
|  m8  |    8   | Spin Left 50%                       |
|  m9  |    9   | Spin Right 25%                      |
|  ma  |   10   | Spin Right 50%                      |
|  mb  |   11   | Pivot Left 25%                      |
|  mc  |   12   | Pivot right 25%                     |
|  md  |   13   | Curve - Left 25%, Right 50%         |
|  me  |   14   | Curve - Left 50%, Right 25%         |
|  mf  |   15   | Curve - Left 75%, Right 50%         |


## Resetting and getting the Pi in sync with the Arduino.

Since the longest command is 6 bytes, send ^ repeatidly with a 20ms gap until you receive a RST message. Then try ? and v looking at the responses. If they don't succeeed, then repeat the entire sequencies. 

This is under review. 


## Interpreter errors

Interpreter error text strings are not defined to be stable across versions, but these error codes used when verbose is off, will be as far as possible.
 
```
enum
{
  T_OK = 0,
  T_OUT_OF_RANGE = 1,
  T_READ_NOT_SUPPORTED = 2,
  T_LINE_TOO_LONG = 3,
  T_UNKNOWN_COMMAND = 4,
  T_UNEXPECTED_TOKEN = 5
};
```

## Other messages
Apart from the start up message, any other warning messages have a @ before them.
For example:

    @Defaulting Params         ; Shown when there was a problem loading parameters on boot.


## Other information

floating_decimal_places = 3     - Number of floating point decimal places printed.


# Where to find more information

https://www.arduino.cc/reference/en/

http://www.micromouseonline.com/micromouse-book/

# Credits

Based on ukmarsbot examples by UK Micromouse & Robotics Society & Peter Harrison
https://github.com/ukmars/

Some code based on:
https://github.com/micromouseonline/ukmarsbot-line-follower-basic


# History and Other Notes

It bares some resemblance in some places to G-code (and implementations like grbl), but does not attempt to maintain any G-code compatability, since really G-code is targetted at CNC not really at self-moving small robots.

Since this runs on an ATMEG328P Arduino Nano, there is not much Flash or RAM, so things like help and extended error messages have been minimised. 

We aim to have shortened the amount of bytes requiring to be transmitted in order to maximise the bandwidth across the serial link, while still allowing for human entry via a text terminal.


# Dev Notes

Currently this code has a wall sensor reading in the background. This needs to be changed to be more general! Some generic illumination/reading system should be added.

