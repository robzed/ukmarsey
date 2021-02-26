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
#include "pid_v1.h"
#include "public.h"
#include "robot_config.h"
#include <Arduino.h>
/***
 * Global variables
 */

float fwd_kp = 0.010;
float fwd_ki = 0.5;
float fwd_kd = 0.000;
float fwd_set_speed;
float fwd_volts;

float rot_kp = 0.010;
float rot_ki = 0.5;
float rot_kd = 0.000;
float rot_set_speed;
float rot_volts;

bool flag_controllers_use_ff = true;

PID fwd_controller(&robot_velocity, &fwd_volts, &fwd_set_speed, fwd_kp, fwd_ki, fwd_kd);
PID rot_controller(&robot_omega, &rot_volts, &rot_set_speed, rot_kp, rot_ki, rot_kd);

enum
{
    PWM_488_HZ,
    PWM_3900_HZ,
    PWM_31_KHZ
};
void pwmSetup(int frequency = PWM_488_HZ)
{
    switch (frequency)
    {
        case PWM_31_KHZ:
            // Divide by 1. frequency = 31.25 kHz;
            bitClear(TCCR1B, CS11);
            bitSet(TCCR1B, CS10);
            break;
        case PWM_3900_HZ:
            // Divide by 8. frequency = 3.91 kHz;
            bitSet(TCCR1B, CS11);
            bitClear(TCCR1B, CS10);
            break;
        case PWM_488_HZ:
        default:
            // Divide by 64. frequency = 488Hz;
            bitSet(TCCR1B, CS11);
            bitSet(TCCR1B, CS10);
            break;
    }
}

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
    pwmSetup(PWM_31_KHZ);

    setMotorVolts(0, 0);
    fwd_controller.SetOutputLimits(-6.0, 6.0);
    fwd_controller.SetMode(AUTOMATIC); // turns on the controller. Set to manual for off.
    rot_controller.SetOutputLimits(-6.0, 6.0);
    rot_controller.SetMode(AUTOMATIC); // turns on the controller. Set to manual for off.
}

//TODO: if controllers can be disabled, we can apply fixed voltage/pwm to motors
void update_motors()
{
    rot_controller.Compute();
    fwd_controller.Compute();
    // assume both motors behave the same
    const float k_velocity_ff = (1.0 / 280.0);
    const float k_bias_ff = (23.0 / 280.0);

    float left_volts = 0;
    float right_volts = 0;

    left_volts += fwd_volts;
    right_volts += fwd_volts;

    left_volts -= rot_volts;
    right_volts += rot_volts;

    if (flag_controllers_use_ff)
    {
        float fwd_ff = fwd_set_speed * k_velocity_ff;
        float rot_ff = rot_set_speed * (WHEEL_SEPARATION / (2 * 57.29)) * k_velocity_ff;
        // rot_ff = 0;

        left_volts += fwd_ff;
        right_volts += fwd_ff;

        left_volts -= rot_ff;
        right_volts += rot_ff;
    }

    setMotorVolts(left_volts, right_volts);
    // setMotorVolts(1.4, 1.4);
    // setMotorVolts(0, 0);
};

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
    int motorPWM = (int)((255.0f * volts) / battery_voltage);
    setLeftMotorPWM(motorPWM);
}

void setRightMotorVolts(float volts)
{
    volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
    int motorPWM = (int)((255.0f * volts) / battery_voltage);
    setRightMotorPWM(motorPWM);
}

void setMotorVolts(float left, float right)
{
    setLeftMotorVolts(left);
    setRightMotorVolts(right);
}
