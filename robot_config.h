#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

//
// Robot Specific Configuration
// 
// Depending on how you wired up your motor and encoders, you might need to tweak these values
// 

// These define which way is forward for each motor.
// Set to 1 or 0, depending upon your wiring.
// NOTE: Since the one motor has to turn the opposite way, then it's often the case that
// one is reversed compared to another unless the wiring is reversed.
#define LEFT_MOTOR_DIRECTION_FORWARD 1
#define LEFT_MOTOR_DIRECTION_BACKWARD (1-LEFT_MOTOR_DIRECTION_FORWARD)

#define RIGHT_MOTOR_DIRECTION_FORWARD 0
#define RIGHT_MOTOR_DIRECTION_BACKWARD (1-RIGHT_MOTOR_DIRECTION_FORWARD)

// These define the encoders.
// Set these to 0 or 1
// The encoders should count up when the robot is moving forward.
// NOTE: Since one motor goes int he opposite direction, it is frequently the case that they are differnet.
#define ENCODER_LEFT_POLARITY 1
#define ENCODER_RIGHT_POLARITY 0

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
