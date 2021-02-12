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
#include "public.h"
/***
 * Global variables
 */
// the current value of the sensors

volatile int gSensorA0_dark;
volatile int gSensorA1_dark;
volatile int gSensorA2_dark;
volatile int gSensorA3_dark;
volatile int gSensorA4_dark;
volatile int gSensorA5_dark;

volatile int gSensorA0_light;
volatile int gSensorA1_light;
volatile int gSensorA2_light;
volatile int gSensorA3_light;
volatile int gSensorA4_light;
volatile int gSensorA5_light;
void analogueSetup() {
  // increase speed of ADC conversions to 28us each
  // by changing the clock prescaler from 128 to 16
  bitClear(ADCSRA, ADPS0);
  bitClear(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2);
}
char emitter_on = 1;



void sensors_control_setup() {
  pinMode(EMITTER, OUTPUT);
  digitalWriteFast(EMITTER, 0);  // be sure the emitter is off
  analogueSetup();               // increase the ADC conversion speed
}

void print_hex2(int value) {
  value = constrain(value, 0, 255);
  Serial.print(value / 16, HEX);
  Serial.print(value % 16, HEX);
}

void print_sensors_control(char mode) {
  int a0_dark;
  int a1_dark;
  int a2_dark;
  int a3_dark;
  int a4_dark;
  int a5_dark;
  int a0_lit;
  int a1_lit;
  int a2_lit;
  int a3_lit;
  int a4_lit;
  int a5_lit;
  const char comma = ',';

  // read the sensors
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    a0_dark = gSensorA0_dark;
    a0_lit = gSensorA0_light;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    a1_dark = gSensorA1_dark;
    a1_lit = gSensorA1_light;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    a2_dark = gSensorA2_dark;
    a2_lit = gSensorA2_light;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    a3_dark = gSensorA3_dark;
    a3_lit = gSensorA3_light;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    a4_dark = gSensorA4_dark;
    a4_lit = gSensorA4_light;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    a5_dark = gSensorA5_dark;
    a5_lit = gSensorA5_light;
  }

  if (mode == 'd') {  // the default is decimal differences
    Serial.print(max(a0_lit - a0_dark, 0));
    Serial.print(comma);
    Serial.print(max(a1_lit - a1_dark, 0));
    Serial.print(comma);
    Serial.print(max(a2_lit - a2_dark, 0));
    Serial.print(comma);
    Serial.print(max(a3_lit - a3_dark, 0));
    Serial.print(comma);
    Serial.print(max(a4_lit - a4_dark, 0));
    Serial.print(comma);
    Serial.print(max(a5_lit - a5_dark, 0));
  } else if (mode == 'h') {  // display differences as hex values
    print_hex2(a0_lit - a0_dark);
    print_hex2(a1_lit - a1_dark);
    print_hex2(a2_lit - a2_dark);
    print_hex2(a3_lit - a3_dark);
    print_hex2(a4_lit - a4_dark);
    print_hex2(a5_lit - a5_dark);
  } else if (mode == 'r') { // display both dark and lit values
    Serial.print(a0_dark);
    Serial.print(comma);
    Serial.print(a1_dark);
    Serial.print(comma);
    Serial.print(a2_dark);
    Serial.print(comma);
    Serial.print(a3_dark);
    Serial.print(comma);
    Serial.print(a4_dark);
    Serial.print(comma);
    Serial.print(a5_dark);
    Serial.print(comma);
    Serial.print(' ');
    Serial.print(' ');
    Serial.print(a0_lit);
    Serial.print(comma);
    Serial.print(a1_lit);
    Serial.print(comma);
    Serial.print(a2_lit);
    Serial.print(comma);
    Serial.print(a3_lit);
    Serial.print(comma);
    Serial.print(a4_lit);
    Serial.print(comma);
    Serial.print(a5_lit);
  }
  Serial.println();
}
