/*
 * interpreter - provides quick and low-bandwith serial command line interpreter
 * to access all features of the hardware, and allow a secondary processor (e.g.
 * Raspberry Pi Zero) to coordinate robot actions.

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
#include <Arduino.h>
#include "digitalWriteFast.h"
#include "public.h"
#include <EEPROM.h>

/*
 * Small command line interpreter
 */

#define MAX_INPUT_SIZE 14
static char inputString[MAX_INPUT_SIZE];         // a String to hold incoming data
static int inputIndex = 0;  // where we are on the input
static bool interpreter_echo = true;


//
// There are (NUM_STORED_PARAMS+2) * 4 bytes stored in EEPROM. NOTE: The last one is a magic number to detect an uninitialised EEPROM.
//
#define NUM_STORED_PARAMS 16

float stored_params[NUM_STORED_PARAMS];
static const float stored_parameters_default_values[NUM_STORED_PARAMS] = {

        // Index: Usage
  0.0F, //  0: undefined
  0.0F, //  1: undefined
  0.0F, //  2: undefined
  0.0F, //  3: undefined

  0.0F, //  4: undefined
  0.0F, //  5: undefined
  0.0F, //  6: undefined
  0.0F, //  7: undefined

  0.0F, //  8: undefined
  0.0F, //  9: undefined
  0.0F, // 10: undefined
  0.0F, // 11: undefined

  0.0F, // 12: undefined
  0.0F, // 13: undefined
  0.0F, // 14: undefined
  0.0F, // 15: undefined

};

#define NUMBER_OF_BITFIELD_STORED_PARAMS 32
static const uint32_t bitfield_default_values =
// Default 0UL/1UL  Index: Usage
  (0UL << 31) +  // 131: undefined
  (0UL << 30) +  // 130: undefined
  (0UL << 29) +  // 129: undefined
  (0UL << 28) +  // 128: undefined
  (0UL << 27) +  // 127: undefined
  (0UL << 26) +  // 126: undefined
  (0UL << 25) +  // 125: undefined
  (0UL << 24) +  // 124: undefined

  (0UL << 23) +  // 123: undefined
  (0UL << 22) +  // 122: undefined
  (0UL << 21) +  // 121: undefined
  (0UL << 20) +  // 120: undefined
  (0UL << 19) +  // 119: undefined
  (0UL << 18) +  // 118: undefined
  (0UL << 17) +  // 117: undefined
  (0UL << 16) +  // 116: undefined

  (0UL << 15) +  // 115: undefined
  (0UL << 14) +  // 114: undefined
  (0UL << 13) +  // 113: undefined
  (0UL << 12) +  // 112: undefined
  (0UL << 11) +  // 111: undefined
  (0UL << 10) +  // 110: undefined
  (0UL <<  9) +  // 109: undefined
  (0UL <<  8) +  // 108: undefined

  (0UL <<  7) +  // 107: undefined
  (0UL <<  6) +  // 106: undefined
  (0UL <<  5) +  // 105: undefined
  (0UL <<  4) +  // 104: undefined
  (0UL <<  3) +  // 103: undefined
  (0UL <<  2) +  // 102: undefined
  (0UL <<  1) +  // 101: undefined
  (0UL      ) ;  // 100: undefined
//
// actual data storage
//
uint32_t bitfield_stored_params = bitfield_default_values;

// Addresses
#define BITFIELD_ADDRESS ((NUM_STORED_PARAMS+1)*4)
#define MAGIC_ADDRESS (BITFIELD_ADDRESS+sizeof(bitfield_stored_params))

//
// Version number for parameter configuration in EEPROM
//
// If the parameter configuration in EEPROM changes, increase this number
// and new versions will get a clean set of defaults.
#define PARAMETER_EEPROM_VERSION 1

// Magic to detect uninitialised space
#define MAGIC_NUMBER ((0x00CAFE00) ^ (PARAMETER_EEPROM_VERSION))


/** @brief  Access a stored parameter
 *  @param  Index of parameter
 *  @return stored_param, or 0 if that parameter doesn't exist.
 */
float get_float_param(int param_index)
{
  if(param_index < 0 or param_index > NUM_STORED_PARAMS) { return 0; }
  return stored_params[param_index];
}

/** @brief  Access a stored bool parameter
 *  @param  Index of parameter
 *  @return stored_param, or false if that parameter doesn't exist.
 */
bool get_bool_param(int param_index)
{
  if(param_index < 100 or param_index > NUMBER_OF_BITFIELD_STORED_PARAMS) { return false; }
  return bitfield_stored_params & (1<<(param_index-100));
}

// ------------------------------------------
enum
{
  T_OK = 0,
  T_OUT_OF_RANGE = 1,
  T_READ_NOT_SUPPORTED = 2,
  T_LINE_TOO_LONG = 3,
  T_UNKNOWN_COMMAND = 4,
  T_UNEXPECTED_TOKEN = 5
};
bool verbose_errors = true;

/** @brief  Print the interpreter error
 *  @param  error to be printed
 *  @return void
 */
void interpreter_error(int error, char* extra=0)
{
  if(verbose_errors)
  {
    if (error != T_OK)
    {
      Serial.print("@Error:");
    }
    switch(error)
    {
      case T_OK:
        Serial.println(F("OK"));
        break;
      case T_OUT_OF_RANGE:
        Serial.println(F("Out of range"));
        break;
      case T_READ_NOT_SUPPORTED:
        Serial.println(F("Read not supported"));
        break;
      case T_LINE_TOO_LONG:
        Serial.println(F("Too long"));
        break;
      case T_UNKNOWN_COMMAND:
        Serial.print(F("Unknown "));
        if(extra) {
          Serial.println(extra);
        }
        else
        {
          Serial.println();
        }
        break;
      case T_UNEXPECTED_TOKEN:
        Serial.println(F("Unexpected"));
        break;
      default:
        Serial.println(F("Error"));
        break;
    }
  }
  else
  {
    Serial.print("@Error:");
    Serial.println(error);
  }
}

typedef struct {
    const char *nam;
    void (*func)();
} entry_t;

//
// functions to run commands
//

/** @brief  Turns on and off the fixed LED
 *  @param
 *  @return void
 */
void led() { digitalWriteFast(LED_BUILTIN, (inputString[1] == '0') ? LOW:HIGH); }

/** @brief  Prints OK
 *  @param
 *  @return void
 */
void ok() { interpreter_error(T_OK); }


/** @brief  Reset the robot state to a known value
 *          NOTE: Does not reset $ parameters.
 *  @param
 *  @return void
 */
void reset_state() {
  char function = inputString[1];
  if(function == '^')
  {
    void(* resetFunc) (void) = 0;  // declare reset fuction at address 0
    resetFunc();
  }
  else if(function == '?')
  {
      extern uint8_t PoR_status;
      Serial.println(PoR_status);
  }
  else
  {
    // We should reset all state here. At the moment there isn't any.
    Serial.println(F("RST"));
    // Reset the actual state
  }
}


/** @brief  Show version of the program.
 *  @param
 *  @return void
 */
void show_version() { Serial.println(F("v1.3")); }


/** @brief  Print the decoded switch value.
 *  @param
 *  @return void
 */
void print_switches() { Serial.println(readFunctionSwitch()); }


/** @brief  Select one of several hardware motor tests.
 *  @param
 *  @return void
 */
void motor_test() {

  char function = inputString[1];

  switch (function) {
    case '0':
      setMotorVolts(0, 0);
      Serial.println(F("motors off"));
      break;
    case '1':
      setMotorVolts(1.5, 1.5);
      Serial.println(F("forward 25%"));
      break;
    case '2':
      setMotorVolts(3.0, 3.0);
      Serial.println(F("forward 50%"));
      break;
    case '3':
      setMotorVolts(4.5, 4.5);
      Serial.println(F("forward 75%"));
      break;
    case '4':
      setMotorVolts(-1.5, -1.5);
      Serial.println(F("reverse 25%"));
      break;
    case '5':
      setMotorVolts(-3.0, -3.0);
      Serial.println(F("reverse 50%"));
      break;
    case '6':
      setMotorVolts(-4.5, -4.5);
      Serial.println(F("reverse 75%"));
      break;
    case '7':
      setMotorVolts(-1.5, 1.5);
      Serial.println(F("spin left 25%"));
      break;
    case '8':
      setMotorVolts(-3.0, 3.0);
      Serial.println(F("spin left 50%"));
      break;
    case '9':
      setMotorVolts(1.5, -1.5);
      Serial.println(F("spin right 25%"));
      break;
    case 'a':
      setMotorVolts(3.0, -3.0);
      Serial.println(F("spin right 50%"));
      break;
    case 'b':
      setMotorVolts(0, 1.5);
      Serial.println(F("pivot left 25%"));
      break;
    case 'c':
      setMotorVolts(1.5, 0);
      Serial.println(F("pivot right 25%"));
      break;
    case 'd':
      setMotorVolts(1.5, 3.0);
      Serial.println(F("curve left"));
      break;
    case 'e':
      setMotorVolts(3.0, 1.5);
      Serial.println(F("curve right"));
      break;
    case 'f':
      setMotorVolts(4.5, 3.0);
      Serial.println(F("big curve right"));
      break;
    default:
      setMotorVolts(0, 0);
      break;
  }

  uint32_t endTime = millis() + 2000;
  while (endTime > millis()) {
    if (readFunctionSwitch() == 16) {
      break;  // stop running if the button is pressed
    }
  }
  // be sure to turn off the motors
  setMotorVolts(0, 0);


}

//int numeric_mode = 10;
//const char[] = "0123456789ABCDEF";    // support just upper case hex? or lower case as well?

/** @brief  Decodes a unsigned int from the input line
 *  @param  index of where in the input should be parsed
 *  @return int value of string
 *          Also alters inputIndex to past float
 */
// Decode a three digit decimal number, e.g. from 0 to 999
// -1 means invalid value
int decode_input_value(int index)
{
  int n = inputString[index++]-'0';
  if(n < 0 or n >= 10)
  {
    inputIndex = index-1;
    return -1;
  }
  while(true)
  {
    int n2 = inputString[index]-'0';
    if(n2 < 0 or n2 >= 10)
    {
      break;
    }
    n = n*10 + n2;
    index++;
  }
  inputIndex = index;
  return n;
}

/** @brief  Decodes a signed int from the input line
 *  @param  index of where in the input should be parsed
 *  @return int value of string
 *          Also alters inputIndex to past float
 */
int decode_input_value_signed(int index)
{
  if(inputString[index]=='-')
  {
    int value = decode_input_value(index+1);
    if(value < 0)
    {
      return 0;
    }
    return -value;
  }
  else
  {
    int value = decode_input_value(index);
    if(value < 0)
    {
      return 0;
    }
    return value;
  }
}


/** @brief  Decodes a fraction float (past decimal point) from the input line
 *  @param  index of where in the input should be parsed
 *  @param  n value of integer part of float.
 *  @return float value of string
 *          Also alters inputIndex to past float
 */
float fractional_float(int index, float n)
{
  float frac = 0.1F;
  while(true)
  {
    int digit = inputString[index]-'0';
    if(digit < 0 or digit > 9)
    {
      break;
    }
    n += digit * frac;
    frac *= 0.1F;
    index++;
  }
  inputIndex = index;
  return n;
}

/** @brief  Decodes a float from the input line
 *  @param  index of where in the input should be parsed
 *  @return float value of string
 *          Also alters inputIndex to past float
 */
// If invalid, return 0;
// Differnt to atoi, etc., because updates index.
float decode_input_value_float_unsigned(int index)
{
  float n = 0.0F;
  while(true)
  {
    int digit = inputString[index];
    if(digit < '0' or digit > '9')
    {
      if(digit == '.') // decimal point
      {
        index++;
        return fractional_float(index, n);
      }
      break;
    }
    n = n*10 + digit-'0';
    index++;
  }
  inputIndex = index;
  return n;
}

/** @brief  Parses a float from the input line
 *  @param  index of where in the input should be parsed
 *  @return float value of string
 *          Also alters inputIndex to past float
 */
float decode_input_value_float(int index)
{
  if(inputString[index]=='-')
  {
    float value = decode_input_value_float_unsigned(index+1);
    if(value>0)
    {
      return -value;
    }
    return -1;
  }
  else
  {
    return decode_input_value_float_unsigned(index);
  }
}

/** @brief Reads or writes a digital GPIO
 *  @return Void.
 */
void digital_pin_control()
{
  // D3=1
  // D13=0
  // Ignore spaces?

  int port = decode_input_value(1);
  if(port >= 0)
  {
    if(inputString[inputIndex] == '=')
    {
      // write port
      //
      // Could be: digitWrite(port, inputString[inputIndex+1]-'0')
      if(inputString[inputIndex+1] == '1')
      {
        digitalWriteFast(port, HIGH);
      }
      else
      {
        digitalWriteFast(port, LOW);
      }
    }
    else // read port
    {
      Serial.println(digitalReadFast(port));
    }
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}

static volatile int* pointers_to_ADC_readings[] =
{
  &gSensorA0_dark, // SENSOR_RIGHT_MARK = A0;
  &gSensorA1_dark, // SENSOR_1 = A1;
  &gSensorA2_dark, // SENSOR_2 = A2;
  &gSensorA3_dark, // SENSOR_3 = A3;
  &gSensorA4_light, // SENSOR_4 = A4;
  &gSensorA5_light, // SENSOR_LEFT_MARK = A5;
  &Switch_ADC_value, // FUNCTION_PIN = A6;
  &raw_BatteryVolts_adcValue, // BATTERY_VOLTS = A7;
};


/** @brief Reads an analogue pin or sets a PWM output.
 *  @return Void.
 */
void analogue_control()
{
  // A2
  // A9=255
  int port = decode_input_value(1);
  if(port >= 0)
  {
    if(inputString[inputIndex] == '=')
    {
      // write PWM
      //
      int value = decode_input_value(inputIndex+1);
      if(value >= 0)
      {
        analogWrite(port, value);
      }
      else
      {
        interpreter_error(T_OUT_OF_RANGE);
      }
    }
    else // read port
    {
      if(port >= 0 or port <= 7)
      {
        Serial.println(*(pointers_to_ADC_readings[port]));
      }
      else
      {
        interpreter_error(T_OUT_OF_RANGE);
      }
    }
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}

/** @brief Turns a specific motor PWM to a specific value (and also set direction)
 *  @return Void.
 */
void motor_control()
{
  int motor = decode_input_value(1);
  if(motor >= 0)
  {
    if(inputString[inputIndex] == '=')
    {
      // write PWM
      //
      int motorPWM = decode_input_value_signed(inputIndex+1);
      if(motor==1)
      {
        setLeftMotorPWM(motorPWM);
      }
      else
      {
        setRightMotorPWM(motorPWM);
      }
    }
    else // read motor
    {
      interpreter_error(T_READ_NOT_SUPPORTED);
    }
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}



/** @brief Reads or writes the motor encoder values
 *  @return Void.
 */
void encoder_values()
{
  int motor = decode_input_value(1);
  if(motor >= 0)
  {
    if(inputString[inputIndex] == '=')
    {
      // write encoder
      //
      int32_t param = decode_input_value_signed(inputIndex+1);
      if(motor==1)
      {
        encoderLeftCount = param;
      }
      else
      {
        encoderRightCount = param;
      }
    }
    else // read motor
    {
      if(motor==1)
      {
        Serial.println(encoderLeftCount);
      }
      else
      {
        Serial.println(encoderRightCount);
      }
    }
  }
  else
  {
    char c = inputString[1];
    if (c == 'h')
    {
      // read both encoder values ahead of time so print time doesn't offset.
      int32_t left = encoderLeftCount;
      int32_t right = encoderRightCount;
      if (inputString[2] == 'z')
      {
        encoderLeftCount = 0;
        encoderRightCount = 0;
      }

      Serial.print(left, HEX);
      Serial.print(",");
      Serial.println(right, HEX);
    }
    else if (c == 0 or c == 'z')
    {
      // read both encoder values ahead of time so print time doesn't offset.
      int32_t left = encoderLeftCount;
      int32_t right = encoderRightCount;
      if (c == 'z')
      {
        encoderLeftCount = 0;
        encoderRightCount = 0;
      }
      Serial.print(left);
      Serial.print(",");
      Serial.println(right);
    }
    else
    {
      interpreter_error(T_OUT_OF_RANGE);
    }
  }
}


/** @brief Selects the left and right motor voltages
 *  @return Void.
 */
void motor_control_dual_voltage()
{
  float motor_left = decode_input_value_float(1);
  if(inputString[inputIndex] == ',')
  {
    // write PWM
    //
    float motor_right = decode_input_value_float(inputIndex+1);
    setMotorVolts(motor_left, motor_right);   // should this be float or what?
  }
  else // no comma
  {
    interpreter_error(T_UNEXPECTED_TOKEN);
  }
}


/** @brief Reads and writes stored parameters
 *  @return Void.
 */
void stored_parameter_control()
{
  int param_number = decode_input_value(1);
  if(param_number >= 0 and param_number < NUM_STORED_PARAMS)
  {
    if(inputString[inputIndex] == '=')
    {
      // write param
      //
      float param = decode_input_value_float(inputIndex+1);
      stored_params[param_number] = param;
      EEPROM.put(param_number*4, param);
    }
    else // read param
    {
        Serial.println(stored_params[param_number], floating_decimal_places);
    }
  }
  else if(param_number >= 100 and param_number < (100+NUMBER_OF_BITFIELD_STORED_PARAMS))
  {
    uint8_t shift = param_number-100;
    if(inputString[inputIndex] == '=')
    {
      uint32_t mask = 1UL << shift;
      uint32_t bit_value = decode_input_value(inputIndex+1);
      if(bit_value)
      {
        bitfield_stored_params |= mask;
      }
      else
      {
        bitfield_stored_params &= ~mask;
      }
      EEPROM.put(BITFIELD_ADDRESS, bitfield_stored_params);
    }
    else
    {
        Serial.println((bitfield_stored_params >> shift) & 1);
    }
  }
  else
  {
    if(inputString[1]=='a')
    {
      for(int i=0; i<NUM_STORED_PARAMS; i++)
      {
        Serial.println(stored_params[i], floating_decimal_places);
      }
    }
    else if(inputString[1]=='b')
    {
      for(int i=0; i<NUMBER_OF_BITFIELD_STORED_PARAMS; i++)
      {
        Serial.println((bitfield_stored_params & (1UL<<i))?1:0);
      }
    }
    else if(inputString[1]=='d')
    {
      const float* p = stored_parameters_default_values;
      for(int i=0; i<NUM_STORED_PARAMS; i++)
      {
        EEPROM.put(i*4, *p);
        stored_params[i] = *p++;
      }
      EEPROM.put(BITFIELD_ADDRESS, bitfield_default_values);
      bitfield_stored_params = bitfield_default_values;
    }
    else
    {
      interpreter_error(T_OUT_OF_RANGE);
    }
  }
}


/** @brief Reads the parameters into RAM. If Magic number not found will default all parameters.
 *  @return Void.
 */
void init_stored_parameters()
{
  uint32_t magic = 0;
  EEPROM.get(MAGIC_ADDRESS, magic);
  if(magic != MAGIC_NUMBER)
  {
    Serial.println("@Defaulting Params");
    // default values here
    const float* p = stored_parameters_default_values;
    for(int i=0; i<NUM_STORED_PARAMS; i++)
    {
      // we use write here, not update
      EEPROM.put(i*4, *p++);
    }
    EEPROM.put(BITFIELD_ADDRESS, bitfield_default_values);
    // finally write magic back
    EEPROM.put(MAGIC_ADDRESS, MAGIC_NUMBER);
  }

  for(int i=0; i<NUM_STORED_PARAMS; i++)
  {
    float f;
    EEPROM.get(i*4, f);
    stored_params[i] = f;
  }
  EEPROM.get(BITFIELD_ADDRESS, bitfield_stored_params);
}


/** @brief Turns command line interpreter verbose error messages on and off
 *  @return Void.
 */
void verbose_control()
{
  int param = decode_input_value(1);
  if(param == 0 or param == 1)
  {
    verbose_errors = param;
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}

/** @brief Turns command line interpreter echo of input on and off
 *  @return Void.
 */
void echo_control()
{
  int param = decode_input_value(1);
  if(param == 0 or param == 1)
  {
    interpreter_echo = param;
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}

/** @brief Turns the main emitter LED on and off.
 *  @return Void.
 */
void emitter_control()
{
  int param = decode_input_value(1);
  if(param == 0 or param == 1)
  {
    emitter_on = param;
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}

/** @brief  Echos a number to stdout from the command line.
 *  @return Void.
 */
void echo_command()
{
  int param = inputString[1];
  if(param == 'F')
  {
    Serial.println(decode_input_value_float(2), floating_decimal_places);
    return;
  }
  if(param == 'U')
  {
    Serial.println(decode_input_value(2));
    return;
  }
  if(param == 'S')
  {
    Serial.println(decode_input_value_signed(2));
    return;
  }
  if(param == '*')
  {
    Serial.println(inputString);
    return;
  }
  Serial.println(decode_input_value_float(1), floating_decimal_places);
}

/** @brief  Allows configuration of the Pin Mode from the command line.
 *  @return Void.
 */
void pinMode_command()
{
  int pin_number = decode_input_value(1);
  if(pin_number >= 0)
  {
    if(inputString[inputIndex] == '=')
    {
      // write PWM
      //
      char mode = inputString[inputIndex+1];
      if(mode == 'I')
      {
        pinMode(pin_number, INPUT);
      }
      else if(mode == 'O')
      {
        pinMode(pin_number, OUTPUT);
      }
      else if(mode == 'U')
      {
        pinMode(pin_number, INPUT_PULLUP);
      }
      else
      {
        interpreter_error(T_UNEXPECTED_TOKEN);
      }
    }
    else // read?
    {
      interpreter_error(T_READ_NOT_SUPPORTED);
    }
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}


/** @brief  Stops both motors
 *  @return Void.
 */
void stop_motors_and_everything_command()
{
  setMotorVolts(0, 0);
  // add action stop here as well
}

/** @brief  Prints the sensors (in various formats)
 *  @return Void.
 */
void print_sensors_control_command()
{
  int mode = inputString[1];
  if (mode == 'h')
  {
    print_sensors_control('h'); // hex
  }
  else if (mode == 0)
  {
    print_sensors_control('d'); // decimal
  }
  else  if (mode == 'r')
  {
    print_sensors_control('r'); // raw light and dark
  }
  else
  {
    interpreter_error(T_UNEXPECTED_TOKEN);
  }
}

#define SERIAL_IN_CAPTURE 0
#if SERIAL_IN_CAPTURE
char serial_capture_read_buff[256]; // circular buffer
uint8_t serial_capture_read_index = 0;
char hex[] = "0123456789ABCDEF";

/** @brief  Allows logging of input data for analysis.
 *  @return Void.
 */
void print_serial_capture_read_buff()
{
  for(int i=0; i< 256; i+= 16)
  {
    for(int j=0; j<16; j++)
    {
      uint8_t offset = serial_capture_read_index + j + i;
      char c = serial_capture_read_buff[offset];
      if (c< 32 or c > 126) {
        Serial.print(hex[c>>4]);
        Serial.print(hex[c&0xF]);
      }
      else
      {
        Serial.print(' ');
        Serial.print(c);
      }
    }
    Serial.println("");
  }
}
#endif



void not_implemented(){
  interpreter_error(T_UNKNOWN_COMMAND, inputString);
}

typedef void (*fptr)();

const PROGMEM fptr PROGMEM cmd2[] =
{
  not_implemented, // ' '
  not_implemented, // '!'
  not_implemented, // '"'
  not_implemented, // '#'
  stored_parameter_control, // '$'
  not_implemented, // '%'
  not_implemented, // '&'
  not_implemented, // '''
  not_implemented, // '('
  not_implemented, // ')'
  emitter_control, // '*'
  not_implemented, // '+'
  not_implemented, // ','
  not_implemented, // '-'
  not_implemented, // '.'
  not_implemented, // '/'
  not_implemented, // '0'
  not_implemented, // '1'
  not_implemented, // '2'
  not_implemented, // '3'
  not_implemented, // '4'
  not_implemented, // '5'
  not_implemented, // '6'
  not_implemented, // '7'
  not_implemented, // '8'
  not_implemented, // '9'
  not_implemented, // ':'
  not_implemented, // ';'
  not_implemented, // '<'
  echo_command, // '='
  not_implemented, // '>'
  ok, // '?'
  not_implemented, // '@'
  analogue_control, // 'A'
  not_implemented, // 'B'
  encoder_values, // 'C'
  digital_pin_control, // 'D'
  echo_control, // 'E'
  not_implemented, // 'F'
  not_implemented, // 'G'
  not_implemented, // 'H'
  not_implemented, // 'I'
  not_implemented, // 'J'
  not_implemented, // 'K'
  not_implemented, // 'L'
  motor_control, // 'M'
  motor_control_dual_voltage, // 'N'
  not_implemented, // 'O'
  pinMode_command, // 'P'
  not_implemented, // 'Q'
  not_implemented, // 'R'
  print_sensors_control_command, // 'S'
  not_implemented, // 'T'
  not_implemented, // 'U'
  verbose_control, // 'V'
  not_implemented, // 'W'
  not_implemented, // 'X'
  not_implemented, // 'Y'
  not_implemented, // 'Z'
  not_implemented, // '['
#if SERIAL_IN_CAPTURE
  print_serial_capture_read_buff, // '\'
#else
  not_implemented, // '\'
#endif
  not_implemented, // ']'
  reset_state, // '^'
  not_implemented, // '_'
  not_implemented, // '`'
  not_implemented, // 'a'
  print_bat, // 'b'
  not_implemented, // 'c'
  not_implemented, // 'd'
  print_encoders, // 'e'
  not_implemented, // 'f'
  not_implemented, // 'g'
  ok, // 'h'
  not_implemented, // 'i'
  not_implemented, // 'j'
  not_implemented, // 'k'
  led, // 'l'
  motor_test, // 'm'
  not_implemented, // 'n'
  not_implemented, // 'o'
  not_implemented, // 'p'
  not_implemented, // 'q'
  print_encoder_setup, // 'r'
  print_switches, // 's'
  not_implemented, // 't'
  not_implemented, // 'u'
  show_version, // 'v'
  not_implemented, // 'w'     // used to be print_wall_sensors
  stop_motors_and_everything_command, // 'x'
  not_implemented, // 'y'
  zero_encoders, // 'z'
  not_implemented, // '{'
  not_implemented, // '|'
  not_implemented, // '}'
};
const int CMD2_SIZE = sizeof(cmd2) / sizeof(fptr);


/** @brief  Finds the single character command from a list.
 *  @return Void.
 */
void parse_cmd() {
  int command = inputString[0] - ' ';
  if (command >= CMD2_SIZE) {
    interpreter_error(T_UNKNOWN_COMMAND);
    return;
  }
  fptr f = fptr(pgm_read_ptr(cmd2 + command));
  f();
}

#define CTRL_C 0x03
#define BACKSPACE 0x08
#define CTRL_X 0x18



/** @brief  Command line interpreter.
 *  @return Void.
 */
void interpreter()
{
    while (Serial.available()) {
      char inChar = (char)Serial.read();      // get the new byte:
#if SERIAL_IN_CAPTURE
  serial_capture_read_buff[serial_capture_read_index++]=inChar;
#endif

      if(inChar > ' ')
      {
        if(interpreter_echo) { Serial.write(inChar); }

        inputString[inputIndex++] = inChar;      // add it to the inputString:
        if(inputIndex == MAX_INPUT_SIZE)
        {
            interpreter_error(T_LINE_TOO_LONG);
            inputIndex = 0;
        }
      }
      else
      {
        // if the incoming character is a newline interpret it
        if(inChar == '\n')
        {
            if(interpreter_echo) { Serial.write(inChar); }
            if(inputIndex)
            {
              inputString[inputIndex] = 0;  // zero terminate
              parse_cmd();
              inputIndex = 0;
            }
            else
            {
              // what do we want to print here? OK for V0?
              interpreter_error(T_OK);
            }
        }
        else if(inChar == CTRL_X or inChar == CTRL_C)
        {
          if(inChar == CTRL_X)
          {
            stop_motors_and_everything_command();
          }
          inputIndex = 0;
          Serial.println();
        }
        else if(inChar == BACKSPACE and inputIndex != 0)
        {
            inputIndex--;

            // This sequence depends on terminal emulator
            Serial.print("\x08 \x08");
            //Serial.print("\x08");
        }
      }
    }
}
