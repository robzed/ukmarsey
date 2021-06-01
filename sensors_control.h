/*
 * Generic sensor system (under development).

   ukmarsey is a machine and human command-based Robot Low-level I/O platform initially targetting UKMARSBot
   For more information see:
       https://github.com/robzed/ukmarsey
       https://ukmars.org/
       https://github.com/ukmars/ukmarsbot
       https://github.com/robzed/pizero_for_ukmarsbot

  MIT License

  Copyright (c) 2020-2021 Rob Probin & Peter Harrison
  Copyright (c) 2019-2021 UK Micromouse and Robotics Society

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef SENSORS_CONTROL_H_
#define SENSORS_CONTROL_H_

void start_sensor_cycle();
void sensors_control_setup();
void print_sensors_control(char mode);
void emitter_on(bool state);
void update_battery_voltage();

// ADC channels
extern volatile int raw_BatteryVolts_adcValue;
extern volatile float battery_voltage;
extern volatile float g_battery_scale; // adjusts PWM for voltage changes
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

/*** steering variables ***/
#ifdef STEERING_CONTROL_IN_LOW_LEVEL_MCU_ENABLE
extern bool g_steering_enabled;
extern volatile float g_cross_track_error;
extern volatile float g_steering_adjustment;
#else
const bool g_steering_enabled = true;
extern volatile float g_steering_adjustment;
#endif
#endif /* SENSORS_CONTROL_H_ */
