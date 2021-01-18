#include <Arduino.h>
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
// There are (NUM_STORED_PARAMS+1) * 4 bytes stored in EEPROM. NOTE: The last one is a magic number to detect an uninitialised EEPROM.
//
#define NUM_STORED_PARAMS 16
#define MAGIC_ADDRESS ((NUM_STORED_PARAMS+1)*4)
#define MAGIC_NUMBER 0xD00DCAFE
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
void interpreter_error(int error)
{
  if(verbose_errors)
  {
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
        Serial.println(F("?"));
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
void led() { digitalWrite(LED_BUILTIN, (inputString[1] == '0') ? LOW:HIGH); }

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
  // We should reset all state here. At the moment there isn't any
  Serial.println(F("RST"));
}


/** @brief  Show version of the program.
 *  @param  
 *  @return void
 */
void show_version() { Serial.println(F("v1.1")); }


/** @brief  Print the decoded switch value.
 *  @param  
 *  @return void
 */
void print_switches() { Serial.println(gFunctionSwitch); }


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
    if (gFunctionSwitch == 16) {
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
        digitalWrite(port, HIGH);
      }
      else
      {
        digitalWrite(port, LOW);
      }
    }
    else // read port
    {
      Serial.println(digitalRead(port));
    }
  }
  else
  {
    interpreter_error(T_OUT_OF_RANGE);
  }
}

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
      Serial.println(analogRead(port));
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
    interpreter_error(T_OUT_OF_RANGE);
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
  else
  {
    if(inputString[1]=='a')
    {
      for(int i=0; i<NUM_STORED_PARAMS; i++)
      {
        Serial.println(stored_params[i], floating_decimal_places);
      }
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
    
    // finally write magic back
    EEPROM.put(MAGIC_ADDRESS, MAGIC_NUMBER);
  }
  
  for(int i=0; i<NUM_STORED_PARAMS; i++)
  {
    float f;
    EEPROM.get(i*4, f);
    stored_params[i] = f;
  }
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

/** @brief  Echos a number to stdout from the command line.
 *  @return Void.
 */
void echo_number()
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
  Serial.println(decode_input_value_float(1), floating_decimal_places);
}

typedef struct {
    char cmd;
    void (*func)();
} cmds_t;

const /*PROGMEM*/ cmds_t cmds[] = {
    {'l', led },
    {'?', ok },
    {'h', ok },
    {'b', print_bat },
    {'s', print_switches },
    {'w', print_wall_sensors },
    {'e', print_encoders },
    {'z', zero_encoders },
    {'r', print_encoder_setup },
    {'m', motor_test },
    {'=', echo_number },   // not official command, just for testing

    // Interpreter Commands
    {'^', reset_state },
    {'v', show_version },
    {'V', verbose_control },
    {'E', echo_control },
    

    // Robot I/O remote control
    {'D', digital_pin_control },
    {'A', analogue_control },
    {'M', motor_control },
    {'N', motor_control_dual_voltage },
    {'C', encoder_values },
    {'$', stored_parameter_control },
    {0, 0}
};

/*
// templates from:  https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs
template <typename T> void PROGMEM_readAnything (const T * sce, T& dest)
  {
  memcpy_P (&dest, sce, sizeof (T));
  }

template <typename T> T PROGMEM_getAnything (const T * sce)
  {
  static T temp;
  memcpy_P (&temp, sce, sizeof (T));
  return temp;
  }
*/

/** @brief  Finds the single character command from a list.
 *  @return Void.
 */
void parse_cmd()
{
    const cmds_t* cmd_ptr = cmds;
    char command;
    while(1)
    {
        if( not (command = cmd_ptr->cmd) )
        {
            interpreter_error(T_UNKNOWN_COMMAND);
            return;
        }
        
        if(command == inputString[0])
        {
            cmd_ptr->func();
            return;
        }
        cmd_ptr++;
    }
}

#define CTRL_C 0x03
#define CTRL_X 0x18

/** @brief  Command line interpreter.
 *  @return Void.
 */
void interpreter()
{    
    while (Serial.available()) {
      char inChar = (char)Serial.read();      // get the new byte:

      if(interpreter_echo) { Serial.write(inChar); }
      
      inputString[inputIndex++] = inChar;      // add it to the inputString:
      if(inputIndex == MAX_INPUT_SIZE)
      {
          interpreter_error(T_LINE_TOO_LONG);
          inputIndex = 0;
      }
      // if the incoming character is a newline interpret it
      else if(inChar <= ' ')
      {
        if(inChar == '\n')
        {
            parse_cmd();
            inputIndex = 0;
        }
        else if(inChar == CTRL_X or inChar == CTRL_C)
        {
          if(inChar == CTRL_X)
          {
            setMotorVolts(0, 0);
          }
          inputIndex = 0;
        }
      }
    }
}
