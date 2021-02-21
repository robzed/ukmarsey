# UKMARSEY Code Development

## How to build the code

### Arduino IDE

ukmarsey builds on the Standard Arduino IDE, where you can also program the Nano. 

It also has a serial monitor for talking to the command line - although you'll probably want one of the serial terminal after a while.

This is the simplest option for beginners.


### Eclipse for Arduino

This is called Sloeber, which is an add-on to Eclipse CDT for Arduino.

The easiest is the Sloeber **product** - this a version of Eclipse with everything installed as a single download.

http://eclipse.baeyens.it/


If you already use Eclipse, then you can install Sloeber Arduino Plug via the "Install New Software". You probably want to see this page http://eclipse.baeyens.it/getting-started.shtml

NOTE for Mac: On the latest operating system, Big Sur, requires Eclipse2020-12 - and various changes are being made to make Sloeber 4.4 product and plug-in.

Generally Eclipse has many features which are helpful for development - [this video](https://www.youtube.com/watch?v=QWEvO2DztoI) details a bunch of these. Some examples:

 * Built-in 'scope' / plotter.
 * Code completion
 * Call trees
 * Refactoring
 * TODO: task list
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

(write some nice things here)

One of the developers of ukmarsey is a convert to Visual Studio Code with Platform IO.



## Serial Terminals

Here are some of our favourites.


 * CoolTerm - Linux/Windows/Mac/Raspberry Pi. https://www.freeware.the-meiers.org/

 * Teraterm - Windows, somewhat hard to use but powerful. https://ttssh2.osdn.jp/index.html.en




# Code formatting

We have a '.clang-format' file which can be used with clang-format to ensure the coding style is consistent.


