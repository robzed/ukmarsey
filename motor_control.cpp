#include <Arduino.h>
#include "hardware_pins.h"
#include "public.h"
#include "robot_config.h"

/***
 * Global variables
 */



void motorSetup() {
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  digitalWrite(MOTOR_LEFT_PWM, 0);
  digitalWrite(MOTOR_LEFT_DIR, LEFT_MOTOR_DIRECTION_FORWARD);
  digitalWrite(MOTOR_RIGHT_PWM, 0);
  digitalWrite(MOTOR_RIGHT_DIR, RIGHT_MOTOR_DIRECTION_FORWARD);
}

void setLeftMotorPWM(int pwm) {
  pwm = constrain(pwm, -255, 255);
  if (pwm < 0) {
    digitalWrite(MOTOR_LEFT_DIR, LEFT_MOTOR_DIRECTION_BACKWARD);
    analogWrite(MOTOR_LEFT_PWM, -pwm);
  } else {
    digitalWrite(MOTOR_LEFT_DIR, LEFT_MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_LEFT_PWM, pwm);
  }
}

void setRightMotorPWM(int pwm) {
  pwm = constrain(pwm, -255, 255);
  if (pwm < 0) {
    digitalWrite(MOTOR_RIGHT_DIR, RIGHT_MOTOR_DIRECTION_BACKWARD);
    analogWrite(MOTOR_RIGHT_PWM, -pwm);
  } else {
    digitalWrite(MOTOR_RIGHT_DIR, RIGHT_MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_RIGHT_PWM, pwm);
  }
}

void setMotorPWM(int left, int right) {
  setLeftMotorPWM(left);
  setRightMotorPWM(right);
}

void setLeftMotorVolts(float volts) {
  volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
  int motorPWM = (int)((255.0f * volts) / get_BatteryVolts());
  setLeftMotorPWM(motorPWM);
}

void setRightMotorVolts(float volts) {
  volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
  int motorPWM = (int)((255.0f * volts) / get_BatteryVolts());
  setRightMotorPWM(motorPWM);
}

void setMotorVolts(float left, float right) {
  setLeftMotorVolts(left);
  setRightMotorVolts(right);
}
