/*
 * ukmarsey main file

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

#include "public.h"
#include "stopwatch.h"
#include <Arduino.h>

uint32_t loop_time_trigger;
uint8_t PoR_status = 0;
void setup()
{
    PoR_status = MCUSR; // is this erased by bootloader?
    MCUSR = 0;
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    Serial.println(F("Hello from ukmarsey"));
    setupSystick();
    //wall_sensors_setup();
    sensors_control_setup();
    setup_encoders();
    motorSetup();
    init_stored_parameters();
    loop_time_trigger += LOOP_INTERVAL_MS;
}

void loop()
{
    if (millis() > loop_time_trigger)
    {
        // this is where we will do all the controller calculations and updates
        loop_time_trigger += LOOP_INTERVAL_MS;
        Stopwatch sw;
        // setLeftMotorPWM(100);
        // setRightMotorPWM(100);
        update_encoders(); // (50us) surprisingly quick.
        sw.stop();
        // Serial.print(millis());
        // Serial.print(' ');
        // Serial.print(robot_velocity);
        // Serial.print(' ');
        // Serial.print(0);  // placeholder for controller voltage
        // Serial.print(' ');
        // Serial.print(split);
        // Serial.print(' ');
        // Serial.print(sw.elapsed_time());
        // Serial.println();
    }
    interpreter();
}
