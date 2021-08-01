# UKMARSEY Command Language

This is a real-time command language for low level I/O micros of mobile robots, and first with the UKMARSBOT Robot.

The commands are terse but intended to be entered either by human via a
terminal or via machine, e.g. a Pi Zero. It's intention is to allow real-time
command loops of low level operations, or high-level control for less experienced users.

NOTE: Originally this project was intended as a wall follower for the UKMARSBOT robot, but it only got as far as a hardware test command line via a simple command line interpreter before it was repurposed.

* Github project: https://github.com/robzed/ukmarsey
* Hardware Designed by UK Micromouse and Robotics Society http://ukmars.org
* Hardware details: https://github.com/ukmars/ukmarsbot
* Examples code sketches: https://github.com/ukmars/ukmarsbot-examples

NOTE: This project is related to the Pi Zero for UKMARSbot project https://github.com/robzed/pizero_for_ukmarsbot

This project is licensed under the MIT license.

# Getting Started

Loads into Arduino via serial lead from the Arduino IDE. You can issue commands to get the robot to do things.

As well as directly commanding the robot over the serial port, it's designed to be controlled from a host CPU which can be anything
but we've been using it with the Raspberry Pi.

There is some more information about development environments in [Code Development](Documentation/code_development.md).

# Robot Configuration

Even though the UKMarsBot is a standard design, there are several options:

1. The gearbox ratio
2. The wiring of left and right motors
3. The wiring of the encoders
4. The wheel size.

These can be configured in the file 'robot_config.h'. See that file and make appropriate adjustments.

(If you are using low level control from a Raspberry Pi, you might be able to adjust for these in the Raspberry Pi. But if you are using any mid- or high-level control, you will definitely need§ to adjust this file!)


# Summary of Interpreter Usage

NOTICE: This document reflects version 1.6 of the interpreter.

## Serial Connection and Command Entry

Baud rate is 115200, 8N1 bit format (8-bits, no parity, and 1-stop bit).

Once connected you should get a prompt like on reset:

    Hello from ukmarsey

No spaces, tabs or other control characters (below value 32) are allowed in commands, except for those mentioned below in 'Special Control Characters' below, and their use is undefined and may change in future versions of the interpreter. (Technically UTF-8 characters are ok, but none are currently used in commands).

All serial commands are case sensitive. Each command needs to have a LF (10, 0x0D) at the end of it.

Values are in decimal, but if the value is out of range then interpreter will either issue an error, ignore extra values or interpret this in an undefined way. This last two options are considered undefined operation - and future changes of the interpreter might change the behaviour. Examples are D11= (no value), D11=2 (out of range setting of an I/O port, might throw an error.), D11=100 (out of range, might ignore 00) or D1=-3 (unexpected minus, probably error).

Any commands that return values is done on a seperate line per command. The ends of these lines returned from the interpreter contain a CR LF (0x0D 0x0A).

## Special Control Characters

* Control-X (0x18) - Soft-Reset - same as Control-C, but stops motors, and any active commands. (Same as 'x' command).
* Control-C (0x03) - Abort entry of line. NOTICE: Character subsequent to this character will be treated as the start of a new line.
* Line Feed (LF, 0x10) - Finish line entry and send to the interpreter.
* Carriage Return (CR, 0x13) - Ignored by interpreter.
* Backspace (0x08) - Removes one character from input buffer, assuming input buffer has any characters in. Generates "\x08 \x08" which should step back, erase, then step back. However on some serial terminal emulators this might need to be enabled (e.g. on CoolTerm this option is 'Handle BS and Del Characters').

## Serial Commands List

Note: 'x' command stops the motors in the case of runaway, but requires a newline (LF/0x10) afterwards. If you are using a serial terminal emulator (as opposed to Arduino Serial Monitor) you can also use Control-X to stop a runaway.


### Interpreter Control Commands

| Cmd | Action    |
|:---:|-----------|
| ^ | reset state - writes RST in reply. |
| ^^ | processor reset |
| v | show version |
| V | Verbose error code, 2=extra verbose (default), 1=verbose, 0=numeric - see 'Interpreter errors'.  |
| E | Serial echo of input 0=off, 1=on (default). Generally should be turned off for machines |


Examples:

    V0      Verbose off (numeric error codes)
    V1      Verbose on (text error codes)
    V2      Like V1 except even successful commands reply with OK
    E1      Echo input back (for humans with serial terminals)
    E0      Don't echo input back (for machines, or for use with Arduino Serial monitor)

### High level control commands

| Cmd | Params| Action    |
|:---:|--|---------|
| c1 |  | enable the motor controllers |
| c0 | | disable the motor controllers |
| cz | | reset the motor controllers |
|  |   |  |
|  |   | POSITION/SPEED MOVE| 
| p | pd,t,f,a | start positon profile d=distance,t=topSpeed,f=finalSpeed,a=acceleration. Distance is in mm, speeds are in mm/s, acceleration in mm/s/s |
| p? | | has the position profile finished? |
| pz | | reset the position profile |
|    | |   |
|    | | ROTATION MOVE |
| R | Rd,t,f,a | start rotation profile d=distance,t=topSpeed,f=finalSpeed,a=acceleration. Distance is in degrees, speeds are in degrees/second, acceleration in degrees/sec/sec |
| R?  | | has the rotation profile finished? |
| Rz | | reset the rotation profile |
|    | |   |
|    | | TRACKING |
| T  | Tn | n = tracking/steering adjustment, 0=no adjustment. Used to steer away with walls with a PD controller. This is output of that controller. Applied every cycle until changed. | 


NOTE: Using these command allows mid-level control of the Robot.

Combining position with rotation gives smooth curves. Rotation alone will be in-place. If final velocity is not zero robot keeps moving.

Examples:

    p100,100,50,400    move 100mm, at 300mm/s finally slowing to 50mm/s with a maximum acceleration of 400mm/s^2
    T1.02              Add error to tracking/steering.




### Low Level I/O Control Commands

| Cmd | Params     | Action    |
|:---:|------------|-----------|
|  D  | Dp or Dp=n | Set or read digital pin |
|  A  | Aa or Ap=n | Read analogue pin, or write PWM pin |
|  M  | Mm=p       | Motor control, using PWM value. 1 = left motor, 0 or 2 = right motor. PWM value -255 to 255, with 0 as stop. |
|  N  | Nm,n       | Dual motor control specifying a battery voltage |
|  P  | Pp=d       | PinMode - Set up GPIO pins; p is pin, d is I(input) or O(output) or U(Pull Up Input)  |

Regarding analogue readings, (A command), these are read off interrupts, so can be up to 2ms old.

M - PWM values are low level control of the motor input. 255 represents full voltage - whatever level that is.

N - Set the voltage to the motor. Voltage levels are generated via PWM, after scaling for the battery level. This gives a more consistent result than raw PWM values, because it doesn't result in variation due to battery level.


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

Output a motor PWM duty cycle from the range 0..255.

    M1=45           returns nothing
    M1=255          full PWM forwards left motor
    M2=0            PWM = 0, effectively off
    M2=-128         Half PWM backwards, right motor
    M0=128          Right motor

N command also controls the motors, however it's referenced to battery which means we can avoid differnt speeds as the battery discharges - and instead specify voltages rather than PWM (which is in fact a proportion of the current battery voltage). The general format is 'Nf,g' where f and g are signed floats representing voltage levels, negative backwards, positive forward.

    N3.5,-3.5       Left motor 3.5 volts, right motor backwards 3.5 volts
    N0,0            Stop both motors

The advantage of this command is that it will go the same speed regardless of battery level, assuming you keep values to the range underneath the minimum battery voltage expected.

The Pin mode commands allows the user to configure pins.

    P6=O            Configure right LED on wall follower board to Output
    P11=O           Configure left LED on wall follower board to Output

You can how turn the left and right LEDs on and off:

    D11=1
    D11=0
    D6=0
    D6=1

To switch them back to inputs:

    P6=I            Stop driving the right LED
    P11=I           Stop driving the led LED


### Motor Count commands

Reading an encoder counter might be more involved. It is the total so far and the range is int32 (+/- 2,147m even at 1000 counts per mm!). Result or parameter is signed.

| Cmd | Action    |
|:---:|-----------|
| C0 | Read right wheel counter, also possible to use C2 |
| C1 | Read Left wheel counter. Might return the left wheel counter as '-3752901'. |
| C  | Alone with no parameters - prints both encoders as 'left,right' |
| Cz | Same as C, but also zeros encoders immediately after reading. See warning. |
| Ch | Same as C, but values in Hex |
| ChZ| Same as Ch, but also zeros encoders immediately after reading. See warning. |
| z | zero wheel encoder counters. No return. See Warning.|
| ea | print wheel current info (all) - Format 'encoder-sum,distance,encoder-difference,angle' |
| e  | Old command for 'ea' command, still supported for backward compatability |
| er | print wheel current info (raw format) - Format 'encoder-sum,encoder-difference'|
| eu | print wheel current info (unit format) - Format 'distance,angle' where distance is mm, angle is degrees |
| es | print speed (mm/s) and rotation speed (degrees/s). Note: instant estimates only - use measurements over longer periods for better results. |
| r | print encoder setup - Format 'mm-per-count,degrees-per-count' |

Warning: Zeroing encoders while speed/rotation control commands are working is a bad idea and will likely lead to unexpected operation.
 
NOTE: 'r' command allows decoding 'er' into distances/angles on the host, rather than taking time to print floating values.


Examples:
    C
        98514,98181
    C
        -5075,129406

    Cz
        8487,-8186
    Cz
        0,0

    Ch
        5C65,FFFFA6D8
    Chz
        B507,FFFF50CE
    Ch
        21E4,FFFFDEBE


### Sensor Processing commands

It's sometimes necessary, with Infra-red sensors, to read the IR sensor 'dark', turn on IR illumination, take another IR sensor reading ('light') then turn the IR illumination off.

These commands provide the ability to specify up to three output ports for IR illumination, and read up to size sensors.

By default this is set up to read A0, A1, A2, A3 as sensors and use D12 to turn on the IR illumination with a logic 1 , which will work for the standard line and wall followers of the UKMARSBot (although the wall follower doesn't use A3).

| Cmd | Action    |
|:---:|-----------|
|  S  | Read sensors - gives difference between dark and light - which is what you normal what you need |
|  Sr | Read sensors 'raw' format - dark values then light values seperated by commas. |
|  Sh | As per S, but in hex from 0-FF without commas. You can get more data samples per second in this format. |
|  *  | Enable/Disable emitter LED Control. Used to save power. *0 and *1 |

NOTE: Sh values are divided by 4 (lose bottom 2 bits), capped at 255 (FF). The bottom bits are generally noise anyway. Use this if the transfer time is more important than resolution.

Examples of output of 'S':

    S
        398,104,19,6

Examples of output of 'Sh':

    Sh
    FF691306
    Sh
    FF681307

Examples of output of 'Sr':

    0,0,0,142,28,32,486,352         <- wall close

    0,0,0,4,28,13,15,11            <- no wall


Examples of Emitter control:

    *1
    Sr
        0,0,0,131,135,25,440,318
    *0
    Sr
        0,0,0,1,0,0,0,1

### Parameter Commands

Parameters select specific characteristics of high level commands. They are stored in EEPROM so are preserved on power off, especially for human users. Parameters hold values such as the current tuning constants for controllers.

There are three sets of parameters available stored in different locations:

 * FLASH - the defaults hard coded into the firmware when it was build. They cannot be changed by the code.
 * RAM - the current working versions. These are the settings used by the robot when running and will be lost after a reset.
 * EEPROM - Held in non-volatile memory, these will survive reset and will normally be read into the RAM settings at reset. Users must manually store settings to EEPROM if they are to be retained over a reset.

| Cmd | Action    |
|:---|-----------|
| $*n*  | Read parameter *n* |
| $*n*=*f* | Write parameter *n* with value *f*. E.g. $0=1.1 |
| $$   | display values of all settings in RAM|
| $?   | Display a detailed list of the working settings as a C declaration |
| $@   | Load all saved settings from EEPROM |
| $!   | Store current working settings to EEPROM |
| $#   | Restore defaults hard-coded in firmware |

#### List of parameters

     0 ACTION(int, revision, SETTINGS_REVISION)                \ used for tracking settings revision
     1 ACTION(uint16_t, flags,          0                    ) \ not used yet
     2 ACTION(float, fwdKP ,            FWD_KP               ) \ used by position controller
     3 ACTION(float, fwdKD ,            FWD_KD               ) \ used by position controller
     4 ACTION(float, rotKP ,            ROT_KP               ) \ used by rotation controller
     5 ACTION(float, rotKD ,            ROT_KD               ) \ used by rotation controller
     6 ACTION(float, steering_KP,       STEERING_KP          ) \ not used set
     7 ACTION(float, steering_KD,       STEERING_KD          ) \ not used yet
     8 ACTION(float, mouseRadius,       MOUSE_RADIUS         ) \ not used yet
     9 ACTION(int,   left_calibration,  LEFT_CALIBRATION     ) \ not used yet
    10 ACTION(int,   front_calibration, FRONT_CALIBRATION    ) \ not used yet
    11 ACTION(int,   right_calibration, RIGHT_CALIBRATION    ) \ not used yet
    12 ACTION(float, left_adjust,       LEFT_SCALE           ) \ not used yet
    13 ACTION(float, front_adjust,      FRONT_SCALE          ) \ not used yet
    14 ACTION(float, right_adjust,      RIGHT_SCALE          ) \ not used yet
    15 ACTION(int,   left_threshold,    LEFT_THRESHOLD       ) \ not used yet
    16 ACTION(int,   front_threshold,   FRONT_THRESHOLD      ) \ not used yet
    17 ACTION(int,   right_threshold,   RIGHT_THRESHOLD      ) \ not used yet
    18 ACTION(int,   left_nominal,      LEFT_NOMINAL         ) \ not used yet
    19 ACTION(int,   front_nominal,     FRONT_NOMINAL        ) \ not used yet
    20 ACTION(int,   right_nominal,     RIGHT_NOMINAL        ) \ not used yet

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
| bi | Shows the voltage of the battery in millivolts. Example: '7421' |
| bh | Shows the voltage of the battery in millivolts in hex format |
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

Send Control-C (or Control-X) followed by ^ repeatedly with a 20ms gap until you receive a RST message. Then try ? and v looking at the responses. If they don't succeeed, then repeat the entire sequence.

This is under review.


## Interpreter Errors

Interpreter error text strings (verbose on) are not defined to be stable across versions. Numeric error codes (verbose off) will be stable across releases as far as possible.

NOTE: All error messages (both verbose text and numeric are preceeded by '@Error:' as the first characters on the line, except for verbose error for OK, which appears simply as 'OK' on the line.

OK was left for compatbility with older versions (and it sort of makes sense, since it's not strictly an error as such). This message is only generated by specific commands - generally commands are silent when they succeed. OK can be considered command output, although it will change if verbose is off to an Error style code ('@Error:0').

NOTE: This makes Error codes a special case of unsolicited return messages - see below.

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

### Examples:

    OK
    @Error:0
    @Error:1
    @Error:5
    @Error:Out of range

## Other messages - Unsolicited return messages
Apart from the start up message, any other messages **that are not caused by a command** have a @ before them and are on a seperate line.

For example:

|Message| Cause                               |
|-------|-------------------------------------|
| @Defaulting Params | Shown when there was a problem loading parameters on boot. |

NOTE: Interpreter Error codes also have this format ('@Error:') - see Interpreter Errors.

## Other internal settings and hidden information

| Setting | Cause                               |
|---------|-------------------------------------|
|floating_decimal_places = 3 | Number of floating point decimal places printed. |


# Where to find more information

https://www.arduino.cc/reference/en/

http://www.micromouseonline.com/micromouse-book/

# Credits

This interpreter was written by Rob Probin, Jan 2021, with help and code from Peter Harrison, with many suggestions and input from UK Micromouse and Robotics Society members.

Code and this README Copyright (c) 2021 Rob Probin, except where noted in code or documentation. All licensed via MIT License. See LICENSE file.

Based on ukmarsbot examples by UK Micromouse & Robotics Society & Peter Harrison
https://github.com/ukmars/

Some code based on:
https://github.com/micromouseonline/ukmarsbot-line-follower-basic


# History and Other Notes

It bares some resemblance in some places to G-code (and implementations like grbl), but does not attempt to maintain any G-code compatability, since really G-code is targetted at CNC not really at self-moving small robots.

Since this runs on an ATMEG328P Arduino Nano, there is not much Flash or RAM, so things like help and extended error messages have been minimised.

We aim to have shortened the amount of bytes requiring to be transmitted in order to maximise the bandwidth across the serial link, while still allowing for easy human entry via a text terminal.


# Dev Notes

## ADC
There are three subsystems running all the time currently on the system tick interrupt that runs every 2ms: battery reading, function switch reading and update sensors control. The analogue command (e.g. A0) uses these subsystems to read the ADCs (partially to avoid conflicts since there is only one ADC unit).

## Serial Buffering

The Arduino Nano has a 64 byte input buffer and a 64 byte output buffer. Transmission to and from the Arduino needs to be carefully designed not to overrun these buffers.

If you overrun the 64 byte output buffer the print commands will start waiting inside the Arduino Nano (the commands will take longer to complete). The could cause several affects - once of which could potentially be commands stacking up in the input buffer. It only requires, for instance perhaps three 'Sr' commands to cause the output buffer to be filled.

If you overrun the 64 byte input buffer, then old characters will be dropped. This is bad for several reasons - one of which includes partial comamnds - which could cause an error or action you don't intend.

There are several techniques possible - for instance avoid sending a long stream of commands without any commanbds with a reply. Having a long stream of no-reply commands means the host program cannot track how far through the input buffer the interpreter has got, therefore estimate how many bytes are currently queued.

## Baud rate

If you are changing the baud rate, care must be taken to choose a baud rate that both end can generate accurately. If the total error exceeds of both sides exceeds around 2 or 3% then you are likely to start getting byte errors. Ideally you want to be within 1%.

One such baud rate table for the AVR on the Arduino Nano includes: https://trolsoft.ru/en/uart-calc

Remeber to add on the error rate of the other side as well - whehter that be a Rasberry Pi, USB-Serial converter, or other serial port. Sometimes you can get lucky. If they are both, say +3% of the target, then the baud rates will match. But a -2.5% on one end, and a +2.5% on the other end gives 5% error, and this will cause problems. (Although errors rates up to 5% would theoretically work before it meets an edge, the reality of sampling mechanisms, slew rate and other factors means that realistic error rates are well under half of this.)

Sometimes these cannot be simply looked up from microcontroller or microprocessor data sheets - crystals tend to be accurate, but devices with resonators or internal RC oscillators tend to have large tolerance between devices themselves - and this needs to be taken into account.


