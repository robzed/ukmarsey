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
 * Macros derived from
 * https://stackoverflow.com/questions/201593/is-there-a-simple-way-to-convert-c-enum-to-string/238157#238157
 *
 * Storing and reading stuff in FLASH from:
 * https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
 *
 * SPOILERS: Some proper preprocessor arcana here.
 *           Look away now if you are squeamish
 *
 **************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

/***
 * The revision number can be used to indicate a change to the settins
 * structure. For example a type change for a variable or the addition or
 * deletion of a variable.
 *
 * Without some kind of indicator like this, the code on the target amy
 * try to load settings form EEPROM into a settings structure that is
 * different to the one used to store those settings.
 *
 * A change in the revision implies that the defaults have changed in some
 * significant manner. Normally this is a fundamental change, not just some
 * different default value for a variable.
 *
 * Another reason for a change in the revision might be for a codebase that
 * runs on a similar but different target.
 *
 * The only safe approach when that happens is to resto overwrite any settings
 * stored in EEPROM with the compiled defaults and load the working settings
 * with those values.
 *
 * NOTE: this means that any custom values in EEPROM will be lost.
 */

/***
 * The address of the copy stored in EEPROM must be fixed. Although the size of
 * the structure is known to the compiler, any program using the settings module
 * may be less aware of how much space is taken from EEPROM storage.
 *
 * That means that the user must keep track of the sizes of any objects held
 * in EEPROM
 */
const int SETTINGS_EEPROM_ADDRESS = 0x0000;
const int SETTING_MAX_SIZE = 64;

/***
 * First, list  all the types that will be used. Identifiers must all be of the
 * form T_xxxx where xxxx is any legal type name in C/C++
 */
enum TypeName : uint8_t
{
    T_char,
    T_bool,
    T_uint32_t,
    T_float,
    T_int,
};

// clang-format off
/***
 * Now create a list of all the settings variables. For each variable add a line
 * of the form
 *    ACTION( type, name, default)  \
 * where
 *    type is any valid C/C++ type
 *    name is a legal C/C++ identifier
 *    default is the value stored in flash as the default
 *
 * This list will be used to generate structures and populate them autumatically
 * at build time.
 *
 * NOTE: if the structure is changes, update SETTINGS_REVISION
 */
// TODO: Many, or all, of these are going to be robot specific
// TODO: use macros efined in the robot_config.h file for the values.
// TODO: should the revision be a value separate form the actual parameters?
const int SETTINGS_REVISION = 1005;
#define SETTINGS_PARAMETERS(ACTION)             \
    ACTION(int, revision, SETTINGS_REVISION)    \
    ACTION(float, fwd_kp, 0.010)                \
    ACTION(float, fwd_ki, 0.500)                \
    ACTION(float, fwd_kd, 0.000)                \
    ACTION(float, rot_kp, 0.010)                \
    ACTION(float, rot_ki, 0.500)                \
    ACTION(float, rot_kd, 0.000)                \
    ACTION(float, k_velocity_ff, (1.0 / 280.0)) \
    ACTION(float, k_bias_ff, (23.0 / 280.0))    \


/***
 * These macros are going to be used to generate individual entries in
 * the data structures that are created later on.
 *
 * The macro name will be substituted for the string 'ACTION' in the list above
 *
 */
#define MAKE_STRINGS(CTYPE, VAR, VALUE) const PROGMEM char s_##VAR[] = #VAR;
#define MAKE_NAMES(CTYPE, VAR, VALUE) s_##VAR,
#define MAKE_TYPES(CTYPE, VAR, VALUE) T_##CTYPE,
#define MAKE_DEFAULTS(CTYPE, VAR, VALUE) .VAR = VALUE,
#define MAKE_STRUCT(CTYPE, VAR, VALUE) CTYPE VAR;
#define MAKE_POINTERS(CTYPE, VAR, VALUE) reinterpret_cast<void *>(&settings.VAR),
#define MAKE_CONFIG_ENTRY(CTYPE, VAR, VALUE) {#VAR, T_##CTYPE, reinterpret_cast<void *>(&config.VAR)},

// clang-format on

//
/***
 * define the structure that holds the settings
 *
 * Uses the MAKE_STRUCT macro to generate one line for each variable in the
 * parameters list. The result will be a basic struct definition like
 *
 * struct Settings {
 *   float mass;
 *   int quanity;
 *   bool loaded;
 * }
 */
struct Settings
{
    SETTINGS_PARAMETERS(MAKE_STRUCT)
};

/***
 * Now declare the two global instances of the settings
 */
extern Settings settings;       // the global working copy in RAM
extern const Settings defaults; // The coded-in defaults in flash
extern void *const variablePointers[] PROGMEM;
extern const TypeName variableType[] PROGMEM;
const int get_settings_count();
int restore_default_settings();

void save_settings_to_eeprom();
void load_settings_from_eeprom(bool verbose = false);

// send all to the serial device. sets displayed decimals
void dump_settings(const int dp = 2);

// send one setting to the serial device
void print_setting(const int i, const int dp = 2);

// write a setting by index number
int write_setting(const int i, const char *valueString);
/***
 * The templated version executes much faster because there are
 * no calls to the ascci_to_xxx converters.
 *
 * If the string converting version is never called, you will
 * save about 1k of flash. Unless you use atof() or atoi() elsewhere.
 */
template <class T>
int write_setting(const int i, const T value)
{
    if ((i == 0) or (i >= get_settings_count()))
    {
        return -1; // cannot change settings version
    }
    void *ptr = (void *)pgm_read_word_near(variablePointers + i);
    switch (pgm_read_byte_near(variableType + i))
    {
        case T_float:
            *reinterpret_cast<float *>(ptr) = value;
            break;
        case T_bool:
            *reinterpret_cast<bool *>(ptr) = value;
            break;
        case T_char:
            *reinterpret_cast<char *>(ptr) = value;
            break;
        case T_uint32_t:
            *reinterpret_cast<uint32_t *>(ptr) = value;
            break;
        case T_int:
            *reinterpret_cast<int *>(ptr) = value;
            break;
        default:
            return -1;
    }
    return 0;
}
#endif //SETTINGS_H
