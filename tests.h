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

#ifndef TESTS_H
#define TESTS_H

#include <Arduino.h>

/***
 * All the tests in this module are normally run via calls to this function
 */
int8_t cmd_test_runner();

/***
 * Canned telemetry logging functions
 */
void log_controller_data();

/*** @brief Just a hacky controller test.
 * Anything could happen
 */
void test_controllers();

// TODO: consider use of on-board switches to select type of test.

/***
 * Fixed voltages are applied to the motors and the robot speed is recorded
 * Plot speed vs voltage to calculate the feedforward constants for forward motion.
 */
void test_fwd_feedforward();
void test_rot_feedforward();

/***
 * Command simple constant speed forward and rotation motion while adjusting
 * the PID controller constants. Data is streamed back to the host for
 * analysis
 */
void test_fwd_speed_control_constants();
void test_rot_speed_control_constants();

/***
 * Complex speed profiles are commanded of the robot and telemetry streamed
 * back to the host for analysis.
 * Feedback and feedforward control are applied so that the robot response to
 * motion commands can be fine tuned. Step, sinusoidal and triangular profiles
 * can be used along with constant velocity.
 */
void test_fwd_motion();
void test_rot_motion();

#endif
