/*
 * interpreter.h - command line interpreter

   ukmarsey is a machine and human command-based Robot Low-level I/O platform initially targetting UKMARSBot
   For more information see:
       https://github.com/robzed/ukmarsey
       https://ukmars.org/
       https://github.com/ukmars/ukmarsbot
       https://github.com/robzed/pizero_for_ukmarsbot

  MIT License

  Copyright (c) 2021 Rob Probin & Peter Harrison
  Copyright (c) 2021 UK Micromouse and Robotics Society

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

 *  Created on: 24 Feb 2021
 *      Author: Rob Probin
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#define MAX_INPUT_SIZE 14
extern char inputString[MAX_INPUT_SIZE]; // a String to hold incoming data
extern int inputIndex;                   // where we are on the input
void interpreter();

int decode_input_value(int index);

// These are the error codes produced by commands to pass into interpreter error.
enum
{
    T_SILENT_ERROR = -1, // Special error code designed for ?, h and unimplemented commands that report their own errors.
    T_OK = 0,            // Normal 'no error' return code.
    T_OUT_OF_RANGE = 1,
    T_READ_NOT_SUPPORTED = 2,
    T_LINE_TOO_LONG = 3,
    T_UNKNOWN_COMMAND = 4,
    T_UNEXPECTED_TOKEN = 5
};

#endif /* INTERPRETER_H_ */
