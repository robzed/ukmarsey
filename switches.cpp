/*
 * Switches. Read and test the on-board function switches and button

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

#include "switches.h"
#include "public.h"
#include <Arduino.h>

/** @brief  Convert the switch ADC reading into a switch reading.
 *  @return integer in range 0..16
 */
int readFunctionSwitch()
{
    const int adcReading[] = {660, 647, 630, 614, 590, 570, 545, 522, 461,
                              429, 385, 343, 271, 212, 128, 44, 0};

    if (Switch_ADC_value > 800)
    {
        return 16;
    }
    for (int i = 0; i < 16; i++)
    {
        if (Switch_ADC_value > (adcReading[i] + adcReading[i + 1]) / 2)
        {
            return i;
        }
    }
    // TODO: should there be a more informative error value?
    return 15; // should never get here... but if we do show 15
}

/** @brief  Test the user pushbutton
 *  There is no debouncing so take care
 *  @return boolean
 */
bool button_pressed()
{
    return readFunctionSwitch() == 16;
}

void wait_for_button_press()
{
    while (not button_pressed())
    {
        delay(10);
    }
}

void wait_for_button_release()
{
    while (button_pressed())
    {
        delay(10);
    }
}

void wait_for_button_click()
{
    wait_for_button_press();
    wait_for_button_release();
}
