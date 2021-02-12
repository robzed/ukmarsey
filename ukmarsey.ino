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


#include <Arduino.h>
#include "public.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("Hello from ukmarsey"));
  setupSystick();
  //wall_sensors_setup();
  sensors_control_setup();
  setupEncoders();
  motorSetup();
  init_stored_parameters();
}


//extern unsigned long t_systick1;
//extern unsigned long t_systick2;
//extern unsigned long t_systick3;

//unsigned int next_time = 0;

void loop() {

  /*
    if(millis() >= next_time)
    {
      delay(100);
      Serial.println(Serial.availableForWrite());
    Serial.println();
    Serial.print("bat read time=");
    Serial.print(t_systick1);
    Serial.println("µs");
    Serial.print("switch read time=");
    Serial.print(t_systick2);
    Serial.println("µs");
    Serial.print("wall sensor read time=");
    Serial.print(t_systick3);
    Serial.println("µs");

    unsigned long _start = micros();
    unsigned long timing_timing = micros() - _start;
    Serial.print("timing timing =");
    Serial.print(timing_timing);
    Serial.println(" µs");
    next_time = millis() + 2000;  // every 2 seconds
    }
  */


  interpreter();
}
