#include <Arduino.h>
#include "hardware_pins.h"
#include "public.h"

/***
 * Global variables
 */

const float MAX_MOTOR_VOLTS = 6.0f;


void motorSetup() {
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  digitalWrite(MOTOR_LEFT_PWM, 0);
  digitalWrite(MOTOR_LEFT_DIR, 0);
  digitalWrite(MOTOR_RIGHT_PWM, 0);
  digitalWrite(MOTOR_RIGHT_DIR, 0);
}

void setLeftMotorPWM(int pwm) {
  pwm = constrain(pwm, -255, 255);
  if (pwm < 0) {
    digitalWrite(MOTOR_LEFT_DIR, 0);
    analogWrite(MOTOR_LEFT_PWM, -pwm);
  } else {
    digitalWrite(MOTOR_LEFT_DIR, 1);
    analogWrite(MOTOR_LEFT_PWM, pwm);
  }
}

void setRightMotorPWM(int pwm) {
  pwm = constrain(pwm, -255, 255);
  if (pwm < 0) {
    digitalWrite(MOTOR_RIGHT_DIR, 1);
    analogWrite(MOTOR_RIGHT_PWM, -pwm);
  } else {
    digitalWrite(MOTOR_RIGHT_DIR, 0);
    analogWrite(MOTOR_RIGHT_PWM, pwm);
  }
}

void setMotorPWM(int left, int right) {
  setLeftMotorPWM(left);
  setRightMotorPWM(right);
}

void setLeftMotorVolts(float volts) {
  volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
  int motorPWM = (int)((255.0f * volts) / gBatteryVolts);
  setLeftMotorPWM(motorPWM);
}

void setRightMotorVolts(float volts) {
  volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
  int motorPWM = (int)((255.0f * volts) / gBatteryVolts);
  setRightMotorPWM(motorPWM);
}

void setMotorVolts(float left, float right) {
  setLeftMotorVolts(left);
  setRightMotorVolts(right);
}
