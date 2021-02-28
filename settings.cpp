/***********************************************************************
 * Created by Peter Harrison on 2019-06-10.
 * Copyright (c) 2019 Peter Harrison
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "settings.h"
#include "EEPROM.h"
#include <Arduino.h>
// TODO: read and write the setting in EEPROM

// create arrays holding the details of each setting in flash

/***
 * In flash memory, create an array of strings
 * to hold the variable names.
 *
 * This list would normally only be used to inform the user
 * during displays or dumps of the setting or perhaps as
 * part of an interacive configurator.
 *
 * The operation has to be done in two parts.
 *
 * First each variable name is made into an array of
 * characters in flash memory.
 *
 * Then, another array of pointers to thos strings is created, also in flash
 */

SETTINGS_PARAMETERS(MAKE_STRINGS);

const char *const variableString[] PROGMEM = {
    SETTINGS_PARAMETERS(MAKE_NAMES)};

/***
 * The actual settings are stored in a struct which is fine for normal
 * use and programs can refer to them in the usual ways like:
 *    settings.age = 34;
 *
 * But, for some use cases, it is convenient to have an array. An array
 * can't be used to hold the settings because they are of different types
 * and array indexing is not a very semantic representation. It is also
 * less efficient.
 *
 * For those cases where the user wants to just iterate through the settings,
 * for example to dump them to the serial port, or read them from a device,
 * there is an array of pointers to the individual variables in the structure.
 *
 * The entries in the array are in the order of declaration in the original
 * SETTING_PARAMETERS list.
 *
 * The array is held in flash since the variables do not move and RAM is
 * always at a premium.
 */
void *const variablePointers[] PROGMEM = {SETTINGS_PARAMETERS(MAKE_POINTERS)};

/***
 * If the settings variables are to be referred to by index then another
 * array, also in flash, is needed to tell the code what type is being stored
 */
const TypeName variableType[] PROGMEM = {SETTINGS_PARAMETERS(MAKE_TYPES)};

/***
 * Now store a copy of all the default values in FLASH in case we need them.
 * These defaults are the values as given in the SETTINGS_PARAMETERS list in the
 * header file. They cannot be changed at run time
 */
const Settings defaults PROGMEM = {SETTINGS_PARAMETERS(MAKE_DEFAULTS)};

/***
 * Finally, it is time to define the actaul, global working copy of the
 * settings in RAM and initialise it with the default values.
 *
 * This operation gets performed as part of the data initialisation
 * at start up.
 *
 * If there are settings stored in EEPROM that should be used, they will
 * need to be checked for and loaded from EEPROM in the setup() user code.
 */
Settings settings = {SETTINGS_PARAMETERS(MAKE_DEFAULTS)};

/***
 * All the flash-based arrays are of the same length so any one of them
 * will do to calculate the number of settings entries.
 */
constexpr int SETTINGS_SIZE = sizeof(variableType) / sizeof(variableType[0]);
const int get_settings_count()
{
    static_assert(SETTINGS_SIZE <= SETTING_MAX_SIZE, "SETTINGS TOO BIG");
    return SETTINGS_SIZE;
}

/***
 * utility function to send all the current settings values to the serial
 * device.
 */
void dump_settings(const int dp)
{
    Serial.println();
    for (int i = 0; i < get_settings_count(); i++)
    {
        print_setting(i, dp);
    }
}

void reset_eeprom_settings_to_defaults()
{
    restore_default_settings();
    save_settings_to_eeprom();
}

void save_settings_to_eeprom()
{
    EEPROM.put(SETTINGS_EEPROM_ADDRESS, settings);
}

void load_settings_from_eeprom(bool verbose)
{
    Settings eeprom_settings;
    EEPROM.get(SETTINGS_EEPROM_ADDRESS, eeprom_settings);
    if (eeprom_settings.revision == SETTINGS_REVISION)
    {
        settings = eeprom_settings;
    }
    else
    {
        if (verbose)
        {
            Serial.println(F("settings updated."));
        }
        reset_eeprom_settings_to_defaults();
    }
}

/***
 * Utility function to send details of a single settings variable over
 * the serial device.
 *
 * The variable is identified by its index, i.
 */
void print_setting(const int i, const int dp)
{
    Serial.print('$');
    Serial.print(i);

    Serial.print(' ');
    // Serial.print('=');
    // Serial.print('=');
    void *ptr = (void *)pgm_read_word_near(variablePointers + i);
    switch (pgm_read_byte_near(variableType + i))
    {
        case T_float:
            Serial.print(*reinterpret_cast<float *>(ptr), dp);
            break;
        case T_bool:
            Serial.print((*reinterpret_cast<bool *>(ptr)) ? F("true") : F("false"));
            break;
        case T_char:
            Serial.print(*reinterpret_cast<char *>(ptr));
            break;
        case T_uint32_t:
            Serial.print(*reinterpret_cast<uint32_t *>(ptr));
            break;
        case T_int:
            Serial.print(*reinterpret_cast<int *>(ptr));
            break;
        default:
            Serial.println(F(" unknown type"));
    }
    char buffer[32];
    Serial.print(' ');
    Serial.print('#');
    strncpy_P(buffer, (char *)pgm_read_word(&(variableString[i])), 31); // Necessary casts and dereferencing,
    Serial.print(buffer);
    Serial.println();
}

/***
 * For convenience, settings can be written to by index number without
 * knowing the variable name. This can be useful when loading settings
 * over a serial port since there is not a good way for the code to
 * locate a variable by name without a slow search of the names table.
 * (Though that is another approach)
 */
int write_setting(const int i, const char *valueString)
{
    void *ptr = (void *)pgm_read_word_near(variablePointers + i);
    switch (pgm_read_byte_near(variableType + i))
    {
        case T_float:
            *reinterpret_cast<float *>(ptr) = (float)atof(valueString);
            break;
        case T_bool:
            *reinterpret_cast<bool *>(ptr) = atoi(valueString) ? true : false;
            break;
        case T_char:
            *reinterpret_cast<char *>(ptr) = valueString[0];
            break;
        case T_uint32_t:
            *reinterpret_cast<uint32_t *>(ptr) = atoi(valueString);
            break;
        case T_int:
            *reinterpret_cast<int *>(ptr) = atoi(valueString);
            break;
        default:
            return -1;
    }
    return 0;
}

/***
 * A simple, dumb binary copy is a safe way to restore values from flash
 * because the structures are guaranteed to be identical
 */
int restore_default_settings()
{
    memcpy_P(&settings, &defaults, sizeof(defaults));
    return 0;
}
