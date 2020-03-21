#include <Arduino.h>
#include "public.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("\nHello from ukmarsey\n"));
  setupSystick();
  
  /*
  setupEncoder();
  Serial.println(F("Hello\n"));
  updateTime = millis() + updateInterval;
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
  Serial.println(F("Press the user button to continue..."));
  while(getFunctionSwitch() != 16){
    // do nothing until the pushbutton is pressed
  }
  Serial.println();
  
  */
}


void loop() {
    interpreter();
/*
  if (millis() > updateTime) {
    updateTime += updateInterval;
    if(getFunctionSwitch() == 16){
        encoderLeftCount = 0;
        encoderRightCount = 0;
    }
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
  
  */
}
