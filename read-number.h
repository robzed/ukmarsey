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
#ifndef READ_NUMBER_H
#define READ_NUMBER_H

#include <Arduino.h>

/***
 * Numeric input parsers
 *
 * Each is provided with
 *
 *   line  - a pointer to a character array
 *   pos   - a pointer to a position in that character array
 *   value - a pointer to a variable of type float or integer
 *
 * The converters try to extract the value of a number starting at line[pos].
 * Conversion continues until a non-numeric character is found.
 * Numbers may have an optional leading '-' but no whitespace.
 *
 * On exit,
 *
 *  - the value variable is updated with teh converted number
 *  - pos is updated to point to the first character after the number
 *
 * The return value is the number of digits converted - excluding any leading '-'
 *
 * If no number is found, the function will
 *  - return zero
 *  - not change the value of pos
 *  - not change the contents of the variable 'value'
 *
 * There is no checking done to ensure the charactar array bounds are not
 * exceeded.
 *
 * Only the first MAX_DIGITS numerical characters will be converted though
 * scanning will continue until all numeric digits have been used.
 *
 * read_integer() will stop scanning at a decimal point.
 *
 * It is quite possible that only read_float() is really needed if it is
 * accepatble for ints to be assigned a truncated floating point value
 */

// floats have no more than 8 significant digits
#define MAX_DIGITS 8

uint8_t read_integer(const char *line, uint8_t *pos, int *value);
uint8_t read_float(const char *line, uint8_t *pos, float *value);

#endif