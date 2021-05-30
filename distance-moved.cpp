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
#include "robot_config.h"
#include "stopwatch.h"
#include "distance-moved.h"
#include "interpreter.h"
#include <Arduino.h>
#include <util/atomic.h>

/*
   from ATMega328p datasheet section 12:
   The ATMega328p can generate interrupt as a result of changes of state on two of its pins:

   PD2 for INT0  - Arduino Digital Pin 2
   PD3 for INT1  - Arduino Digital Pin 3

   The INT0 and INT1 interrupts can be triggered by a falling or rising edge or a low level.
   This is set up as indicated in the specification for the External Interrupt Control Register A –
   EICRA.

   The External Interrupt 0 is activated by the external pin INT0 if the SREG I-flag and the
   corresponding interrupt mask are set. The level and edges on the external INT0 pin that activate
   the interrupt are defined as

   ISC01 ISC00  Description
     0     0    Low Level of INT0 generates interrupt
     0     1    Logical change of INT0 generates interrupt
     1     0    Falling Edge of INT0 generates interrupt
     1     1    Rising Edge of INT0 generates interrupt


   The External Interrupt 1 is activated by the external pin INT1 if the SREG I-flag and the
   corresponding interrupt mask are set. The level and edges on the external INT1 pin that activate
   the interrupt are defined in Table 12-1

   ISC11 ISC10  Description
     0     0    Low Level of INT1 generates interrupt
     0     1    Logical change of INT1 generates interrupt
     1     0    Falling Edge of INT1 generates interrupt
     1     1    Rising Edge of INT1 generates interrupt

   To enable these interrupts, bits must be set in the external interrupt mask register EIMSK

   EIMSK:INT0 (bit 0) enables the INT0 external interrupt
   EIMSK:INT1 (bit 1) enables the INT1 external interrupt

*/

/***
 * Variables
 *
 * The raw count values are retained because they do not suffer
 * from floating point error accumulation when calculating the total distance
 * and angle travelled.
 *
 * The encoders accumulate counts as the wheels turn.
 * The count for each wheel encoder changes frequently and means nothing without
 * an associated time interval.
 *
 *
 *
 */

// These are only used just in case we use for returning to the Pi Zero.
const float MM_PER_COUNT = (PI * WHEEL_DIAMETER) / (2 * ENCODER_PULSES_PER_ROTATION * GEAR_RATIO);
const float DEG_PER_COUNT = (360.0 * MM_PER_COUNT) / (PI * WHEEL_SEPARATION);

// These are the constants use in calculations
const float MM_PER_COUNT_LEFT = (1 - ROTATION_BIAS) * PI * WHEEL_DIAMETER / (ENCODER_PULSES_PER_ROTATION * GEAR_RATIO);
const float MM_PER_COUNT_RIGHT = (1 + ROTATION_BIAS) * PI * WHEEL_DIAMETER / (ENCODER_PULSES_PER_ROTATION * GEAR_RATIO);
const float DEG_PER_MM_DIFFERENCE = (180.0 / (2 * MOUSE_RADIUS * PI));

static volatile float s_robot_position;
static volatile float s_robot_angle;

static float s_robot_fwd_increment = 0;
static float s_robot_rot_increment = 0;

int encoder_left_counter;
int encoder_right_counter;

static volatile int32_t s_left_total;
static volatile int32_t s_right_total;

static volatile int left_delta;
static volatile int right_delta;

void reset_encoders()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        encoder_left_counter = 0;
        encoder_right_counter = 0;
        s_robot_position = 0;
        s_robot_angle = 0;
        s_left_total = 0;
        s_right_total = 0;
        left_delta = 0;
        right_delta = 0;
    }
}

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
        encoder_left_counter = 0;
        // right
        pinMode(ENCODER_RIGHT_CLK, INPUT);
        pinMode(ENCODER_RIGHT_B, INPUT);
        // configure the pin change
        bitClear(EICRA, ISC11);
        bitSet(EICRA, ISC10);
        // enable the interrupt
        bitSet(EIMSK, INT1);
        encoder_right_counter = 0;
    }
    reset_encoders();
}

// units are all in counts and counts per second
void update_encoders()
{

    // Make sure values don't change while being read. Be quick.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        left_delta = encoder_left_counter;
        right_delta = encoder_right_counter;
        encoder_left_counter = 0;
        encoder_right_counter = 0;
    }
    s_left_total += left_delta;
    s_right_total += right_delta;
    float left_change = left_delta * MM_PER_COUNT_LEFT;
    float right_change = right_delta * MM_PER_COUNT_RIGHT;
    s_robot_fwd_increment = 0.5 * (right_change + left_change);
    s_robot_rot_increment = (right_change - left_change) * DEG_PER_MM_DIFFERENCE;
    s_robot_position += s_robot_fwd_increment;
    s_robot_angle += s_robot_rot_increment;
}

float robot_position()
{
    float distance;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { distance = s_robot_position; }
    return distance;
}

float robot_fwd_increment()
{
    float distance;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { distance = s_robot_fwd_increment; }
    return distance;
}

float robot_rot_increment()
{
    float distance;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { distance = s_robot_rot_increment; }
    return distance;
}

float robot_angle()
{
    float angle;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { angle = s_robot_angle; }
    return angle;
}

uint32_t encoder_left_total()
{
    return s_left_total;
};

uint32_t encoder_right_total()
{
    return s_right_total;
};

// Interrupt called every time there is a change on the left encoder.
// INT0 will respond to the XOR-ed pulse train from the left encoder
// runs in constant time of around 3us per interrupt.
// would be faster with direct port access
ISR(INT0_vect)
{
    static bool oldA = false;
    static bool oldB = false;
    bool newB = digitalReadFast(ENCODER_LEFT_B);
    bool newA = digitalReadFast(ENCODER_LEFT_CLK) ^ newB;
    int delta = ENCODER_LEFT_POLARITY * ((oldA ^ newB) - (newA ^ oldB));
    encoder_left_counter += delta;
    oldA = newA;
    oldB = newB;
}

// Interrupt called every time there is a change on the right encoder.
// INT1 will respond to the XOR-ed pulse train from the right encoder
// runs in constant time of around 3us per interrupt.
// would be faster with direct port access
ISR(INT1_vect)
{
    static bool oldA = false;
    static bool oldB = false;
    bool newB = digitalReadFast(ENCODER_RIGHT_B);
    bool newA = digitalReadFast(ENCODER_RIGHT_CLK) ^ newB;
    int delta = ENCODER_RIGHT_POLARITY * ((oldA ^ newB) - (newA ^ oldB));
    encoder_right_counter += delta;
    oldA = newA;
    oldB = newB;
}

/******************************** command functions **************************/

// command 'r'
int8_t print_encoder_setup()
{
    const char comma = ',';

    Serial.print(MM_PER_COUNT, 5);
    Serial.print(comma);
    Serial.println(DEG_PER_COUNT, 5);
    return T_OK;
}

// command 'z'
int8_t zero_encoders()
{
    reset_encoders();
    return T_OK;
}

// command 'e'
bool print_encoders(char select)
{
    const char comma = ',';

    if (select == 'a' or select == 0)
    {
        // the encoder sum is a measure of forward travel
        Serial.print(s_right_total + s_left_total);
        Serial.print(comma);
        Serial.print(s_robot_position);
        Serial.print(comma);
        Serial.print(s_right_total - s_left_total);
        Serial.print(comma);
        Serial.println(s_robot_angle);
    }
    else if (select == 'r')
    {
        Serial.print(s_right_total + s_left_total);
        Serial.print(comma);
        Serial.println(s_right_total - s_left_total);
    }
    else if (select == 'u')
    {
        Serial.print(s_robot_position);
        Serial.print(comma);
        Serial.println(s_robot_angle);
    }
    else if (select == 's')
    {
        int fwd, rot;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            fwd = s_robot_fwd_increment;
            rot = s_robot_rot_increment;
        }
        float robot_velocity = LOOP_FREQUENCY * fwd;
        float robot_omega = LOOP_FREQUENCY * rot;

        Serial.print(robot_velocity);
        Serial.print(comma);
        Serial.println(robot_omega);
    }
    else
    {
        return false;
    }
    return true;
}
