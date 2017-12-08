#include <Arduino.h>
#include <MenuSystem.h>


void initMenu();
MenuSystem* getMenu();

boolean getMenuEnabled();
void setMenuEnabled();
void resetMenuEnabled();

boolean getLayout1();
boolean getLayout2();
int getDisplayBright();
boolean getClock();
int getTimeZone();
boolean getDayLightSavingTime();
float getOffsetDHT();

boolean getLocalSystem();
boolean getCrono();
boolean getCronoSet();
boolean getProgCrono();
boolean getCronoLearn();
boolean getDoSystemReset();

boolean getUIChanged();
boolean getSystemChanged();
void setSystemChanged();
void setUIChanged();
void setSystem(boolean bVal);
void resetUIChanged();
void resetSystemChanged();

//boolean getFlag_initScreen();
//void setFlag_initScreen(boolean);

void on_item_ProgCrono_deselected();
void printMenuBody(Ucglib_ILI9341_18x240x320_HWSPI, boolean);
void printMenuMove(Ucglib_ILI9341_18x240x320_HWSPI);
void printMenu(Ucglib_ILI9341_18x240x320_HWSPI);
void ReadAllSettingsFromPreferences();
void ReadAllSettingsFromSPIFFS();

void on_change_setPoint(float);
float getSetpointFromSPIFFS();
