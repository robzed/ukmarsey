#include <Arduino.h>
#include "public.h"

/*
 * Small command line interpreter
 */

#define MAX_INPUT_SIZE 10
static char inputString[MAX_INPUT_SIZE];         // a String to hold incoming data
static int inputIndex = 0;  // where we are on the input
static bool interpreter_echo = true;

enum
{
  T_OK = 0,
  T_PORT_OUT_OF_RANGE = 1,
};


const char* p;
void interpreter_error(int error)
{
  switch(error)
  {
    case T_OK:
      Serial.println(F("OK"));
      break;
    case T_PORT_OUT_OF_RANGE:
      Serial.println(F("Out of range"));
      break;
    default:
      Serial.println(F("Error"));
      break;    
  }
}

typedef struct {
    const char *nam;
    void (*func)();
} entry_t;

//
// functions to run commands
//
void led() { digitalWrite(LED_BUILTIN, (inputString[1] == '0') ? LOW:HIGH); }

void ok() { interpreter_error(T_OK); }

void reset_state() {
  // We should reset all state here. At the moment there isn't any
  Serial.println(F("RST"));
}

void show_version() { Serial.println(F("v1.1")); }

void print_switches() { Serial.println(gFunctionSwitch); }

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

int decode_input_value_1or2char_unsigned(int index)
{
  int n = inputString[index++]-'0';
  if(n < 0 or n >= 10)
  {
    inputIndex = index-1; 
    return -1;
  }
  int n2 = inputString[index]-'0';
  if(n2 >= 0 and n2 < 10)
  {
    n = n*10 + n2;
    index++;
  }
  inputIndex = index;
  return n;
}

void set_digital_output_pin()
{
  // D3=1
  // D13=0
  // Ignore spaces?

  int port = decode_input_value_1or2char_unsigned(1);
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
    interpreter_error(T_PORT_OUT_OF_RANGE);
  }
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
    {'^', reset_state },
    {'v', show_version },

    // Robot remote control
    {'D', set_digital_output_pin },
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
void parse_cmd()
{
    const cmds_t* cmd_ptr = cmds;
    char command;
    while(1)
    {
        if( not (command = cmd_ptr->cmd) )
        {
            Serial.println(F("?\n"));
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


void interpreter()
{
    while (Serial.available()) {
      char inChar = (char)Serial.read();      // get the new byte:

      if(interpreter_echo) { Serial.write(inChar); }
      
      inputString[inputIndex++] = inChar;      // add it to the inputString:
      if(inputIndex == MAX_INPUT_SIZE)
      {
          Serial.println(F("Too long"));
          inputIndex = 0;
      }
      // if the incoming character is a newline interpret it
      else if(inChar == '\n')
      {
          parse_cmd();
          inputIndex = 0;
      }
    }
}
