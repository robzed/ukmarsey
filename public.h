#ifndef PUBLIC_H
#define PUBLIC_H

void interpreter();

//
// provided by systick.cpp
//
void setupSystick();
void print_bat();
extern float gBatteryVolts;
extern byte gFunctionSwitch;
extern byte gDipSwitch;;

//
// provided by wall-sensors.cpp
//
void wall_sensors_setup();
void print_wall_sensors();
void updateWallSensor();  // usually called from sysTick interrupt

// provided by distance-moved.cpp
void setupEncoders();
void print_encoder_setup();
void zero_encoders();
void print_encoders();

// provided by motor_control.cpp
void setMotorVolts(float left, float right);
void setRightMotorVolts(float volts);
void setLeftMotorVolts(float volts);
void motorSetup();

#endif
