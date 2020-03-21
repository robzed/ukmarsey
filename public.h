#ifndef PUBLIC_H
#define PUBLIC_H

void interpreter();

// provided by systick.cpp
void setupSystick();
void print_bat();
extern float gBatteryVolts;
extern byte gFunctionSwitch;
extern byte gDipSwitch;;


//

#endif

