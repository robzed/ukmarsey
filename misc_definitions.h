//

#ifndef MISC_DEFINITIONS_H
#define MISC_DEFINITIONS_H

// internal use
#define MEASURE_TIMING 0

typedef unsigned long time_measure_t;
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
const int DEFAULT_DECIMAL_PLACES = 3;

#define EXPECTED_ROBOT_CONFIG_VERSION 3
#if ROBOT_CONFIG_VERSION != EXPECTED_ROBOT_CONFIG_VERSION
#error "robot_config.h is the wrong version - have you updated robot_config.h from robot_config_sample.h?"
#endif

#endif
