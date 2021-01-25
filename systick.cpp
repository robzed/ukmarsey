/*
 * Systick

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
#include <Arduino.h>
#include "hardware_pins.h"
#include "public.h"

/***
 * Global variables
 */

int raw_BatteryVolts_adcValue;
float raw_BatteryVolts;
int Switch_ADC_value;
const float batteryDividerRatio = 2.0f;

/***/

void updateBatteryVolts() {
  raw_BatteryVolts_adcValue = analogRead(BATTERY_VOLTS);
  raw_BatteryVolts = raw_BatteryVolts_adcValue * (5.0f * batteryDividerRatio / 1023.0f);
}

float get_BatteryVolts()
{
  return raw_BatteryVolts;
  //return raw_BatteryVolts_adcValue * (5.0f * batteryDividerRatio / 1023.0f);
}


/** @brief  Read the raw switch reading
 *  @return void
 */
void updateFunctionSwitch() {
  /**
   * Typical ADC values for all function switch settings
   */
  Switch_ADC_value = analogRead(FUNCTION_PIN);
}

/** @brief  Convert the switch ADC reading into a switch reading.
 *  @return void
 */
int readFunctionSwitch() {
  const int adcReading[] = {660, 647, 630, 614, 590, 570, 545, 522, 461,
                            429, 385, 343, 271, 212, 128, 44,  0};

  if(Switch_ADC_value > 1000){
    return 16;
  }
  for (int i = 0; i < 16; i++) {
    if (Switch_ADC_value > (adcReading[i] + adcReading[i + 1]) / 2) {
      return i;
    }
  }
  return 15;   // should never get here... but if we do show 15
}

/***
 * If you are interested in what all this does, the ATMega328P datasheet
 * has all the answers but it is not easy to follow until you have some
 * experience. For now just use the code as it is.
 */
void setupSystick() {
  // set the mode for timer 2
  bitClear(TCCR2A, WGM20);
  bitSet(TCCR2A, WGM21);
  bitClear(TCCR2B, WGM22);
  // set divisor to 128 => timer clock = 125kHz
  bitSet(TCCR2B, CS22);
  bitClear(TCCR2B, CS21);
  bitSet(TCCR2B, CS20);
  // set the timer frequency
  OCR2A = 249;  // (16000000/128/500)-1 = 249
  // enable the timer interrupt
  bitSet(TIMSK2, OCIE2A);
}

unsigned long t_systick1 = 0;
unsigned long t_systick2 = 0;
unsigned long t_systick3 = 0;

// The systick event is an ISR attached to Timer 2
// This currently runs at 2ms or 500Hz.
ISR(TIMER2_COMPA_vect) {
  //unsigned long _start = micros();
  updateBatteryVolts();
  //t_systick1 = micros() - _start;

  //_start = micros();
  updateFunctionSwitch();
  //t_systick2 = micros() - _start;

  //_start = micros();
  //updateWallSensor();
  update_sensors_control();
  //t_systick3 = micros() - _start;

  //speed_control();
}

void print_bat()
{
    Serial.println(get_BatteryVolts(), floating_decimal_places);
    //Serial.println(F(" v"));
}
