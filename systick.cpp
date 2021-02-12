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
#include "digitalWriteFast.h"
#include <wiring_private.h>
#include <pins_arduino.h>

/***
 * Global variables
 */

volatile int raw_BatteryVolts_adcValue;
volatile float raw_BatteryVolts;
volatile int Switch_ADC_value;
const float batteryDividerRatio = 2.0f;

/***
 * Locals
*/
static uint8_t systick_phase = 0;

/***
 * NOTE: Manual analogue conversions
 * All channels will be converted by systick nut conversions must be 
 * performed manually. That is, AnalogueRead will block during a conversion
 * and that is not good
 */

static uint8_t analog_reference = DEFAULT;

void start_adc(uint8_t pin){

	if (pin >= 14) pin -= 14; // allow for channel or pin numbers
	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)
	ADMUX = (analog_reference << 6) | (pin & 0x07);
#endif
	// start the conversion
	sbi(ADCSRA, ADSC);
}


int get_adc_result(){
	// ADSC is cleared when the conversion finishes
	// while (bit_is_set(ADCSRA, ADSC));

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	uint8_t low  = ADCL;
	uint8_t high = ADCH;

	// combine the two bytes
	return (high << 8) | low;
}

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

  // set divisor to 32 => timer clock = 500kHz
  bitClear(TCCR2B, CS22);
  bitSet(TCCR2B, CS21);
  bitSet(TCCR2B, CS20);

  // set the timer frequency
  OCR2A = 24;              // (16000000/32/20000)-1 = 24
  bitSet(TIMSK2, OCIE2A);  // enable the timer interrupt
}

unsigned long t_systick1 = 0;
unsigned long t_systick2 = 0;
unsigned long t_systick3 = 0;


/***
 * This is the systick event - an ISR connected to Timer 2
 * It currently runs at 20kHz so that the emitter on duty
 * is low.
 * There are 40 possible time-slots, or phases, if the systick
 * is intended to be synchronous with the main control loop
 * running every 2 milliseconds. 
 * Each tick is called every 50us so that an analogue conversion 
 * started in one phase will be completed ready for the next phase. 
 * This technique avoids having the processor sitting idle
 * during conversions and while waiting for the sensors to respond.
 * So that high-speed encoder interrupts are not missed each phase
 * should ideally last no more than 10us.
 * 
 * The reason for this apparently arcane technique is that the 
 * ATMEGA328P does not have nested interrupts by default. At top speed, the 
 * encoders may generate interrupt at more than 20kHz and we cannot
 * afford to miss one. Also, received serial characters must be serviced
 * as they arrive or they will be lost. Having a single systick event
 * that does everything but takes many tens of microseconds risks
 * lost serial data and encoder pulses.
 *  
 * For testing, the built-in LED can be turned on at the start of the interrupt
 * and off again at the end. This can be used to measure system load.
 * Disable the feature if you want to use the built-in LED for some other 
 * purpose.
 * 
 */
ISR(TIMER2_COMPA_vect) {
  // digitalWriteFast(LED_BUILTIN, 1);
  systick_phase++;
  if (systick_phase >= 40) {
    systick_phase = 0;
  }
  switch (systick_phase) {
    case 0:
      // always start conversions as soon as  possible so they get a
      // full 50us to convert
      start_adc(BATTERY_VOLTS);
      delayMicroseconds(40);  // just a long marker for the oscilloscope.
      break;
    case 1:
      raw_BatteryVolts_adcValue = get_adc_result();
      start_adc(FUNCTION_PIN);
      // TODO find a magic voltage divider ratio that makes this a single multiply for millivolts
      raw_BatteryVolts = (raw_BatteryVolts_adcValue * 2 * 5) / 1024;
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
      if (emitter_on) {
        digitalWriteFast(EMITTER, 1);
      }
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
      if (emitter_on) {
        digitalWriteFast(EMITTER, 0);
      }
      break;
    default:
      break;
  }
  // digitalWriteFast(LED_BUILTIN, 0);
  /***
   * Speed control may now need to happen either in short sections here
   * or at the top level of the code. A flag, set in one phase
   * of systick could tell the higher level code that it is time to
   * update the controllers. Or they could just use the millis()
   * counter, or hey could get around to it as often as possible and
   * make use of the elapsed time in the calculations
   */
}

void print_bat()
{
    Serial.println(get_BatteryVolts(), floating_decimal_places);
    //Serial.println(F(" v"));
}
