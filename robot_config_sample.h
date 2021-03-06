#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include <arduino.h>
/*******************************************************************
 *
 * Copy this file as robot_config.h in the working directory
 * and edit the contents to match your particular robot.
 *
 * This sample is maintained by git but the working copy is not.
 *
 * So don't lose it
 *
 *******************************************************************/
//
// Robot Specific Configuration

/***
 * Include the polarity information. This file is not part of the repo but should
 * contain the following lines

     // Encoder polarity is either 1 or -1 and is used to account for reversal of the encoder phases
     // The encoders should count up when the robot is moving forward.
     #define ENCODER_LEFT_POLARITY (-1)
     #define ENCODER_RIGHT_POLARITY (1)

     // Similarly, the motors may be wired with different polarity and that is defined here so that
     // Setting a positive voltage always moves the robot forwards
     #define MOTOR_LEFT_POLARITY (1)
     #define MOTOR_RIGHT_POLARITY (-1)

*/
// Depending on how you wired up your motor and encoders, you might need to tweak these values
//

// Encoder polarity is either 1 or -1 and is used to account for reversal of the encoder phases
// The encoders should count up when the robot is moving forward.
#define ENCODER_LEFT_POLARITY (-1)
#define ENCODER_RIGHT_POLARITY (1)

// Similarly, the motors may be wired with different polarity and that is defined here so that
// Setting a positive voltage always moves the robot forwards
#define MOTOR_LEFT_POLARITY (1)
#define MOTOR_RIGHT_POLARITY (-1)

/***
 * Global robot characteristic constants
 */
const int COUNTS_PER_ROTATION = 12;
const float GEAR_RATIO = 19.5;
const float WHEEL_DIAMETER = 32.5f;
const float WHEEL_SEPARATION = 75.2;

// You probably don't need to adjust these
const float MM_PER_COUNT = (PI * WHEEL_DIAMETER) / (2 * COUNTS_PER_ROTATION * GEAR_RATIO);
const float DEG_PER_COUNT = (360.0 * MM_PER_COUNT) / (PI * WHEEL_SEPARATION);

// The is the maximum voltage the motor should be driven at when
// using voltage based control.
const float MAX_MOTOR_VOLTS = 6.0f;

const float LOOP_FREQUENCY = 500.0;                //Hz
const float LOOP_INTERVAL = 1.0f / LOOP_FREQUENCY; //seconds

// Motor controller constants
// These are the defaults that will be loaded into settings
const float SPEED_FF = (1.0 / 280.0);
const float BIAS_FF = (23.0 / 280.0);
const float FWD_KP = 0.010;
const float FWD_KI = 0.500;
const float FWD_KD = 0.000;
const float ROT_KP = 0.010;
const float ROT_KI = 0.500;
const float ROT_KD = 0.010;

#define ROBOT_CONFIG_VERSION 2

#endif
