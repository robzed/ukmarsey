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
 */

/***
 * The encoders accumulate counts as the wheels turn.
 * The count for each wheel encoder changes frequently and means nothing without
 * an associated time interval.
 *
 * At the beginning of each control loop interval, the counts are captured and
 * used to update the current speeds and distances.
 *
 */
volatile int encoder_left_count;    // Updated by pin change interrupts. Reset every loop interval.
volatile int encoder_right_count;   // Updated by pin change interrupts. Reset every loop interval.


/***
 * Raw count values are not normally used informative except when calibrating the
 * robot to calculate the counts MM_PER_COUNT and DEG_PER_COUNT constants.
 *
 * Even so, the raw count values are retained because they do not suffer
 * from floating point error accumulation when calculating the total distance
 * and angle travelled.
 */
int32_t encoderLeftTotal;
int32_t encoderRightTotal;


float robot_distance;   // mm
float robot_angle;      // degrees

// PID controller expects doubles. Arduino has double defined as float anyway
double robot_velocity;  // mm/s
double robot_omega;     // deg/s


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
    int leftCount;
    int rightCount;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        leftCount = encoder_left_count;
        rightCount = encoder_right_count;
        encoder_left_count = 0;
        encoder_right_count = 0;
    }

    encoderRightTotal += rightCount;
    encoderLeftTotal += leftCount;

    int encoderSum = rightCount + leftCount;
    robot_velocity = LOOP_FREQUENCY * MM_PER_COUNT * encoderSum;
    robot_distance = MM_PER_COUNT * (encoderRightTotal + encoderLeftTotal);

    int encoderDiff = rightCount - leftCount;
    robot_omega = LOOP_FREQUENCY * DEG_PER_COUNT * encoderDiff;
    robot_angle = DEG_PER_COUNT * (encoderRightTotal - encoderLeftTotal);
}


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

void print_encoder_setup()
{

    Serial.print(F("MM PER COUNT = "));
    Serial.println(MM_PER_COUNT, 5);

    Serial.print(F("So 500mm travel would be 500/"));
    Serial.print(MM_PER_COUNT, 5);
    Serial.print(F(" = "));
    Serial.print(500.0 / MM_PER_COUNT);
    Serial.println(F(" counts"));
    Serial.println();

    Serial.print(F("DEG PER COUNT = "));
    Serial.println(DEG_PER_COUNT, 5);

    Serial.print(F("So 360 degrees rotation would be 360/"));
    Serial.print(DEG_PER_COUNT, 5);
    Serial.print(F(" = "));
    Serial.print(360.0 / DEG_PER_COUNT);
    Serial.println(F(" counts"));
    Serial.println();
}

void zero_encoders()
{
    noInterrupts();
    encoderLeftTotal = 0;
    encoderRightTotal = 0;
    robot_distance = 0;
    robot_angle = 0;
    interrupts();
}

void print_encoders()
{
    Serial.print(F("EncoderSum: "));
    Serial.print(encoderRightTotal + encoderLeftTotal);
    Serial.print(F(" = "));
    Serial.print(robot_distance);
    Serial.print(F(" mm    "));

    Serial.print(F("EncoderDifference: "));
    Serial.print(encoderRightTotal - encoderLeftTotal);
    Serial.print(F(" = "));
    Serial.print(robot_angle);
    Serial.print(F(" deg"));

    Serial.println();
}
