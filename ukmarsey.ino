#include <Arduino.h>
#include "public.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("\nHello from ukmarsey"));
  setupSystick();
  //wall_sensors_setup();
  sensors_control_setup();
  setupEncoders();
  motorSetup();
  init_stored_parameters();
}


//extern unsigned long t_systick1;
//extern unsigned long t_systick2;
//extern unsigned long t_systick3;

//unsigned int next_time = 0;

void loop() {

  /*
  if(millis() >= next_time)
  {
      delay(100);
      Serial.println(Serial.availableForWrite());
    Serial.println();
    Serial.print("bat read time=");
    Serial.print(t_systick1);
    Serial.println("µs");
    Serial.print("switch read time=");
    Serial.print(t_systick2);
    Serial.println("µs");
    Serial.print("wall sensor read time=");
    Serial.print(t_systick3);
    Serial.println("µs");

    unsigned long _start = micros();
    unsigned long timing_timing = micros() - _start;
    Serial.print("timing timing =");
    Serial.print(timing_timing);
    Serial.println(" µs");
    next_time = millis() + 2000;  // every 2 seconds
  }
  */

    
    interpreter();
}



 
