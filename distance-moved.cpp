/*
 * distance-moved - provides encoder interrupts to measure distance moved
 * usually by encoders on the motor shafts.

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
#include "public.h"
#include "robot_config.h"
#include "stopwatch.h"
#include <Arduino.h>
#include <util/atomic.h>

/***
 * Global variables
 *
 * Raw count values are not normally used informative except when calibrating the
 * robot to calculate the counts MM_PER_COUNT and DEG_PER_COUNT constants.
 *
 * Even so, the raw count values are retained because they do not suffer
 * from floating point error accumulation when calculating the total distance
 * and angle travelled.
 */
int32_t encoder_left_total;  // counts
int32_t encoder_right_total; // counts

float robot_distance; // mm
float robot_angle;    // degrees

float robot_velocity; // mm/s
float robot_omega;    // deg/s

/***
 * Local variables
 *
 * The encoders accumulate counts as the wheels turn.
 * The count for each wheel encoder changes frequently and means nothing without
 * an associated time interval.
 *
 * At the beginning of each control loop interval, the counts are captured and
 * used to update the current speeds and distances.
 *
 */

static volatile int encoder_left_count;  // Updated by pin change interrupts. Reset every loop interval.
static volatile int encoder_right_count; // Updated by pin change interrupts. Reset every loop interval.

void setup_encoders()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // left
        pinMode(ENCODER_LEFT_CLK, INPUT);
        pinMode(ENCODER_LEFT_B, INPUT);
        // configure the pin change
        bitClear(EICRA, ISC01);
        bitSet(EICRA, ISC00);
        // enable the interrupt
        bitSet(EIMSK, INT0);
        encoder_left_count = 0;
        // right
        pinMode(ENCODER_RIGHT_CLK, INPUT);
        pinMode(ENCODER_RIGHT_B, INPUT);
        // configure the pin change
        bitClear(EICRA, ISC11);
        bitSet(EICRA, ISC10);
        // enable the interrupt
        bitSet(EIMSK, INT1);
        encoder_right_count = 0;
    }
    zero_encoders();
}

/***
 * update_encoders() must be called at the control loop frequency so that
 * speeds and distances can be correctly calculated.
 */
void update_encoders()
{
    int left_count;
    int right_count;
    // Make sure values don't change while being read. Be quick.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        left_count = encoder_left_count;
        right_count = encoder_right_count;
        encoder_left_count = 0;
        encoder_right_count = 0;
    }

    encoder_right_total += right_count;
    encoder_left_total += left_count;
    // calculate and low pass filter the speeds
    //TODO: how does the filtering affect the controller?
    float velocity = LOOP_FREQUENCY * MM_PER_COUNT * (right_count + left_count);
    robot_velocity += 0.5 * (velocity - robot_velocity);

    float omega = LOOP_FREQUENCY * DEG_PER_COUNT * (right_count - left_count);
    robot_omega += 0.5 * (omega - robot_omega);

    robot_distance = MM_PER_COUNT * (encoder_right_total + encoder_left_total);
    robot_angle = DEG_PER_COUNT * (encoder_right_total - encoder_left_total);
}

// Interrupt called every time there is a change on the left encoder
ISR(INT0_vect)
{
    static bool oldA = 0;
    static bool oldB = 0;
    bool newB = digitalReadFast(ENCODER_LEFT_B);
    bool newA = digitalReadFast(ENCODER_LEFT_CLK) ^ newB;
    int delta = ENCODER_LEFT_POLARITY * ((oldA ^ newB) - (newA ^ oldB));
    encoder_left_count += delta;
    oldA = newA;
    oldB = newB;
}

// Interrupt called every time there is a change on the right encoder
ISR(INT1_vect)
{
    static bool oldA = 0;
    static bool oldB = 0;
    bool newB = digitalReadFast(ENCODER_RIGHT_B);
    bool newA = digitalReadFast(ENCODER_RIGHT_CLK) ^ newB;
    int delta = ENCODER_RIGHT_POLARITY * ((oldA ^ newB) - (newA ^ oldB));
    encoder_right_count += delta;
    oldA = newA;
    oldB = newB;
}

/******************************** command functions **************************/

// command 'r'
void print_encoder_setup()
{
    const char comma = ',';

    Serial.print(MM_PER_COUNT, 5);
    Serial.print(comma);
    Serial.println(DEG_PER_COUNT, 5);
}

// command 'z'
void zero_encoders()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        encoder_left_total = 0;
        encoder_right_total = 0;
        robot_distance = 0;
        robot_angle = 0;
    }
}

// command 'e'
bool print_encoders(char select)
{
    const char comma = ',';

    if (select == 'a' or select == 0)
    {
        // the encoder sum is a measure of forward travel
        Serial.print(encoder_right_total + encoder_left_total);
        Serial.print(comma);
        Serial.print(robot_distance);
        Serial.print(comma);
        Serial.print(encoder_right_total - encoder_left_total);
        Serial.print(comma);
        Serial.println(robot_angle);
    }
    else if (select == 'r')
    {
        Serial.print(encoder_right_total + encoder_left_total);
        Serial.print(comma);
        Serial.println(encoder_right_total - encoder_left_total);
    }
    else if (select == 'u')
    {
        Serial.print(robot_distance);
        Serial.print(comma);
        Serial.println(robot_angle);
    }
    else
    {
        return false;
    }
    return true;
}
