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
#include "digitalWriteFast.h"
#include "hardware_pins.h"
#include "systick.h"
#include "sensors_control.h"
#include "distance-moved.h"
#include "profile.h"
#include "motors.h"
#include <Arduino.h>
#include <pins_arduino.h>
#include <wiring_private.h>

/***
 * If you are interested in what all this does, the ATMega328P datasheet
 * has all the answers but it is not easy to follow until you have some
 * experience. For now just use the code as it is.
 */
void setup_systick()
{
    // set the mode for timer 2
    bitClear(TCCR2A, WGM20);
    bitSet(TCCR2A, WGM21);
    bitClear(TCCR2B, WGM22);

    // set divisor to 128 => timer clock = 125kHz
    bitSet(TCCR2B, CS22);
    bitClear(TCCR2B, CS21);
    bitSet(TCCR2B, CS20);

    // set the timer frequency to 500Hz
    OCR2A = 249;            // (16000000/128/500)-1 = 249
    bitSet(TIMSK2, OCIE2A); // enable the timer interrupt
}

/** @brief This is the systick event - an ISR connected to Timer 2
 * Running at 500Hz, the systick interrupt handles all the regular
 * control updates including
 *   - switch debounce
 *   - speed and odometry updates from the encoders
 *   - speed control of the robot
 *   - such other tasks as may be required from time to time
 * At the end of each systck interrupt, the ADC interrupt is enables and
 * an ADC conversion started. This begins the sensor read cycle. Details
 * are in ISR(ADC_vect)
 *
 * The code running in the systck interrupt should execute as fast as possible
 * and should not run for more than about 500us in total to avoid excessive
 * system load.
 *
 * NOTE: during the systick interrupt service global interrupts are enabled
 * so that higher priority interrupts from the encoderd and UART can be serviced.
 * These will impact total run time for this ISR and care must be taken when
 * handling values that may be changed by these other iterrupt sources.
 *
 * For testing, the built-in LED can be turned on at the start of the interrupt
 * and off again at the end. This can be used to measure system load.
 * Disable the feature if you want to use the built-in LED for some other
 * purpose.
 *
 */
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK)
{
    // digitalWriteFast(LED_BUILTIN, 1);
    // grab the encoder values first because they will continue to change
    update_encoders();
    update_battery_voltage();

    battery_voltage = raw_BatteryVolts_adcValue * (2.0 * 5.0 / 1024.0);

    forward.update();
    rotation.update();
#ifdef STEERING_CONTROL_IN_LOW_LEVEL_MCU_ENABLE
    g_cross_track_error = update_wall_sensors();
    g_steering_adjustment = calculate_steering_adjustment(g_cross_track_error);
    update_motor_controllers(g_steering_adjustment);
#else
    update_motor_controllers(g_steering_adjustment);
#endif

    // digitalWriteFast(LED_BUILTIN, 0);
    start_sensor_cycle();
    // NOTE: no code should follow this line;
}
