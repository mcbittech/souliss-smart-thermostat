//DISPLAY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include "Ucglib.h"

void display_print(Ucglib_ILI9341_18x240x320_HWSPI, float, int);
void displayTopics(Ucglib_ILI9341_18x240x320_HWSPI, int, float fSetpoint_Output[6]);
void displayTopicsHomePageLayout2 (Ucglib_ILI9341_18x240x320_HWSPI, float f_Value[6], boolean);
void printNumber(Ucglib_ILI9341_18x240x320_HWSPI,float, float , String , String );

boolean getTopicsPageEnabled();
void setTopicsPageEnabled();
void resetTopicsPageEnabled();

int getWidthCenterW(Ucglib_ILI9341_18x240x320_HWSPI);
int getHeightCenterW(Ucglib_ILI9341_18x240x320_HWSPI);
