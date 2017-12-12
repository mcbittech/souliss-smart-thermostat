//DISPLAY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include "Ucglib.h"

void display_layout2_HomeScreen(Ucglib_ILI9341_18x240x320_HWSPI, float, float, float);
void display_layout2_Setpoint(Ucglib_ILI9341_18x240x320_HWSPI, float, boolean, boolean,boolean,boolean);
void display_layout2_print_datetime(Ucglib_ILI9341_18x240x320_HWSPI);
void display_layout2_print_circle_green(Ucglib_ILI9341_18x240x320_HWSPI);
void display_layout2_print_circle_white(Ucglib_ILI9341_18x240x320_HWSPI);
void display_layout2_print_circle_black(Ucglib_ILI9341_18x240x320_HWSPI);
void calcoloAndamento(Ucglib_ILI9341_18x240x320_HWSPI, float);

