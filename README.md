# UKMARSEY Wall Follower
A wall follder for the UKMARSBOT robot

** WARNING: This is underdevelopment and doesn't solve mazes yet! **

Currently it has a hardware test via a simple command line interpreter.

Github project: https://github.com/robzed/ukmarsey
Hardware Designed by UK Micromouse and Robotics Society http://ukmars.org
Hardware details: https://github.com/ukmars/ukmarsbot
Examples code sketches: https://github.com/ukmars/ukmarsbot-examples


# Getting Started

Loads into Arduino via serial lead. You can issue commands to get it to do things.


## Switch settings

Nothing yet.

## Serial Commands

All serial commands are lower case.

l (lower case L)    =  Led on/off       l0 = led off, l1 = led on  (Notice: this also control by wall sensor...)
? = just print ok
h = just print ok
s = shows the state of the switches
b = shows the voltage of the battery
w = shows wall sensor readings
e = print encoder current info
z = zero encoders
r = print encoder setup

m = motor tests, runs for 2 seconds or until button is pressed

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
|  ma |   10   | Spin Right 50%                      |
|  mb  |   11   | Pivot Left 25%                      |
|  mc  |   12   | Pivot right 25%                     |
|  md  |   13   | Curve - Left 25%, Right 50%         |
|  me  |   14   | Curve - Left 50%, Right 25%         |
|  mf  |   15   | Curve - Left 75%, Right 50%         |



## Where to find more information

https://www.arduino.cc/reference/en/

http://www.micromouseonline.com/micromouse-book/

# Credits

Based on ukmarsbot examples by UK Micromouse & Robotics Society & Dr. Peter Harrison
https://github.com/ukmars/


# Dev Notes

