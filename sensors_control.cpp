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
#include "digitalWriteFast.h"
#include "hardware_pins.h"
#include "sensors_control.h"
#include <Arduino.h>
#include <util/atomic.h>
#include <wiring_private.h>

/***
 * Global variables
 */
// the current value of the sensors

volatile int gSensorA0_dark;
volatile int gSensorA1_dark;
volatile int gSensorA2_dark;
volatile int gSensorA3_dark;
volatile int gSensorA4_dark;
volatile int gSensorA5_dark;

volatile int gSensorA0_light;
volatile int gSensorA1_light;
volatile int gSensorA2_light;
volatile int gSensorA3_light;
volatile int gSensorA4_light;
volatile int gSensorA5_light;

/***
 * Global variables
 */

volatile int raw_BatteryVolts_adcValue;
volatile float battery_voltage;
volatile float g_battery_scale;

volatile int Switch_ADC_value;
const float batteryDividerRatio = 2.0f;

volatile float g_steering_adjustment;

/** @brief change the ADC prescaler to give a suitable conversion rate
 *
 * The default for the Arduino is to give a slow ADC clock for maximum
 * SNR in the results. That typically beans a prescale value of 128
 * for the 16MHz ATMEGA328P running at 16MHz. Conversions then take more
 * than 100us to complete. In this application, we want to be able to
 * perform about 16 conversions in around 500us. To do that the prescaler
 * is reduced to a value of 32. This gives an ADC clock speed of
 * 500kHz and a single conversion in around 26us. SNR is still pretty good
 * at these speeds:
 * http://www.openmusiclabs.com/learning/digital/atmega-adc/
 */
void analogueSetup()
{
    // increase speed of ADC conversions to 28us each
    // by changing the clock prescaler from 128 to 32
    // to give a 500kHz clock
    bitSet(ADCSRA, ADPS2);
    bitClear(ADCSRA, ADPS1);
    bitSet(ADCSRA, ADPS0);
}

/*
 * Update battery voltage calculates the battery voltage and also
 * the scale factor used in the motor control
 *
 */
void update_battery_voltage()
{
    battery_voltage = raw_BatteryVolts_adcValue * (2.0 * 5.0 / 1024.0);
    g_battery_scale = 255.0 / battery_voltage;
}

/***
 * Locals
*/

/***
 * NOTE: Manual analogue conversions
 * All eight available ADC channels are aatomatically converted
 * by the sensor interrupt. Attempting to performa a manual ADC
 * conversion with the Arduino AnalogueIn() function will disrupt
 * that process so avoid doing that.
 */

static const uint8_t ADC_REF = DEFAULT;

static void start_adc(uint8_t pin)
{

    if (pin >= 14)
        pin -= 14; // allow for channel or pin numbers
                   // set the analog reference (high two bits of ADMUX) and select the
                   // channel (low 4 bits).  Result is right-adjusted
#if defined(ADMUX)
    ADMUX = (ADC_REF << 6) | (pin & 0x07);
#endif
    // start the conversion
    sbi(ADCSRA, ADSC);
}

static int get_adc_result()
{
    // ADSC is cleared when the conversion finishes
    // while (bit_is_set(ADCSRA, ADSC));

    // we have to read ADCL first; doing so locks both ADCL
    // and ADCH until ADCH is read.  reading ADCL second would
    // cause the results of each conversion to be discarded,
    // as ADCL and ADCH would be locked when it completed.
    uint8_t low = ADCL;
    uint8_t high = ADCH;

    // combine the two bytes
    return (high << 8) | low;
}

/** @brief  Read the raw switch reading
 *  @return void
 */
void updateFunctionSwitch()
{
    /**
   * Typical ADC values for all function switch settings
   */
    Switch_ADC_value = analogRead(FUNCTION_PIN);
}

static uint8_t private_emitter_on = 1;

void emitter_on(bool state)
{
    private_emitter_on = state;
    // turn off led
    digitalWriteFast(EMITTER, 0); // make sure LED is off - otherwise we could have a burnt out LED
}

void sensors_control_setup()
{
    pinMode(EMITTER, OUTPUT);
    digitalWriteFast(EMITTER, 0); // be sure the emitter is off
    analogueSetup();              // increase the ADC conversion speed
}

static uint8_t sensor_phase = 0;

void start_sensor_cycle()
{
    sensor_phase = 0;     // sync up the start of the sensor sequence
    bitSet(ADCSRA, ADIE); // enable the ADC interrupt
    start_adc(0);         // begin a conversion to get things started
}

void print_hex2(int value)
{
    value >>= 2; // get rid of button 2 bits - probably noise
    value = constrain(value, 0, 255);
    Serial.print(value / 16, HEX);
    Serial.print(value % 16, HEX);
}

void print_sensors_control(char mode)
{
    int a0_dark;
    int a1_dark;
    int a2_dark;
    int a3_dark;
    int a4_dark;
    int a5_dark;
    int a0_lit;
    int a1_lit;
    int a2_lit;
    int a3_lit;
    int a4_lit;
    int a5_lit;
    const char comma = ',';

    // read the sensors
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        a0_dark = gSensorA0_dark;
        a0_lit = gSensorA0_light;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        a1_dark = gSensorA1_dark;
        a1_lit = gSensorA1_light;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        a2_dark = gSensorA2_dark;
        a2_lit = gSensorA2_light;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        a3_dark = gSensorA3_dark;
        a3_lit = gSensorA3_light;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        a4_dark = gSensorA4_dark;
        a4_lit = gSensorA4_light;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        a5_dark = gSensorA5_dark;
        a5_lit = gSensorA5_light;
    }

    if (mode == 'd')
    { // the default is decimal differences
        Serial.print(max(a0_lit - a0_dark, 0));
        Serial.print(comma);
        Serial.print(max(a1_lit - a1_dark, 0));
        Serial.print(comma);
        Serial.print(max(a2_lit - a2_dark, 0));
        Serial.print(comma);
        Serial.print(max(a3_lit - a3_dark, 0));
        Serial.print(comma);
        Serial.print(max(a4_lit - a4_dark, 0));
        Serial.print(comma);
        Serial.print(max(a5_lit - a5_dark, 0));
    }
    else if (mode == 'h')
    { // display differences as hex values
        print_hex2(a0_lit - a0_dark);
        print_hex2(a1_lit - a1_dark);
        print_hex2(a2_lit - a2_dark);
        print_hex2(a3_lit - a3_dark);
        print_hex2(a4_lit - a4_dark);
        print_hex2(a5_lit - a5_dark);
    }
    else if (mode == 'r')
    { // display both dark and lit values
        Serial.print(a0_dark);
        Serial.print(comma);
        Serial.print(a1_dark);
        Serial.print(comma);
        Serial.print(a2_dark);
        Serial.print(comma);
        Serial.print(a3_dark);
        Serial.print(comma);
        Serial.print(a4_dark);
        Serial.print(comma);
        Serial.print(a5_dark);
        Serial.print(comma);
        Serial.print(' ');
        Serial.print(' ');
        Serial.print(a0_lit);
        Serial.print(comma);
        Serial.print(a1_lit);
        Serial.print(comma);
        Serial.print(a2_lit);
        Serial.print(comma);
        Serial.print(a3_lit);
        Serial.print(comma);
        Serial.print(a4_lit);
        Serial.print(comma);
        Serial.print(a5_lit);
    }
    Serial.println();
}

/** @brief Sample all the sensor channels with and without the emitter on
 *
 * At the end of the 500Hz systick interrupt, the ADC interrupt is enabled
 * and a conversion started. After each ADC conversion the interrupt gets
 * generated and this ISR is called. The eight channels are read in turn with
 * the sensor emitter(s) off.
 * At the end of that sequence, the emiter(s) get turned on and a dummy ADC
 * conversion is started to provide a delay while the sensors respond.
 * After that, all channels are read again to get the lit values.
 * After all the channels have been read twice, the ADC interrupt is disabbled
 * and the sensors are idle until triggered again.
 *
 * There are actually 16 available channels and channel 8 is the internal
 * temperature sensor. Channel 15 is Gnd. If appropriate, a read of channel
 * 15 can be used to zero the ADC sample and hold capacitor.
 */
ISR(ADC_vect)
{
    // digitalWriteFast(13, 1);
    switch (sensor_phase)
    {
        case 0:
            // always start conversions as soon as  possible so they get a
            // full 50us to convert
            start_adc(BATTERY_VOLTS);
            break;
        case 1:
            raw_BatteryVolts_adcValue = get_adc_result();
            start_adc(FUNCTION_PIN);
            break;
        case 2:
            Switch_ADC_value = get_adc_result();
            start_adc(A0);
            break;
        case 3:
            gSensorA0_dark = get_adc_result();
            start_adc(A1);
            break;
        case 4:
            gSensorA1_dark = get_adc_result();
            start_adc(A2);
            break;
        case 5:
            gSensorA2_dark = get_adc_result();
            start_adc(A3);
            break;
        case 6:
            gSensorA3_dark = get_adc_result();
            start_adc(A4);
            break;
        case 7:
            gSensorA4_dark = get_adc_result();
            start_adc(A5);
            break;
        case 8:
            gSensorA5_dark = get_adc_result();
            // got all the dark ones so light them up
            if (private_emitter_on)
            {
                digitalWriteFast(EMITTER, 1);
            }
            start_adc(A7); // dummy read of the battery to provide delay
            // wait at least one cycle for the detectors to respond
            break;
        case 9:
            start_adc(A0);
            break;
        case 10:
            gSensorA0_light = get_adc_result();
            start_adc(A1);
            break;
        case 11:
            gSensorA1_light = get_adc_result();
            start_adc(A2);
            break;
        case 12:
            gSensorA2_light = get_adc_result();
            start_adc(A3);
            break;
        case 13:
            gSensorA3_light = get_adc_result();
            start_adc(A4);
            break;
        case 14:
            gSensorA4_light = get_adc_result();
            start_adc(A5);
            break;
        case 15:
            gSensorA5_light = get_adc_result();
            if (private_emitter_on)
            {
                digitalWriteFast(EMITTER, 0);
            }
            bitClear(ADCSRA, ADIE);
            break;
        default:
            break;
    }
    sensor_phase++;
    // digitalWriteFast(13, 0);
}
