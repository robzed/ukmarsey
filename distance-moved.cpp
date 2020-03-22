#include <Arduino.h>
#include "digitalWriteFast.h"
#include "hardware_pins.h"
#include "public.h"

/***
 * Global robot characteristic constants
 */
const int COUNTS_PER_ROTATION = 12;
const float GEAR_RATIO = 19.5;
const float WHEEL_DIAMETER = 32.5f;
const float WHEEL_SEPARATION = 75.2;

const float MM_PER_COUNT = (PI * WHEEL_DIAMETER) / (2 * COUNTS_PER_ROTATION * GEAR_RATIO);
const float DEG_PER_COUNT = (360.0 * MM_PER_COUNT) / (PI * WHEEL_SEPARATION);

/***
 * Global variables
 */
volatile int32_t encoderLeftCount;
volatile int32_t encoderRightCount;
int32_t encoderSum;
int32_t encoderDifference;


void setupEncoders() {
  // left
  pinMode(ENCODER_LEFT_CLK, INPUT);
  pinMode(ENCODER_LEFT_B, INPUT);
  // configure the pin change
  bitClear(EICRA, ISC01);
  bitSet(EICRA, ISC00);
  // enable the interrupt
  bitSet(EIMSK, INT0);
  encoderLeftCount = 0;
  // right
  pinMode(ENCODER_RIGHT_CLK, INPUT);
  pinMode(ENCODER_RIGHT_B, INPUT);
  // configure the pin change
  bitClear(EICRA, ISC11);
  bitSet(EICRA, ISC10);
  // enable the interrupt
  bitSet(EIMSK, INT1);
  encoderRightCount = 0;
}

ISR(INT0_vect) {
  static bool oldB = 0;
  bool newB = bool(digitalReadFast(ENCODER_LEFT_B));
  bool newA = bool(digitalReadFast(ENCODER_LEFT_CLK)) ^ newB;
  if (newA == oldB) {
    encoderLeftCount--;
  } else {
    encoderLeftCount++;
  }
  oldB = newB;
}

ISR(INT1_vect) {
  static bool oldB = 0;
  bool newB = bool(digitalReadFast(ENCODER_RIGHT_B));
  bool newA = bool(digitalReadFast(ENCODER_RIGHT_CLK)) ^ newB;
  if (newA == oldB) {
    encoderRightCount++;
  } else {
    encoderRightCount--;
  }
  oldB = newB;
}


void print_encoder_setup() {

  Serial.print(F("MM PER COUNT = "));
  Serial.println(MM_PER_COUNT, 5);

  Serial.print(F("So 500mm travel would be 500/"));
  Serial.print(MM_PER_COUNT, 5);
  Serial.print(F(" = "));
  Serial.print(500.0/MM_PER_COUNT);
  Serial.println(F(" counts"));
  Serial.println();

  Serial.print(F("DEG PER COUNT = "));
  Serial.println(DEG_PER_COUNT, 5);

  Serial.print(F("So 360 degrees rotation would be 360/"));
  Serial.print(DEG_PER_COUNT, 5);
  Serial.print(F(" = "));
  Serial.print(360.0/DEG_PER_COUNT);
  Serial.println(F(" counts"));
  Serial.println();
}


void zero_encoders()
{
  encoderLeftCount = 0;
  encoderRightCount = 0;
}

void print_encoders()
{
    encoderSum = encoderRightCount + encoderLeftCount;
    encoderDifference = encoderRightCount - encoderLeftCount;
    float distance = MM_PER_COUNT * encoderSum;
    float angle = DEG_PER_COUNT * encoderDifference;

    Serial.print(F("EncoderSum: "));
    Serial.print(encoderSum);
    Serial.print(F(" = "));
    Serial.print(distance);
    Serial.print(F(" mm    "));

    Serial.print(F("EncoderDifference: "));
    Serial.print(encoderDifference);
    Serial.print(F(" = "));
    Serial.print(angle);
    Serial.print(F(" deg"));

    Serial.println();
}
