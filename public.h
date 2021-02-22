#ifndef PUBLIC_H
#define PUBLIC_H

#include "pid_v1.h"
#include "switches.h"
#include "tests.h"
#include "robot_config.h"
#include <Arduino.h>
#include <wiring_private.h>

// provided by interpreter.cpp
void interpreter();
void init_stored_parameters();
float get_float_param(int param_index);
bool get_bool_param(int param_index);
int decode_input_value(int index);

//
// provided by systick.cpp
void setup_systick();

// ADC channels
extern volatile int raw_BatteryVolts_adcValue;
extern volatile float battery_voltage;
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

// provided by sensors_control.cpp
void start_sensor_cycle();
void sensors_control_setup();
void print_sensors_control(char mode);
extern char emitter_on;

// provided by distance-moved.cpp
void setup_encoders();
void print_encoder_setup();
void zero_encoders();
void update_encoders();
bool print_encoders(char select);

extern float robot_velocity;
extern float robot_omega;

extern float robot_distance;
extern float robot_angle;

extern int32_t encoder_left_total;
extern int32_t encoder_right_total;

// provided by motor_control.cpp
extern float fwd_kp;
extern float fwd_ki;
extern float fwd_kd;
extern float fwd_set_speed;
extern float fwd_volts;
extern PID fwd_controller;

extern float rot_kp;
extern float rot_ki;
extern float rot_kd;
extern float rot_set_speed;
extern float rot_volts;
extern PID rot_controller;

void setMotorVolts(float left, float right);
void setRightMotorVolts(float volts);
void setLeftMotorVolts(float volts);
void setLeftMotorPWM(int pwm);
void setRightMotorPWM(int pwm);
void motorSetup();
void update_motors();

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
