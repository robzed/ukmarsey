/*
 * motor_control - provide motor drive via PWM.

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
#include "digitalWriteFast.h"
#include "hardware_pins.h"
#include "public.h"
#include "robot_config.h"
#include <Arduino.h>

/***
 * Global variables
 */

void motorSetup()
{
    pinMode(MOTOR_LEFT_DIR, OUTPUT);
    pinMode(MOTOR_RIGHT_DIR, OUTPUT);
    pinMode(MOTOR_LEFT_PWM, OUTPUT);
    pinMode(MOTOR_RIGHT_PWM, OUTPUT);
    digitalWriteFast(MOTOR_LEFT_PWM, 0);
    digitalWriteFast(MOTOR_LEFT_DIR, 0);
    digitalWriteFast(MOTOR_RIGHT_PWM, 0);
    digitalWriteFast(MOTOR_RIGHT_DIR, 0);
}

void setLeftMotorPWM(int pwm)
{
    pwm = MOTOR_LEFT_POLARITY * constrain(pwm, -255, 255);
    if (pwm < 0)
    {
        digitalWriteFast(MOTOR_LEFT_DIR, 1);
        analogWrite(MOTOR_LEFT_PWM, -pwm);
    }
    else
    {
        digitalWriteFast(MOTOR_LEFT_DIR, 0);
        analogWrite(MOTOR_LEFT_PWM, pwm);
    }
}

void setRightMotorPWM(int pwm)
{
    pwm = MOTOR_RIGHT_POLARITY * constrain(pwm, -255, 255);
    if (pwm < 0)
    {
        digitalWriteFast(MOTOR_RIGHT_DIR, 1);
        analogWrite(MOTOR_RIGHT_PWM, -pwm);
    }
    else
    {
        digitalWriteFast(MOTOR_RIGHT_DIR, 0);
        analogWrite(MOTOR_RIGHT_PWM, pwm);
    }
}

void setMotorPWM(int left, int right)
{
    setLeftMotorPWM(left);
    setRightMotorPWM(right);
}

void setLeftMotorVolts(float volts)
{
    volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
    int motorPWM = (int)((255.0f * volts) / get_BatteryVolts());
    setLeftMotorPWM(motorPWM);
}

void setRightMotorVolts(float volts)
{
    volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
    int motorPWM = (int)((255.0f * volts) / get_BatteryVolts());
    setRightMotorPWM(motorPWM);
}

void setMotorVolts(float left, float right)
{
    setLeftMotorVolts(left);
    setRightMotorVolts(right);
}
