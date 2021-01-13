#include <Arduino.h>
#include "public.h"

/*
 * Small command line interpreter
 */

#define MAX_INPUT_SIZE 10
char inputString[MAX_INPUT_SIZE];         // a String to hold incoming data
int inputIndex = 0;  // where we are on the input


typedef struct {
    const char *nam;
    void (*func)();
} entry_t;

//
// functions to run commands
//
void led() { digitalWrite(LED_BUILTIN, (inputString[1] == '0') ? LOW:HIGH); }

void ok() { Serial.println(F("OK")); }

void reset_state() {
  // We should reset all state here. At the moment there isn't any
  Serial.println(F("RST"));
}

void show_version() { Serial.println(F("v1.0")); }

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
    {'r', reset_state },
    {'v', show_version },
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
      Serial.write(inChar);
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
