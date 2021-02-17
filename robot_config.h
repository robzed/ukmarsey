#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

//
// Robot Specific Configuration
//
// Depending on how you wired up your motor and encoders, you might need to tweak these values
//

// Encoder polarity is either 1 or -1 and is used to account for reversal of the encoder phases
// The encoders should count up when the robot is moving forward.
#define ENCODER_LEFT_POLARITY (1)
#define ENCODER_RIGHT_POLARITY (-1)

// Similarly, the motors may be wired with different polarity and that is defined here so that
// Setting a positive voltage always moves the robot forwards
#define MOTOR_LEFT_POLARITY (-1)
#define MOTOR_RIGHT_POLARITY (1)

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

#endif
