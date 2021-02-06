/*
 * Generic sensor system (under development).

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
#include "digitalWriteFast.h"
#include "hardware_pins.h"
#include <util/atomic.h>

/***
 * Global variables
 */
// the current value of the sensors
volatile int gSensorA0_dark;
volatile int gSensorA1_dark;
volatile int gSensorA2_dark;
volatile int gSensorA3_dark;
volatile int gSensorA0_light;
volatile int gSensorA1_light;
volatile int gSensorA2_light;
volatile int gSensorA3_light;
void analogueSetup() {
  // increase speed of ADC conversions to 28us each
  // by changing the clock prescaler from 128 to 16
  bitClear(ADCSRA, ADPS0);
  bitClear(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2);
}
char emitter_on = 1;

void update_sensors_control() {
  // first read them dark
  int a0 = analogRead(A0);
  int a1 = analogRead(A1);
  int a2 = analogRead(A2);
  int a3 = analogRead(A3);
  int a0_ = a0;
  int a1_ = a1;
  int a2_ = a2;
  int a3_ = a3;  // they should read as the same if emitter is off.

  if (emitter_on) {
    // light them up
    digitalWriteFast(EMITTER, 1);

    // wait until all the detectors are stable
    delayMicroseconds(50);

    // now find the differences
    a0_ = analogRead(A0);
    a1_ = analogRead(A1);
    a2_ = analogRead(A2);
    a3_ = analogRead(A3);
    // and go dark again.
    digitalWriteFast(EMITTER, 0);
  }

  // make the results available to the rest of the program
  gSensorA0_dark = a0;
  gSensorA1_dark = a1;
  gSensorA2_dark = a2;
  gSensorA3_dark = a3;
  gSensorA0_light = a0_;
  gSensorA1_light = a1_;
  gSensorA2_light = a2_;
  gSensorA3_light = a3_;
}

void sensors_control_setup() {
  pinMode(EMITTER, OUTPUT);
  digitalWriteFast(EMITTER, 0);  // be sure the emitter is off
  analogueSetup();               // increase the ADC conversion speed
}


void print_sensors_control(char mode)
{
  int gSensorA0_dark_;
  int gSensorA1_dark_;
  int gSensorA2_dark_;
  int gSensorA3_dark_;
  int gSensorA0_light_;
  int gSensorA1_light_;
  int gSensorA2_light_;
  int gSensorA3_light_;

  // read the sensors
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { 
    gSensorA0_dark_ = gSensorA0_dark;
    gSensorA1_dark_ = gSensorA1_dark;
    gSensorA2_dark_ = gSensorA2_dark;
    gSensorA3_dark_ = gSensorA3_dark;
    gSensorA0_light_ = gSensorA0_light;
    gSensorA1_light_ = gSensorA1_light;
    gSensorA2_light_ = gSensorA2_light;
    gSensorA3_light_ = gSensorA3_light;
  }

  const char comma = ',';
  if(mode == 'd')
  {
    Serial.print(gSensorA0_light_ - gSensorA0_dark_);
    Serial.print(comma);
    Serial.print(gSensorA1_light_ - gSensorA1_dark_);
    Serial.print(comma);
    Serial.print(gSensorA2_light_ - gSensorA2_dark_);
    Serial.print(comma);
    Serial.print(gSensorA3_light_ - gSensorA3_dark_);
  }
  else if(mode == 'h')
  {
    gSensorA0_light_ = min(gSensorA0_light_-gSensorA0_dark_, 255);
    if(gSensorA0_light_ < 0x10) { Serial.print('0'); }
    Serial.print(gSensorA0_light_, HEX);
    gSensorA1_light_ = min(gSensorA1_light_-gSensorA1_dark_, 255);
    if(gSensorA1_light_ < 0x10) { Serial.print('0'); }
    Serial.print(gSensorA1_light_, HEX);
    gSensorA2_light_ = min(gSensorA2_light_-gSensorA2_dark_, 255);
    if(gSensorA2_light_ < 0x10) { Serial.print('0'); }
    Serial.print(gSensorA2_light_, HEX);
    gSensorA3_light_ = min(gSensorA3_light_-gSensorA3_dark_, 255);
    if(gSensorA3_light_ < 0x10) { Serial.print('0'); }
    Serial.print(gSensorA3_light_, HEX);
  }
  else if(mode == 'r')
  {
    Serial.print(gSensorA0_dark_);
    Serial.print(comma);
    Serial.print(gSensorA1_dark_);
    Serial.print(comma);
    Serial.print(gSensorA2_dark_);
    Serial.print(comma);
    Serial.print(gSensorA3_dark_);
    Serial.print(comma);
    Serial.print(gSensorA0_light_);
    Serial.print(comma);
    Serial.print(gSensorA1_light_);
    Serial.print(comma);
    Serial.print(gSensorA2_light_);
    Serial.print(comma);
    Serial.print(gSensorA3_light_);
  }
  Serial.println();  // sends "\r\n"
}
