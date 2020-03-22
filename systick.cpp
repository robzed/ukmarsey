#include <Arduino.h>
#include "hardware_pins.h"
#include "public.h"

/***
 * Global variables
 */

float gBatteryVolts;
byte gFunctionSwitch;
byte gDipSwitch;
const float batteryDividerRatio = 2.0f;

/***/

void updateBatteryVolts() {
  int adcValue = analogRead(BATTERY_VOLTS);
  gBatteryVolts = adcValue * (5.0f * batteryDividerRatio / 1023.0f);
}

void updateFunctionSwitch() {
  /**
   * Typical ADC values for all function switch settings
   */
  const int adcReading[] = {660, 647, 630, 614, 590, 570, 545, 522, 461,
                            429, 385, 343, 271, 212, 128, 44,  0};

  int adcValue = analogRead(FUNCTION_PIN);
  if(adcValue > 1000){
    gFunctionSwitch = 16;  // pushbutton closed
    return;
  }
  for (int i = 0; i < 16; i++) {
    if (adcValue > (adcReading[i] + adcReading[i + 1]) / 2) {
      gFunctionSwitch = i;
      gDipSwitch = i;
      break;
    }
  }

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

// the systick event is an ISR attached to Timer 2
ISR(TIMER2_COMPA_vect) {
  updateBatteryVolts();
  updateFunctionSwitch();
  updateWallSensor();
}

void print_bat()
{
    Serial.print(F("Battery = "));
    Serial.println(gBatteryVolts);
}
