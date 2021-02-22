/*
 * Tests. Various test functions for demonstration of facilities, tuning and development.

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
#include "tests.h"
#include "public.h"
#include "switches.h"

void cmd_test_runner()
{
    int test = decode_input_value(1);
    Serial.print(F("TEST: "));
    Serial.println(test);
}

// ----------------- telemetry functions

void log_controller_data()
{
    Serial.print(millis());
    Serial.print(' ');
    Serial.print(fwd_set_speed);
    Serial.print(' ');
    Serial.print(rot_set_speed);
    Serial.print(' ');
    Serial.print(robot_velocity);
    Serial.print(' ');
    Serial.print(robot_omega);
    Serial.print(' ');
    Serial.print(fwd_volts);
    Serial.print(' ');
    Serial.print(rot_volts); // placeholder for controller voltage
    Serial.println();
}

void test_fwd_feedforward(){

};

void test_rot_feedforward(){

};

void test_fwd_speed_control_constants(){

};

void test_rot_speed_control_constants(){

};

void test_fwd_motion(){

};

void test_rot_motion(){

};
