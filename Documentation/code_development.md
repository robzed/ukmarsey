# UKMARSEY Code Development

## How to build the code

### Arduino IDE

ukmarsey builds on the Standard Arduino IDE, where you can also program the Nano.

It also has a serial monitor for talking to the command line - although you'll probably want one of the serial terminals after a while.

This is the simplest option for beginners.

Using the standard Arduino IDE, you should open the file ```ukmarsey.ino``` as you would do for any other Arduino sketch. The IDE will also open a number of other tabs. the code for ukmarsey is split among a number of files for ease of development on other platforms. The Arduino IDE is not very good for more complex projects but, even so, it is possible to open and build the project and then send it to the nano on the UKMARSBOT.

Be sure to selct the correct serial port and choose 'Arduino nano' as the target board.

### Eclipse for Arduino

This is called Sloeber, which is an add-on to Eclipse CDT for Arduino.

The easiest is the Sloeber **product** - this a version of Eclipse with everything installed as a single download.

http://eclipse.baeyens.it/


If you already use Eclipse, then you can install the Sloeber Arduino Plug In via the "Install New Software". You probably want to see this page http://eclipse.baeyens.it/getting-started.shtml

NOTE for Mac: On the latest operating system, Big Sur, requires Eclipse2020-12 - and various changes are being made to make Sloeber 4.4 product and plug-in.

Generally Eclipse has many features which are helpful for development - [this video](https://www.youtube.com/watch?v=QWEvO2DztoI) details a bunch of these. Some examples:

 * Built-in 'scope' / plotter.
 * Code completion
 * Call trees
 * Refactoring
 * TODO: task list
 * Spell correct comments!
 * Version Control
 * File Diffs
 * Library development
 * Good syntax highlighting
 * Debugging target possible
 * A built-in serial monitor

And much more!

The code and issue / bug tracker for the plug-in is https://github.com/Sloeber/arduino-eclipse-plugin

One of the developers of ukmarsey is a long time Eclipse user, and also uses it with PyDev (a Python plug in) to debug and write the PiZero for UKMARSbot at the same time in the same IDE.


### Visual Studio Code with Platform IO

Visual Studio Code (VSCode) is a light weight but comprehensive editor and IDE that can be used to develope software for a huge variety of platforms in many different languages as well as edit almost any kind of human-readable file. VSCODE runs on all major platforms and is now also available to run on a Raspberry Pi.

For the UKMARSEY project, it is being used by at least one contributor to develop the code on the robot, edit this documentation file and for work on the host-side Python files.

For development targeting the Arduino nano on UKMARSBOT, the PlatformIO extension to VSCode is used. If you already know how to use these tools, it should be easy enough to get set up. POlease ask if you want specific instructions for setting up PlatformIo to develop and build this project.

If you have not yet tried VSCode, there is a guide to getting started on the UKMARS website at

https://ukmars.org/resources/platformio-vscode-windows/

One of the developers of ukmarsey is a convert to Visual Studio Code with Platform IO.


## Serial Terminals

Here are some of our favourites.


 * **CoolTerm** - Linux/Windows/Mac/Raspberry Pi. "CoolTerm is a simple serial port terminal application (no terminal emulation) that is geared towards hobbyists and professionals with a need to exchange data with hardware connected to serial ports such as servo controllers, robotic kits, GPS receivers, microcontrollers, etc."  -  https://www.freeware.the-meiers.org/

 * **Teraterm** - Windows only. Free and somewhat hard to use but powerful. Can also be used for SSH connections to remote systems like your Raspberry Pi. - https://ttssh2.osdn.jp/index.html.en

* **PuTTY** - Windows. This free and well-established terminal emulator can do much more than just serial terminal emulation. If you have a Raspberry Pi and would like to connect without firing up a bunch of graphical stuff, you will be using SSH for a terminal session and PuTTY is a good choice. https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html

* **MiniTerm** - Linux/Mac/Windows/Pi. If you have python installed and the serial module, you also have miniterm. It is free and works from the command line. Miniterm does not have a lot of features but, it is lightweight and portable.  Assuming Python is installed, you can add erial support with ```pip install pyserial```. The following command will run miniterm and, if it is installed, display the help file.

      python -m serial.tools.miniterm -h

## Code formatting

We have a '.clang-format' file which can be used with clang-format to ensure the coding style is consistent. That file is in the code folder and will automatically be used for configuration if you use the following command line for format files:

    clang-format -i *.cpp *.h *.ino

If you wish to contribute to the project, we ask that all files be formatted before submission. It just make everything easier. Clang-format is available for all platforms and is part of the LLVM compiler project.

