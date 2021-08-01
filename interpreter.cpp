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
#include "interpreter.h"
#include "digitalWriteFast.h"
#include "read-number.h"
#include "settings.h"
#include "switches.h"
#include "tests.h"
#include "motors.h"
#include "profile.h"
#include "distance-moved.h"
#include "sensors_control.h"
#include "misc_definitions.h"
#include <Arduino.h>

/*
 * Small command line interpreter
 */

// TODO: have CLI treat '#' as a comment character and drop remaining line

char inputString[MAX_INPUT_SIZE]; // a String to hold incoming data
int inputIndex = 0;               // where we are on the input
static bool interpreter_echo = true;

// ------------------------------------------
// These are the types
enum
{
    NUMERIC_ERRORS = 0, // Numeric error codes. Good for machines, bad for humans.
    TEXT_ERRORS = 1,    // Text error codes.
    TEXT_VERBOSE = 2    // All commands return a text message, even silent ones. Noisy, but good for beginners.
};

// Setting for verboseness.
uint8_t verbose_errors = TEXT_VERBOSE;

/** @brief  Prints OK
 *  @param
 *  @return void
 */
int8_t ok()
{
    if (verbose_errors)
    {
        Serial.println(F("OK"));
    }
    else
    {
        Serial.println(F("@Error:0"));
    }
    return T_SILENT_ERROR;
}

/** @brief  Print the interpreter error
 *  @param  error to be printed
 *  @return void
 */
void interpreter_error(int8_t error, char *extra = 0)
{
    if (error == T_SILENT_ERROR or (error == T_OK and verbose_errors < TEXT_VERBOSE))
    {
        return;
    }
    if (verbose_errors)
    {
        if (error != T_OK)
        {
            Serial.print(F("@Error:"));
        }
        switch (error)
        {
            case T_OK:
                ok();
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
                if (extra)
                {
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
        Serial.print(F("@Error:"));
        Serial.println(error);
    }
}

//
// functions to run commands
//

/** @brief  Turns on and off the fixed LED
 *  @param
 *  @return void
 */
int8_t led()
{
    digitalWriteFast(LED_BUILTIN, (inputString[1] == '0') ? LOW : HIGH);
    return T_OK;
}

int8_t stop_motors_and_everything_command();

/** @brief  Reset the robot state to a known value
 *          NOTE: Does not reset $ parameters.
 *  @param
 *  @return void
 */
int8_t reset_state()
{
    char function = inputString[1];
    if (function == '^')
    {
        void (*resetFunc)(void) = 0; // declare reset function at address 0
        resetFunc();
    }
    else
    {
        stop_motors_and_everything_command();

        // We should reset all state here. At the moment there isn't any.
        Serial.println(F("RST"));

        // Reset the actual state
        verbose_errors = TEXT_VERBOSE;
        interpreter_echo = true;
    }
    return T_OK;
}

/** @brief  Show version of the program.
 *  @param
 *  @return void
 */
int8_t show_version()
{
    Serial.println(F("v1.6"));
    return T_OK;
}

/** @brief  Print the decoded switch value.
 *  @param
 *  @return void
 */
int8_t print_switches()
{
    Serial.println(readFunctionSwitch());
    return T_OK;
}

// helper function
static void setMotorVolts(float left, float right)
{
    set_left_motor_volts(left);
    set_right_motor_volts(right);
}

/** @brief  Select one of several hardware motor tests.
 *  @param
 *  @return void
 */
int8_t motor_test()
{
    disable_motor_controllers();
    char function = inputString[1];

    switch (function)
    {
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
    while (endTime > millis())
    {
        if (readFunctionSwitch() == 16)
        {
            break; // stop running if the button is pressed
        }
    }
    // be sure to turn off the motors
    setMotorVolts(0, 0);
    return T_OK;
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
    int n = inputString[index++] - '0';
    if (n < 0 or n >= 10)
    {
        inputIndex = index - 1;
        return -1;
    }
    while (true)
    {
        int n2 = inputString[index] - '0';
        if (n2 < 0 or n2 >= 10)
        {
            break;
        }
        n = n * 10 + n2;
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
    if (inputString[index] == '-')
    {
        int value = decode_input_value(index + 1);
        if (value < 0)
        {
            return 0;
        }
        return -value;
    }
    else
    {
        int value = decode_input_value(index);
        if (value < 0)
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
    while (true)
    {
        int digit = inputString[index] - '0';
        if (digit < 0 or digit > 9)
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
    while (true)
    {
        int digit = inputString[index];
        if (digit < '0' or digit > '9')
        {
            if (digit == '.') // decimal point
            {
                index++;
                return fractional_float(index, n);
            }
            break;
        }
        n = n * 10 + digit - '0';
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
    if (inputString[index] == '-')
    {
        float value = decode_input_value_float_unsigned(index + 1);
        if (value > 0)
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
int8_t digital_pin_control()
{
    // D3=1
    // D13=0
    // Ignore spaces?

    int port = decode_input_value(1);
    if (port >= 0)
    {
        if (inputString[inputIndex] == '=')
        {
            // write port
            //
            // Could be: digitWrite(port, inputString[inputIndex+1]-'0')
            if (inputString[inputIndex + 1] == '1')
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
        return T_OUT_OF_RANGE;
    }
    return T_OK;
}

static volatile int *pointers_to_ADC_readings[] =
    {
        &gSensorA0_dark,            // SENSOR_RIGHT_MARK = A0;
        &gSensorA1_dark,            // SENSOR_1 = A1;
        &gSensorA2_dark,            // SENSOR_2 = A2;
        &gSensorA3_dark,            // SENSOR_3 = A3;
        &gSensorA4_light,           // SENSOR_4 = A4;
        &gSensorA5_light,           // SENSOR_LEFT_MARK = A5;
        &Switch_ADC_value,          // FUNCTION_PIN = A6;
        &raw_BatteryVolts_adcValue, // BATTERY_VOLTS = A7;
};

/** @brief Reads an analogue pin or sets a PWM output.
 *  @return Void.
 */
int8_t analogue_control()
{
    // A2
    // A9=255
    int port = decode_input_value(1);
    if (port >= 0)
    {
        if (inputString[inputIndex] == '=')
        {
            // write PWM
            //
            int value = decode_input_value(inputIndex + 1);
            if (value >= 0)
            {
                analogWrite(port, value);
            }
            else
            {
                return T_OUT_OF_RANGE;
            }
        }
        else // read port
        {
            if (port >= 0 or port <= 7)
            {
                Serial.println(*(pointers_to_ADC_readings[port]));
            }
            else
            {
                return T_OUT_OF_RANGE;
            }
        }
    }
    else
    {
        return T_OUT_OF_RANGE;
    }
    return T_OK;
}

/** @brief Turns a specific motor PWM to a specific value (and also set direction)
 *  @return Void.
 */
int8_t motor_control()
{
    disable_motor_controllers();
    int motor = decode_input_value(1);
    if (motor >= 0)
    {
        if (inputString[inputIndex] == '=')
        {
            // write PWM
            //
            int motorPWM = decode_input_value_signed(inputIndex + 1);
            if (motor == 1)
            {
                set_left_motor_pwm(motorPWM);
            }
            else
            {
                set_right_motor_pwm(motorPWM);
            }
        }
        else // read motor
        {
            return T_READ_NOT_SUPPORTED;
        }
    }
    else
    {
        return T_OUT_OF_RANGE;
    }
    return T_OK;
}

/** @brief Reads or writes the motor encoder values
 *  @return Void.
 */
int8_t encoder_values()
{
    int motor = decode_input_value(1);
    if (motor >= 0)
    {
        // read motor
        if (motor == 1)
        {
            Serial.println(encoder_left_total());
        }
        else
        {
            Serial.println(encoder_right_total());
        }
    }
    else
    {
        char c = inputString[1];
        if (c == 'h')
        {
            // read both encoder values ahead of time so print time doesn't offset.
            int32_t left = encoder_left_total();
            int32_t right = encoder_right_total();
            if (inputString[2] == 'z')
            {
                reset_encoders();
            }

            Serial.print(left, HEX);
            Serial.print(",");
            Serial.println(right, HEX);
        }
        else if (c == 0 or c == 'z')
        {
            // read both encoder values ahead of time so print time doesn't offset.
            int32_t left = encoder_left_total();
            int32_t right = encoder_right_total();
            if (c == 'z')
            {
                reset_encoders();
            }
            Serial.print(left);
            Serial.print(",");
            Serial.println(right);
        }
        else
        {
            return T_OUT_OF_RANGE;
        }
    }
    return T_OK;
}

/** @brief Selects the left and right motor voltages
 *  @return Void.
 */
int8_t motor_control_dual_voltage()
{
    disable_motor_controllers();
    float motor_left = decode_input_value_float(1);
    if (inputString[inputIndex] == ',')
    {
        // write PWM
        //
        float motor_right = decode_input_value_float(inputIndex + 1);
        setMotorVolts(motor_left, motor_right); // should this be float or what?
    }
    else // no comma
    {
        return T_UNEXPECTED_TOKEN;
    }
    return T_OK;
}

/*----------------------------------------------------------------*/

/** @brief Turns command line interpreter verbose error messages on and off
 *  @return Void.
 */
int8_t verbose_control()
{
    int param = decode_input_value(1);
    if (param >= 0 or param <= 2)
    {
        verbose_errors = param;
    }
    else
    {
        return T_OUT_OF_RANGE;
    }
    return T_OK;
}

/** @brief Turns command line interpreter echo of input on and off
 *  @return Void.
 */
int8_t echo_control()
{
    int param = decode_input_value(1);
    if (param == 0 or param == 1)
    {
        interpreter_echo = param;
    }
    else
    {
        return T_OUT_OF_RANGE;
    }
    return T_OK;
}

/** @brief Turns the main emitter LED on and off.
 *  @return Void.
 */
int8_t emitter_control()
{
    int param = decode_input_value(1);
    if (param == 0 or param == 1)
    {
        emitter_on(param);
    }
    else
    {
        return T_OUT_OF_RANGE;
    }

    return T_OK;
}

/** @brief  Echos a number to stdout from the command line.
 *  @return Void.
 */
int8_t echo_command()
{
    int param = inputString[1];
    if (param == 'F')
    {
        Serial.println(decode_input_value_float(2), DEFAULT_DECIMAL_PLACES);
    }
    else if (param == 'U')
    {
        Serial.println(decode_input_value(2));
    }
    else if (param == 'S')
    {
        Serial.println(decode_input_value_signed(2));
    }
    else if (param == '*')
    {
        Serial.println(inputString);
    }
    else
    {
        Serial.println(decode_input_value_float(1), DEFAULT_DECIMAL_PLACES);
    }
    return T_OK;
}

/** @brief  Allows configuration of the Pin Mode from the command line.
 *  @return Void.
 */
int8_t pinMode_command()
{
    int pin_number = decode_input_value(1);
    if (pin_number >= 0)
    {
        if (inputString[inputIndex] == '=')
        {
            // write PWM
            //
            char mode = inputString[inputIndex + 1];
            if (mode == 'I')
            {
                pinMode(pin_number, INPUT);
            }
            else if (mode == 'O')
            {
                pinMode(pin_number, OUTPUT);
            }
            else if (mode == 'U')
            {
                pinMode(pin_number, INPUT_PULLUP);
            }
            else
            {
                return T_UNEXPECTED_TOKEN;
            }
        }
        else // read?
        {
            return T_READ_NOT_SUPPORTED;
        }
    }
    else
    {
        return T_OUT_OF_RANGE;
    }
    return T_OK;
}

/** @brief  Stops both motors
 *  @return Void.
 */
int8_t stop_motors_and_everything_command()
{
    disable_motor_controllers();
    stop_motors();

    // not strictly necessary, because we've disabled the
    // controller - but we do this anyway
    forward.reset();
    rotation.reset();
    reset_motor_controllers();

    // add action stop here as well
    return T_OK;
}

/** @brief  Prints the sensors (in various formats)
 *  @return Void.
 */
int8_t print_sensors_control_command()
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
    else if (mode == 'r')
    {
        print_sensors_control('r'); // raw light and dark
    }
    else
    {
        return T_UNEXPECTED_TOKEN;
    }
    return T_OK;
}

int8_t print_encoders_command()
{
    // translate into argument
    // If no second parameters this will be 0.
    if (print_encoders(inputString[1]) == false)
    {
        return T_UNEXPECTED_TOKEN;
    }
    return T_OK;
}

int8_t print_bat()
{
    float bat = battery_voltage;
    if (inputString[1] == 'i')
    {
        int bat_int = bat * 1000;
        Serial.println(bat_int);
    }
    else if (inputString[1] == 'h')
    {
        int bat_int = bat * 1000;
        Serial.println(bat_int, 16);
    }
    else
    {
        Serial.println(battery_voltage, DEFAULT_DECIMAL_PLACES);
    }
    return T_OK;
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
    for (int i = 0; i < 256; i += 16)
    {
        for (int j = 0; j < 16; j++)
        {
            uint8_t offset = serial_capture_read_index + j + i;
            char c = serial_capture_read_buff[offset];
            if (c < 32 or c > 126)
            {
                Serial.print(hex[c >> 4]);
                Serial.print(hex[c & 0xF]);
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

int8_t tracking_steering_adjustment()
{
    float tracking = decode_input_value_float(1);
    g_steering_adjustment = tracking;
    return T_OK;
}

int8_t position_speed_move()
{
    char c = inputString[1];
    if (c == '?')
    {
        forward.is_finished();
    }
    else if (c == 'z')
    {
        forward.reset();
    }
    else // 1,2,3,4
    {
        float distance = decode_input_value_float(1);
        if (inputString[inputIndex] != ',')
        {
            return T_UNEXPECTED_TOKEN;
        }
        float topSpeed = decode_input_value_float(inputIndex + 1);
        if (inputString[inputIndex] != ',')
        {
            return T_UNEXPECTED_TOKEN;
        }
        float finalSpeed = decode_input_value_float(inputIndex + 1);
        if (inputString[inputIndex] != ',')
        {
            return T_UNEXPECTED_TOKEN;
        }
        float acceleration = decode_input_value_float(inputIndex + 1);
        forward.start(distance, topSpeed, finalSpeed, acceleration);
    }
    return T_OK;
}

int8_t rotation_move()
{
    char c = inputString[1];
    if (c == '?')
    {
        rotation.is_finished();
    }
    else if (c == 'z')
    {
        rotation.reset();
    }
    else // 1,2,3,4
    {
        float distance = decode_input_value_float(1);
        if (inputString[inputIndex] != ',')
        {
            return T_UNEXPECTED_TOKEN;
        }
        float topSpeed = decode_input_value_float(inputIndex + 1);
        if (inputString[inputIndex] != ',')
        {
            return T_UNEXPECTED_TOKEN;
        }
        float finalSpeed = decode_input_value_float(inputIndex + 1);
        if (inputString[inputIndex] != ',')
        {
            return T_UNEXPECTED_TOKEN;
        }
        float acceleration = decode_input_value_float(inputIndex + 1);
        rotation.start(distance, topSpeed, finalSpeed, acceleration);
    }
    return T_OK;
}

int8_t motor_control_control()
{
    char c = inputString[1];
    if (c == '1')
    {
        enable_motor_controllers();
    }
    else if (c == '0')
    {
        disable_motor_controllers();
    }
    else if (c == 'z')
    {
        reset_motor_controllers();
    }
    else
    {
        return T_UNEXPECTED_TOKEN;
    }
    return T_OK;
}

int execute_settings_command(char *line)
{
    // we already know that line[0] == '$'
    if (line[1] == '\0')
    {
        return T_OK;
    }
    if (line[2] == '\0')
    { // one of the single character commands
        switch (line[1])
        {
            case '$':
                dump_settings(5);
                return T_OK;
                break;
            case '#':
                restore_default_settings();
                return T_OK;
                break;
            case '@':
                load_settings_from_eeprom();
                return T_OK;
                break;
            case '!':
                save_settings_to_eeprom();
                return T_OK;
                break;
            case '?':
                // could be used by host to populate its data structures
                // list the settings names and types?
                // or send them as a C declaration?
                // or a JSON object ...
                dump_settings_detail();
                return T_OK;
                break;
        }
    }

    //OK - so it must be a parameter fetch/update
    uint8_t pos = 1; // the first character [0] is already known
    // get the parameter index
    int index;
    if (!read_integer(line, &pos, &index))
    {
        // This could be the place to trigger a string search
        return T_UNEXPECTED_TOKEN;
    }
    if (index < 0 or index >= get_settings_count())
    {
        return T_OUT_OF_RANGE;
    }

    // There is a parameter index, now see if this is an assignment
    if (line[pos++] != '=')
    {
        print_setting(index, 3); // no, just report the value
        Serial.println();
        return T_OK;
    }

    // It was an assignment so get the value
    float value;
    if (!read_float(line, &pos, &value))
    {
        return T_OUT_OF_RANGE;
    }
    // Any remaining characters are ignored

    // Everything must have worked. Woot!
    write_setting(index, value);
    return T_OK;
}

int8_t system_command()
{
    return execute_settings_command(inputString);
}

int8_t not_implemented()
{
    interpreter_error(T_UNKNOWN_COMMAND, inputString);
    return T_SILENT_ERROR;
}

typedef int8_t (*fptr)();

const PROGMEM fptr PROGMEM cmd2[] =
    {
        not_implemented,               // ' '
        not_implemented,               // '!'
        not_implemented,               // '"'
        not_implemented,               // '#'
        system_command,                // '$'
        not_implemented,               // '%'
        not_implemented,               // '&'
        not_implemented,               // '''
        not_implemented,               // '('
        not_implemented,               // ')'
        emitter_control,               // '*'
        not_implemented,               // '+'
        not_implemented,               // ','
        not_implemented,               // '-'
        not_implemented,               // '.'
        not_implemented,               // '/'
        not_implemented,               // '0'
        not_implemented,               // '1'
        not_implemented,               // '2'
        not_implemented,               // '3'
        not_implemented,               // '4'
        not_implemented,               // '5'
        not_implemented,               // '6'
        not_implemented,               // '7'
        not_implemented,               // '8'
        not_implemented,               // '9'
        not_implemented,               // ':'
        not_implemented,               // ';'
        not_implemented,               // '<'
        echo_command,                  // '='
        not_implemented,               // '>'
        ok,                            // '?'
        not_implemented,               // '@'
        analogue_control,              // 'A'
        not_implemented,               // 'B'
        encoder_values,                // 'C'
        digital_pin_control,           // 'D'
        echo_control,                  // 'E'
        not_implemented,               // 'F'
        not_implemented,               // 'G'
        not_implemented,               // 'H'
        not_implemented,               // 'I'
        not_implemented,               // 'J'
        not_implemented,               // 'K'
        not_implemented,               // 'L'
        motor_control,                 // 'M'
        motor_control_dual_voltage,    // 'N'
        not_implemented,               // 'O'
        pinMode_command,               // 'P'
        not_implemented,               // 'Q'
        rotation_move,                 // 'R'
        print_sensors_control_command, // 'S'
        tracking_steering_adjustment,  // 'T'       // used to be old motor controller
        not_implemented,               // 'U'
        verbose_control,               // 'V'
        not_implemented,               // 'W'
        not_implemented,               // 'X'
        not_implemented,               // 'Y'
        not_implemented,               // 'Z'
        not_implemented,               // '['
#if SERIAL_IN_CAPTURE
        print_serial_capture_read_buff, // '\'
#else
        not_implemented, // '\'
#endif
        not_implemented,                    // ']'
        reset_state,                        // '^'
        not_implemented,                    // '_'
        not_implemented,                    // '`'
        not_implemented,                    // 'a'
        print_bat,                          // 'b'
        motor_control_control,              // 'c'
        not_implemented,                    // 'd'
        print_encoders_command,             // 'e'
        not_implemented,                    // 'f'
        not_implemented,                    // 'g'
        ok,                                 // 'h'
        not_implemented,                    // 'i'
        not_implemented,                    // 'j'
        not_implemented,                    // 'k'
        led,                                // 'l'
        motor_test,                         // 'm'
        not_implemented,                    // 'n'
        not_implemented,                    // 'o'
        position_speed_move,                // 'p'
        cmd_test_runner,                    // 'q'
        print_encoder_setup,                // 'r'
        print_switches,                     // 's'
        not_implemented,                    // 't'
        not_implemented,                    // 'u'
        show_version,                       // 'v'
        not_implemented,                    // 'w'     // used to be print_wall_sensors
        stop_motors_and_everything_command, // 'x'
        not_implemented,                    // 'y'
        zero_encoders,                      // 'z'
        not_implemented,                    // '{'
        not_implemented,                    // '|'
        not_implemented,                    // '}'
};
const int CMD2_SIZE = sizeof(cmd2) / sizeof(fptr);

/** @brief  Finds the single character command from a list.
 *  @return Void.
 */
void parse_cmd()
{
    int command = inputString[0] - ' ';
    if (command >= CMD2_SIZE)
    {
        interpreter_error(T_UNKNOWN_COMMAND);
        return;
    }
    fptr f = fptr(pgm_read_ptr(cmd2 + command));
    interpreter_error(f());
}

#define CTRL_C 0x03
#define BACKSPACE 0x08
#define CTRL_X 0x18
static char last_NL = 0; // tracks NL changes

/** @brief  Command line interpreter.
 *  @return Void.
 */
void interpreter()
{
    while (Serial.available())
    {
        char inChar = (char)Serial.read(); // get the new byte:
#if SERIAL_IN_CAPTURE
        serial_capture_read_buff[serial_capture_read_index++] = inChar;
#endif

        // At the moment we treat space as a special character and ignore than.
        // In future we might want to change that
        if (inChar > ' ')
        {
            if (interpreter_echo)
            {
                Serial.write(inChar);
            }

            inputString[inputIndex++] = inChar; // add it to the inputString:
            if (inputIndex == MAX_INPUT_SIZE)
            {
                interpreter_error(T_LINE_TOO_LONG);
                inputIndex = 0;
            }
        }
        else
        {
            // if the incoming character is a newline of some sort ... interpret it
            if (inChar == '\n' or inChar == '\r')
            {

                if (inputIndex) // characters in the input buffer - process them
                {
                    if (interpreter_echo)
                    {
                        Serial.println();
                    }
                    inputString[inputIndex] = 0; // zero terminate
                    parse_cmd();
                    inputIndex = 0;
                }
                else
                {
                    // Here comes some complicated code to deal with CR or LF or CRLF line endings without giving a double OK for CRLF
                    //
                    // There has been two line returns (CR or LF) in a row (we know this because of inputIndex==0) ... is one of them a CRLF or LFCR pair?
                    //
                    // So we need to check it wasn't a different one so we can ignore CRLF (or LFCR) pairs.

                    //Serial.write(last_NL+'A'-1);
                    //Serial.write(inChar+'A'-1);
                    if (last_NL == 0 or inChar == last_NL)
                    {
                        if (interpreter_echo)
                        {
                            Serial.println();
                        }
                        // what do we want to print here? OK for V0?
                        ok();
                    }
                    else
                    {
                        // we ignored a CR or LF. We shouldn't ignore the next one
                        inChar = 0;
                    }
                }
                // This makes sure we track CR or LF as the accpeting character
                // But this also ensures a change from, say, LFCR to CR will do the right thing
                last_NL = inChar;
                break; // go back to loop() once we've processed one command to run other loop things. One command at a time only!
            }
            else if (inChar == CTRL_X or inChar == CTRL_C)
            {
                if (inChar == CTRL_X)
                {
                    stop_motors_and_everything_command();
                }
                inputIndex = 0;
                Serial.println();
            }
            else if (inChar == BACKSPACE and inputIndex != 0)
            {
                inputIndex--;

                // This sequence depends on terminal emulator
                Serial.print("\x08 \x08");
                //Serial.print("\x08");
            }
        }
    }
}
