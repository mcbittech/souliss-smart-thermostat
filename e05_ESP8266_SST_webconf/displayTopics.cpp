#include <Arduino.h>
#include "constants.h"
#include "display.h"
#include "language.h"
#include "Ucglib.h"
#include "menu.h"
#include "preferences.h"
int ibaseH = 0;
boolean bTopicsPageEnabled = false;
String strVoid = "    "; //4 cifre
int iPortion = 0;
int integerPartNumber;
String sTemp;
int iLenght;
int getHeightPortion(Ucglib_ILI9341_18x240x320_HWSPI *ucg) {
  return ucg->getHeight() / 3 ;
}
void printNumber(Ucglib_ILI9341_18x240x320_HWSPI ucg, float fVal, String sUnity, String sText) {

  if (fVal == 0) {
    ucg.print("----"); //4 caratteri
  } else {
    integerPartNumber = (int) fVal;
    sTemp = integerPartNumber;
    iLenght = 4 - sTemp.length();
    sTemp = "";

    if (fVal - integerPartNumber > 0) {
      //aggiunge gli spazi necessari per occupare sempre lo spazion di 4 caratteri
      for (int i = 2; i < iLenght; i++) //considero lo spazio necessario al punto ed al carattere decimale
      {
        sTemp = sTemp + " ";
      }
      //se esiste la parte decimale, ne stampa una sola cifra
      ucg.print(sTemp);
      ucg.print(arrotonda(fVal), 1);

    } else {
      //aggiunge gli spazi necessari per occupare sempre lo spazion di 4 caratteri
      for (int i = 0; i < iLenght; i++)
      {
        sTemp = sTemp + " ";
      }
      sTemp = sTemp + integerPartNumber;
      // omette la parte decimale se è uguale a zero
      ucg.print(sTemp);
    }
  }
  ucg.setFont(FONT_BIG_MIN_50_PERCENT);
  ucg.print(sUnity);
  ucg.print(sText);
}

void display_print(Ucglib_ILI9341_18x240x320_HWSPI ucg, float fVal, int idx) {
  ucg.setColor(topicsLayout[idx].labelColor[0],topicsLayout[idx].labelColor[1],topicsLayout[idx].labelColor[2]);
  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  ucg.setFont(FONT_BIG);

  ucg.setFontPosBaseline();
  //calcola ingombro testo
  String str = "    "; //4 cifre
  const char *c = str.c_str();
  iPortion = getHeightPortion(&ucg);
  ibaseH = ucg.getHeight()  + iPortion * (idx - 2) - iPortion / 2 + ucg.getFontAscent() / 2;

  ucg.setPrintPos(5 , ibaseH);
  SERIAL_OUT.println("IBASEH:"+String(ibaseH));
  //  ucg.clearScreen();

  printNumber(ucg, fVal, topicsLayout[idx].unity, topicsLayout[idx].text);
}

float fPrec_Value[6] = {};

void displayTopics(Ucglib_ILI9341_18x240x320_HWSPI ucg, int offset, float fSetpoint_Output[6]) {
  setOnetime_clear_SetpointPage();
  if ( getUIChanged()) {
    ucg.clearScreen();
    SERIAL_OUT.println("TOPICS PAGE " + String(offset / 3 + 1) + " - ClearScreen");
    display_print(ucg, fSetpoint_Output[offset+0], offset+0);
    display_print(ucg, fSetpoint_Output[offset+1], offset+1);
    display_print(ucg, fSetpoint_Output[offset+2], offset+2);
    resetUIChanged();
  } else {
    if (fPrec_Value[offset+0] != fSetpoint_Output[offset+0]) display_print(ucg, fSetpoint_Output[offset+0], offset+0);
    if (fPrec_Value[offset+1] != fSetpoint_Output[offset+1]) display_print(ucg, fSetpoint_Output[offset+1], offset+1);
    if (fPrec_Value[offset+2] != fSetpoint_Output[offset+2]) display_print(ucg, fSetpoint_Output[offset+2], offset+2);
  }

  fPrec_Value[offset+0] = fSetpoint_Output[offset+0];
  fPrec_Value[offset+1] = fSetpoint_Output[offset+1];
  fPrec_Value[offset+2] = fSetpoint_Output[offset+2];
}

void displayTopicsHomePageLayout2(Ucglib_ILI9341_18x240x320_HWSPI ucg, float f_Value[6], boolean forcerefresh) {

  //SERIAL_OUT.println("displayTopics Home Page Layout2");

  int coordinates[6][4] = {
  /*  label     value   */
  /*  lx    ly  tx   ty */
      175, 25,  195, 42,
      200, 63,  210, 82,
      210, 102, 215, 120,
      205, 138, 205, 155,
      193, 173, 188, 190,
      170, 208, 160, 225
  };

  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  ucg.setFont(FONT_SMALL);
  ucg.setFontPosTop();

  for ( int i = 0; i < 3 * TOPICSPAGESNUMBER; i++) {
    if (forcerefresh) {
      ucg.setFont(ucg_font_helvB12_hf);
      ucg.setColor(topicsLayout[i].labelColor[0],topicsLayout[i].labelColor[2],topicsLayout[i].labelColor[2]);
      ucg.setPrintPos(coordinates[i][0], coordinates[i][1]);
      ucg.print(topicsLayout[i].text);
    }
    if (forcerefresh || fPrec_Value[i] != f_Value[i] ) {
      ucg.setFont(ucg_font_helvB14_hf);
      ucg.setPrintPos(coordinates[i][2], coordinates[i][3]);
      ucg.setColor(0,0,0);
      ucg.print("          "); //clear old value
      ucg.setPrintPos(coordinates[i][2], coordinates[i][3]);
      ucg.setColor(topicsLayout[i].textColor[0],topicsLayout[i].textColor[2],topicsLayout[i].textColor[2]);
      ucg.print(f_Value[i]);
      if (topicsLayout[i].unity == "°") {
        ucg.print((char)176);
      } else {
        ucg.print(topicsLayout[i].unity);
      }
      fPrec_Value[i] = f_Value[i];
   }
 }
}

boolean getTopicsPageEnabled() {
  return bTopicsPageEnabled;
}

void setTopicsPageEnabled() {
  bTopicsPageEnabled = true;
}

void resetTopicsPageEnabled() {
  bTopicsPageEnabled = false;
}
