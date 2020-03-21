#include <Arduino.h>
#include "public.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("\nHello from ukmarsey\n"));
  setupSystick();
}

void loop() {
    interpreter();
}
