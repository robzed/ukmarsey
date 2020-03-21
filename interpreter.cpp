#include <Arduino.h>
#include "public.h"

/*
 * Small command line interpreter
 */

#define MAX_INPUT_SIZE 10
char inputString[MAX_INPUT_SIZE];         // a String to hold incoming data
char inputIndex = 0;  // where we are on the input


typedef struct {
    const char *nam;
    void (*func)();
} entry_t;

//
// functions to run commands
//
void led() { digitalWrite(LED_BUILTIN, (inputString[1] == '0') ? LOW:HIGH); }

void ok() { Serial.println(F("OK")); }

void print_switches() { Serial.println(gFunctionSwitch); }



typedef struct {
    char cmd;
    void (*func)();
} cmds_t;

const /*PROGMEM*/ cmds_t cmds[] = {
    {'l', led},
    {'?', ok},
    {'h', ok},
    {'b', print_bat},
    {'s', print_switches},
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
