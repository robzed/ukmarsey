#ifndef PUBLIC_H
#define PUBLIC_H

#include "robot_config.h"
#include <Arduino.h>
// provided by interpreter.cpp
void interpreter();
void init_stored_parameters();
float get_float_param(int param_index);
bool get_bool_param(int param_index);

//
// provided by systick.cpp
//
void setupSystick();
void print_bat();
float get_BatteryVolts();
int readFunctionSwitch();

// ADC channels
extern volatile int raw_BatteryVolts_adcValue;
extern volatile int Switch_ADC_value;

extern volatile int gSensorA0_dark;
extern volatile int gSensorA1_dark;
extern volatile int gSensorA2_dark;
extern volatile int gSensorA3_dark;
extern volatile int gSensorA4_dark;
extern volatile int gSensorA5_dark;

extern volatile int gSensorA0_light;
extern volatile int gSensorA1_light;
extern volatile int gSensorA2_light;
extern volatile int gSensorA3_light;
extern volatile int gSensorA4_light;
extern volatile int gSensorA5_light;

//
// provided by wall_sensors.cpp
//
//void wall_sensors_setup();
//void print_wall_sensors();
//void updateWallSensor();  // usually called from sysTick interrupt

// provided by sensors_control.cpp
void sensors_control_setup();
void print_sensors_control(char mode);
extern char emitter_on;

// provided by distance-moved.cpp
void setup_encoders();
void print_encoder_setup();
void zero_encoders();
void update_encoders();
void print_encoders();

extern double robot_velocity;
extern double robot_omega;

extern float robot_distance;
extern float robot_angle;

extern int32_t encoderLeftTotal;
extern int32_t encoderRightTotal;

// provided by motor_control.cpp
void setMotorVolts(float left, float right);
void setRightMotorVolts(float volts);
void setLeftMotorVolts(float volts);
void setLeftMotorPWM(int pwm);
void setRightMotorPWM(int pwm);
void motorSetup();

// internal use
#define MEASURE_TIMING 0

typedef unsigned time_measure_t;
#if MEASURE_TIMING
#define TIME_START(START_VARIABLE) START_VARIABLE = micros();
#define TIME_END(START_VARIABLE, END_VARIABLE) END_VARIABLE = micros() - START_VARIABLE;
#define TIME_DEFINE_VARIABLE(VARIABLE) time_measure_t VARIABLE;
#else
#define TIME_START(START_VARIABLE)
#define TIME_END(START_VARIABLE, END_VARIABLE)
#define TIME_DEFINE_VARIABLE(VARIABLE)
#endif

// Other constants
const int floating_decimal_places = 3;

#endif
