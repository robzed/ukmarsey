#ifndef HARDWARE_PINS_H
#define HARDWARE_PINS_H
#include <Arduino.h>
/**
 * Hardware pin defines
 */

#include <Arduino.h>

#define BOARD UKMARSBOT_V1
const uint8_t ENCODER_LEFT_CLK = 2;
const uint8_t ENCODER_RIGHT_CLK = 3;
const uint8_t ENCODER_LEFT_B = 4;
const uint8_t ENCODER_RIGHT_B = 5;
const uint8_t USER_IO = 6;
const uint8_t MOTOR_LEFT_DIR = 7;
const uint8_t MOTOR_RIGHT_DIR = 8;
const uint8_t MOTOR_LEFT_PWM = 9;
const uint8_t MOTOR_RIGHT_PWM = 10;
const uint8_t LED_RIGHT = 6;
const uint8_t LED_LEFT = 11;
const uint8_t EMITTER_A = 11; // alias
const uint8_t EMITTER = 12;
const uint8_t EMITTER_B = 12; // alias
const uint8_t SENSOR_RIGHT_MARK = A0;
const uint8_t SENSOR_1 = A1;
const uint8_t SENSOR_2 = A2;
const uint8_t SENSOR_3 = A3;
const uint8_t SENSOR_4 = A4;
const uint8_t SENSOR_LEFT_MARK = A5;
const uint8_t FUNCTION_PIN = A6;
const uint8_t BATTERY_VOLTS = A7;
/****/
#endif
