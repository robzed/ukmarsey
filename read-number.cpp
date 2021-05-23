/*
 * File: read-number.cpp
 * Project: ukmarsey
 * File Created: Thursday, 4th March 2021 2:36:40 pm
 *
 *   ukmarsey is a machine and human command-based Robot Low-level I/O platform initially targetting UKMARSBot
 *   For more information see:
 *       https://github.com/robzed/ukmarsey
 *       https://ukmars.org/
 *       https://github.com/ukmars/ukmarsbot
 *       https://github.com/robzed/pizero_for_ukmarsbot
 *
 *  MIT License
 *
 *  Copyright (c) 2020-2021 Rob Probin & Peter Harrison
 *  Copyright (c) 2019-2021 UK Micromouse and Robotics Society
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
*/

#include "read-number.h"
/////////////////////////////////////////////////////////////////////////////

/***
 * Scan a character array for an integer.
 * Begin scn at line[pos]
 * Assumes no leading spaces.
 * Stops at first non-digit.
 * MODIFIES pos so that it points to the first non-digit
 * MODIFIES value ONLY IF a valid integer is converted
 * RETURNS  boolean status indicating success or error
 *
 * optimisations are possible but may not be worth the effort
 */
uint8_t read_integer(const char *line, uint8_t *pos, int *value)
{
    char *ptr = (char *)line + *pos;
    char c = *ptr++;
    bool is_minus = false;
    uint8_t digits = 0;
    if (c == '-')
    {
        is_minus = true;
        c = *ptr++;
    }
    int32_t number = 0;
    while (c >= '0' and c <= '9')
    {
        if (digits++ < MAX_DIGITS)
        {
            number = 10 * number + (c - '0');
        }
        c = *ptr++;
    }
    *pos = ptr - line - 1;
    if (digits > 0)
    {

        *value = is_minus ? -number : number;
    }
    return digits;
}
/////////////////////////////////////////////////////////////////////////////

/***
 * Scan a character array for a float.
 * This is a much simplified and limited version of the library function atof()
 * It will not convert exponents and has a limited range of valid values.
 * They should be more than adequate for the robot parameters however.
 * Begin scan at line[pos]
 * Assumes no leading spaces.
 * Only scans MAX_DIGITS characters
 * Stops at first non-digit, or decimal point.
 * MODIFIES pos so that it points to the first character after the number
 * MODIFIES value ONLY IF a valid float is converted
 * RETURNS  boolean status indicating success or error
 *
 * optimisations are possible but may not be worth the effort
 */
uint8_t read_float(const char *line, uint8_t *pos, float *value)
{

    char *ptr = (char *)line + *pos;
    char c = *ptr++;
    uint8_t digits = 0;

    bool is_minus = false;
    if (c == '-')
    {
        is_minus = true;
        c = *ptr++;
    }

    uint32_t a = 0.0;
    int exponent = 0;
    while (c >= '0' and c <= '9')
    {
        if (digits++ < MAX_DIGITS)
        {
            a = a * 10 + (c - '0');
        }
        c = *ptr++;
    };
    if (c == '.')
    {
        c = *ptr++;
        while (c >= '0' and c <= '9')
        {
            if (digits++ < MAX_DIGITS)
            {
                a = a * 10 + (c - '0');
                exponent = exponent - 1;
            }
            c = *ptr++;
        }
    }

    *pos = ptr - line - 1;
    float b = a;
    while (exponent < 0)
    {
        b *= 0.1;
        exponent++;
    }
    if (digits > 0)
    {
        *value = is_minus ? -b : b;
    }
    return digits;
}
