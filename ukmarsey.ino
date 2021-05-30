/*
 * ukmarsey main file

   ukmarsey is a machine and human command-based Robot Low-level I/O platform initially targetting UKMARSBot
   For more information see:
       https://github.com/robzed/ukmarsey
       https://ukmars.org/
       https://github.com/ukmars/ukmarsbot
       https://github.com/robzed/pizero_for_ukmarsbot

  MIT License

  Copyright (c) 2020-2021 Rob Probin & Peter Harrison
  Copyright (c) 2019-2021 UK Micromouse and Robotics Society

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "settings.h"
#include "stopwatch.h"
#include "sensors_control.h"
#include "motors.h"
#include "distance-moved.h"
#include "systick.h"
#include "interpreter.h"
#include "hardware_pins.h"
#include <Arduino.h>

// TODO: we probably need to deal properly with different types of reset
void setup()
{
    Serial.begin(115200);
    Serial.println(F("\nHello from ukmarsey"));
    load_settings_from_eeprom();
    setup_systick();
    pinMode(USER_IO, OUTPUT);
    pinMode(EMITTER_A, OUTPUT);
    pinMode(EMITTER_B, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    //enable_sensors();
    setup_motors();
    setup_encoders();
    sensors_control_setup();
    //disable_sensors();
}

void loop()
{
    interpreter();
}
