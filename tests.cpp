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
#include "interpreter.h"
#include "read-number.h"
#include "stopwatch.h"
#include "switches.h"
#include "tests.h"

int8_t cmd_test_runner()
{
    Stopwatch sw;
    int test;
    test = decode_input_value(1);
    // int index = 1;
    // bool ok = read_integer(inputString, &index, &test);
    Serial.print(F("TEST: "));
    Serial.println(test);
    Serial.println(sw.split());
    switch (test)
    {
        case 1:
            test_controllers();
            break;
        default:
            break;
    }
    return T_OK;
}

// ----------------- telemetry functions

void log_controller_data()
{
#if 0
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
#endif
}

/***
 * Illustrates how to setup and run a test.
 * The main user button can be used to start and stop the test
 * It will also run for only a set time.
 * The function switches can be used to determine the test action.
 * Most tests need not be this complicated.
 */
void test_controllers()
{
#if 0
    enable_controllers();
    Serial.print(F("CONTROLLER TEST - "));
    bool was_using_ff = flag_controllers_use_ff;
    if (readFunctionSwitch() & 0x01)
    {
        flag_controllers_use_ff = true;
        Serial.println(F("WITH FF"));
    }
    else
    {
        flag_controllers_use_ff = false;
        Serial.println(F("NO FF"));
    }
    Serial.println(F("Press the button when ready"));
    wait_for_button_click();
    delay(100);
    uint32_t tick = millis() + 10;
    Stopwatch sw;
    while (not button_pressed() && sw.split() < (5 * ONE_SECOND))
    {
        if (millis() > tick)
        {
            tick += 10;
            int ms = millis() % 1000;
            // fwd_set_speed = 600 * sin(0.5*millis()/157.1);
            fwd_set_speed = (ms > 500) ? 400 : -400;
            // fwd_set_speed = 400;
            // rot_set_speed = 1080 * sin(1 * millis() / 157.1);
            // rot_set_speed = 600;
            // fwd_set_speed = 0;
            log_controller_data();
        }
    }
    fwd_set_speed = 0;
    rot_set_speed = 0;
    flag_controllers_use_ff = was_using_ff;
    disable_controllers();
    setMotorVolts(0, 0);
#endif
};

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
