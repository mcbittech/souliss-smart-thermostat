#include <Arduino.h>
#include "Ucglib.h"
#include "constants.h"
#include "menu.h"
#include "crono.h"
#include "ntp.h"
#include "display2.h"
#include "encoder.h"
#include "read_save.h"
#include "FS.h"
#include <ArduinoJson.h>


//GENERAL
///////////////////////////////////////////////////////////
bool pushed = 0;
bool changeday = 0;
bool changebox = 0;
bool delday = 0;
byte setSelected = 0;
byte daySelected = 0;
byte boxSelected = 0;
byte boxPointer = 0;
byte boxPointerView = 0;
byte hourSel_Box1 = 0;
byte hourSel_Box2 = 0;
int encoder0PinBLast1 = 0;
int longpress = 0;
int exitmain = 0;
bool ns = 0;
bool na = 0;
bool np = 0;
byte dS = 0;
byte gS = 0;
int i;
int ii;

//LAYOUT
///////////////////////////////////////////////////////////
byte start_x = 12;        //Start Position Layout X (pixel)
byte start_y = 110;        //Start Position Layout Y (pixel)
byte offset_x = 25;       //Offset between  columns (pixel)
byte offset_y = 75;       //Offset between  rows (pixel)
byte offset_text = 25;    //Offset between Text (pixel)
byte dim_x = 10;          //Box Dimension in X (pixel)
byte dim_y = 5;           //Box Dimension in Y (pixel)
byte dim_x_set = 10;      //Box Dimension in X (pixel)
byte dim_y_set = 10;      //Box Dimension in Y (pixel)
byte texthour = 0;        //Text to be write (hour index)
byte spacing = 1;         //Spacing between columns (pixel)
byte spacing1 = 0;

//SELECTION
///////////////////////////////////////////////////////////
byte dDaysel = 1;         //Day Selected
byte lastDaysel = 0;
byte lastBoxsel = 0;
byte line = 0;
byte dHourSel[8][48] = {0};   //Array Matrix
byte dHourSelCP[1][48] = {0}; //Array Matrix
float setP[5] = { 18.0, 20.0, 21.5, 23.0 };               //Setpoint Eco,Normal,Comfort,Comfort+
char* descP[5] = {"Eco", "Normal", "Comfort", "Comfort+"}; //Setpoint Eco,Normal,Comfort,Comfort+
int colour[4][3] = { { 102, 255, 0 }, { 255, 255, 153 }, { 255, 204, 0 }, { 255, 0, 0 } }; //da sx +chiaro a -chiaro



//drawCrono
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawCrono(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  //CANCELLO SCHERMO
  exitmain = 0;
  ucg.setColor(0, 0, 0);                                      //Nero
  ucg.drawBox(0, 0, ucg.getWidth(), ucg.getHeight());
  //Quadri BASE
  ucg.setColor(255, 255, 255);                                //Bianco
  ucg.setFont(ucg_font_helvB12_hf);
  for (byte nv = 0; nv < 2; nv++) {
    //Serial.print("VERTICALE ");Serial.println(nv);
    for (byte nh = 0; nh < 12; nh++) {
      ucg.setPrintPos(start_x + (offset_x * nh) + (spacing * 2), (start_y + (offset_y * nv)) + offset_text);
      ucg.print(texthour);
      for (byte nc = 0; nc < 2; nc++) {
        ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , start_y + (offset_y * nv) , dim_x , dim_y);
      }
      texthour++;
    }
  }
  texthour = 0;
  drawSetpoint(ucg);
  setSetpoint(ucg);
}


//drawSetpoint
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawSetpoint(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  //NEW SETPOINT
  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  for (int s = 0; s < 4; s++) {
    ucg.setColor(255, 255, 255);                                    //Bianco
    ucg.setFont(ucg_font_helvB10_hf);
    ucg.setPrintPos((start_x * 2) + (72 * s), 20);
    ucg.print(descP[s]);
    ucg.setColor(colour[s][0], colour[s][1], colour[s][2]);         //Colore Variabile
    ucg.setFont(ucg_font_helvB18_hf);
    ucg.setPrintPos((start_x * 2) + (72 * s), 45);
    ucg.print(setP[s], 1);
    ucg.drawBox((start_x * 2) + (72 * s) , 50 , dim_x * 5 , dim_y);
  }
}


//setSetpoint
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setSetpoint(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  //NEW SETPOINT
  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  setSelected = 0;
  setEncoderValue(setP[setSelected]);
  Serial.print("setSetpoint setEncoderValue"); Serial.print(" P:"); Serial.print(setSelected); Serial.print(" "); Serial.println(setP[setSelected]);
  while (pushed == 0 && setSelected < 4) {
    ucg.setColor(255, 255, 255);                                //Bianco
    ucg.drawBox((start_x * 2) + (72 * setSelected) , 50 , dim_x * 5 , dim_y);
    ucg.setColor(colour[setSelected][0], colour[setSelected][1], colour[setSelected][2]);        //Colore Variabile
    ucg.setFont(ucg_font_helvB18_hf);
    ucg.setPrintPos((start_x * 2) + (72 * setSelected), 45);
    ucg.print(setP[setSelected], 1);
    setP[setSelected] = getEncoderValue();
    delay(50);

    //ENCODER
    //////////////////////////////////////////////////////////////
    if ((digitalRead(ENCODER_SWITCH) == LOW && (ns == HIGH)) && setSelected < 3) {
      setSelected++;
      setEncoderValue(setP[setSelected]);
      Serial.print("setSetpoint setEncoderValue"); Serial.print(" P:"); Serial.print(setSelected); Serial.print(" "); Serial.println(setP[setSelected]);
      delay(250);
    }
    ns = digitalRead(ENCODER_SWITCH);

    //ESCAPE FROM WHILE
    //////////////////////////////////////////////////////////////
    if (digitalRead(ENCODER_SWITCH) == LOW & setSelected >= 3) {
      pushed = 1;
    }
    else {
      pushed = 0;
    }

    //MAX setSelected
    //////////////////////////////////////////////////////////////
    if (setSelected >= 3) {
      setSelected = 3;
    }
    delay(1);
  }//endwhile
  for (int f = 0; f < 4; f++) {
    ucg.setColor(colour[f][0], colour[f][1], colour[f][2]);        //Colore Variabile
    ucg.drawBox((start_x * 2) + (72 * f) , 50 , dim_x * 5 , dim_y);
  }
  pushed = 0;
}//end setSetpoint


//setDay
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setDay(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  Serial.print("MENU' setDay ");
  ucg.setColor(102, 255, 0);           // Verde Chiaro
  ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
  ucg.setFont(ucg_font_helvB14_hf);
  ucg.setPrintPos(70, 235);
#ifdef LANG_en_US
//@rmackinnon: Addition of en_US locale
  ucg.print("DAY:");
#else
  ucg.print("GIORNO:");
#endif
  changeday = 1;
  while (pushed == 0) {
    if (changeday == 1) {
      Serial.print("dDaysel "); Serial.println(dDaysel);
      ucg.setColor(0, 0, 0);            //Nero
      ucg.setPrintPos(160, 235);
      ucg.drawBox(155, 220, 150, 20);
      ucg.setColor(255, 255, 255);      //Bianco
      ucg.setPrintPos(160, 235);
      daySelected = dDaysel;
#ifdef LANG_en_US
//@rmackinnon: Addition of en_US locale
      switch (dDaysel) {
        case 1:
          ucg.print("SUNDAY");
          drawBoxes(ucg);
          break;
        case 2:
          ucg.print("MONDAY");
          drawBoxes(ucg);
          break;
        case 3:
          ucg.print("TUESDAY");
          drawBoxes(ucg);
          break;
        case 4:
          ucg.print("WEDNESDAY");
          drawBoxes(ucg);
          break;
        case 5:
          ucg.print("THURSDAY");
          drawBoxes(ucg);
          break;
        case 6:
          ucg.print("FRIDAY");
          drawBoxes(ucg);
          break;
        case 7:
          ucg.print("SATURDAY");
          drawBoxes(ucg);
#else
      switch (dDaysel) {
        case 1:
          ucg.print("DOMENICA");
          drawBoxes(ucg);
          break;
        case 2:
          ucg.print("LUNEDI'");
          drawBoxes(ucg);
          break;
        case 3:
          ucg.print("MARTEDI'");
          drawBoxes(ucg);
          break;
        case 4:
          ucg.print("MERCOLEDI'");
          drawBoxes(ucg);
          break;
        case 5:
          ucg.print("GIOVEDI'");
          drawBoxes(ucg);
          break;
        case 6:
          ucg.print("VENERDI'");
          drawBoxes(ucg);
          break;
        case 7:
          ucg.print("SABATO");
          drawBoxes(ucg);
#endif
          break;
        default:
          break;
      }
      changeday = 0;
    }
    delay(1);

    //ENCODER
    //////////////////////////////////////////////////////////////
    na = digitalRead(ENCODER_PIN_B);
    if ((encoder0PinBLast1 == LOW) && (na == HIGH)) {
      dDaysel++;
      delay(50);
    }
    encoder0PinBLast1 = na;

    //MIN-MAX dDaysel
    //////////////////////////////////////////////////////////////
    if (dDaysel < 1) {
      dDaysel = 7;
    } else if (dDaysel > 7) {
      dDaysel = 1;
    }

    //ESCAPE FROM WHILE
    //////////////////////////////////////////////////////////////
    if (digitalRead(ENCODER_SWITCH) == LOW) {
      pushed = 1;
    }
    else {
      pushed = 0;
    }

    //OnChange
    //////////////////////////////////////////////////////////////
    if (dDaysel != lastDaysel) {
      changeday = 1;
    }
    lastDaysel = dDaysel;
    //////////////////////////////////////////////////////////////////////////////////////

  }//endwhile
  ucg.setColor(255, 255, 255);                                //Bianco
  ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
  ucg.setFont(ucg_font_helvB14_hf);
  ucg.setPrintPos(70, 235);
  ucg.print("GIORNO:");
  pushed = 0;
  daySelected = dDaysel;
  Serial.print("daySelected="); Serial.println(daySelected);
  delay(250);
}


//drawBoxes from array
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawBoxes(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  Serial.println("MENU' drawBoxes ");
  for (byte nv = 0; nv < 2; nv++) {
    //Serial.print("VERTICALE ");Serial.println(nv);
    for (byte nh = 0; nh < 12; nh++) {
      for (byte nc = 0; nc < 2; nc++) {
        //Take data from array matrix
        hourSel_Box1 = dHourSel[daySelected][boxPointer];
        //Serial.print("daySelected+boxPointer=");Serial.print(daySelected);Serial.print("  ");Serial.println(boxPointer);
        if (hourSel_Box1 > 0) {
          //Box C1:nh:1
          ucg.setColor(colour[hourSel_Box1 - 1][0], colour[hourSel_Box1 - 1][1], colour[hourSel_Box1 - 1][2]);  //Colore Variabile
          ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2)) + (offset_y * nv) , dim_x_set , dim_y_set);
          if (hourSel_Box1 > 1) {
            //Box C1:nh:2
            ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * nv) , dim_x_set , dim_y_set);
            if (hourSel_Box1 > 2) {
              //Box C1:nh:3
              ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * nv) , dim_x_set , dim_y_set);
              if (hourSel_Box1 > 3) {
                //Box C1:nh:4
                ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * nv) , dim_x_set , dim_y_set);
              } else {
                ucg.setColor(0, 0, 0);            //Nero
                ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * nv) , dim_x_set , dim_y_set);
              }
            } else {
              ucg.setColor(0, 0, 0);            //Nero
              ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * nv) , dim_x_set , dim_y_set);
              ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * nv) , dim_x_set , dim_y_set);
            }
          } else {
            ucg.setColor(0, 0, 0);            //Nero
            ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * nv) , dim_x_set , dim_y_set);
            ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * nv) , dim_x_set , dim_y_set);
            ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * nv) , dim_x_set , dim_y_set);
          }
        } else {
          ucg.setColor(0, 0, 0);            //Nero
          ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2)) + (offset_y * nv) , dim_x_set , dim_y_set);
          ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * nv) , dim_x_set , dim_y_set);
          ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * nv) , dim_x_set , dim_y_set);
          ucg.drawBox(start_x + (offset_x * nh) + (dim_x * nc) + (spacing * nc) , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * nv) , dim_x_set , dim_y_set);
        }
        boxPointer++;
      }
    }
  }
  boxPointer = 0;
  boxPointerView = 0;
  spacing1 = 0;
  line = 0;
  delay(250);
}


//setBoxes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setBoxes(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  Serial.println("MENU' setBoxes ");
  while (pushed == 0) {
    if (changebox == 1) {
      ucg.setColor(102, 255, 0);           // Verde Chiaro
      ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , start_y + (offset_y * line) , dim_x , dim_y);
      //Serial.print("boxSelected ");Serial.println(boxSelected);
      //Serial.print("boxPointer ");Serial.println(boxPointer);
      switch (boxSelected) {
        case 0:
          dHourSel[daySelected][boxPointer] = 0;
          ucg.setColor(0, 0, 0);            //Nero
          //P1
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2)) + (offset_y * line), dim_x_set , dim_y_set);
          //P2
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * line) , dim_x_set , dim_y_set);
          //P3
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * line) , dim_x_set , dim_y_set);
          //P4
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * line) , dim_x_set , dim_y_set);
          //OverP4
          //ucg.drawBox(start_x+(offset_x*(boxPointerView/2))+spacing1 , (start_y-dim_y-(dim_y*2)-(dim_y_set*4))+(offset_y*line) , dim_x_set , dim_y_set);
          //Serial.print("daySelected+boxPointer=");Serial.print(daySelected);Serial.print("  ");Serial.print(boxPointer);Serial.print(" = ");Serial.print("boxSelected ");Serial.println(boxSelected);
          break;
        case 1:
          dHourSel[daySelected][boxPointer] = 1;
          //ucg.setColor(102, 255, 0);           // Verde Chiaro
          ucg.setColor(colour[boxSelected - 1][0], colour[boxSelected - 1][1], colour[boxSelected - 1][2]);  //Colore Variabile
          //P1
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2)) + (offset_y * line), dim_x_set , dim_y_set);
          ucg.setColor(0, 0, 0);            //Nero
          //P2
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * line) , dim_x_set , dim_y_set);
          //P3
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * line) , dim_x_set , dim_y_set);
          //P4
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * line) , dim_x_set , dim_y_set);
          //Serial.print("daySelected+boxPointer=");Serial.print(daySelected);Serial.print("  ");Serial.print(boxPointer);Serial.print(" = ");Serial.print("boxSelected ");Serial.println(boxSelected);
          break;
        case 2:
          dHourSel[daySelected][boxPointer] = 2;
          //ucg.setColor(102, 255, 0);           // Verde Chiaro
          ucg.setColor(colour[boxSelected - 1][0], colour[boxSelected - 1][1], colour[boxSelected - 1][2]);  //Colore Variabile
          //P1
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2)) + (offset_y * line), dim_x_set , dim_y_set);
          //P2
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * line) , dim_x_set , dim_y_set);
          ucg.setColor(0, 0, 0);            //Nero
          //P3
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * line) , dim_x_set , dim_y_set);
          //P4
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * line) , dim_x_set , dim_y_set);
          //Serial.print("daySelected+boxPointer=");Serial.print(daySelected);Serial.print("  ");Serial.print(boxPointer);Serial.print(" = ");Serial.print("boxSelected ");Serial.println(boxSelected);
          break;
        case 3:
          dHourSel[daySelected][boxPointer] = 3;
          //ucg.setColor(102, 255, 0);           // Verde Chiaro
          ucg.setColor(colour[boxSelected - 1][0], colour[boxSelected - 1][1], colour[boxSelected - 1][2]);  //Colore Variabile
          //P1
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2)) + (offset_y * line), dim_x_set , dim_y_set);
          //P2
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * line) , dim_x_set , dim_y_set);
          //P3
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * line) , dim_x_set , dim_y_set);
          ucg.setColor(0, 0, 0);            //Nero
          //P4
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * line) , dim_x_set , dim_y_set);
          //Serial.print("daySelected+boxPointer=");Serial.print(daySelected);Serial.print("  ");Serial.print(boxPointer);Serial.print(" = ");Serial.print("boxSelected ");Serial.println(boxSelected);
          break;
        case 4:
          dHourSel[daySelected][boxPointer] = 4;
          //ucg.setColor(102, 255, 0);           // Verde Chiaro
          ucg.setColor(colour[boxSelected - 1][0], colour[boxSelected - 1][1], colour[boxSelected - 1][2]);  //Colore Variabile
          //P1
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2)) + (offset_y * line), dim_x_set , dim_y_set);
          //P2
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - dim_y_set) + (offset_y * line) , dim_x_set , dim_y_set);
          //P3
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 2)) + (offset_y * line) , dim_x_set , dim_y_set);
          //P4
          ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , (start_y - dim_y - (dim_y * 2) - (dim_y_set * 3)) + (offset_y * line) , dim_x_set , dim_y_set);
          //Serial.print("daySelected+boxPointer=");Serial.print(daySelected);Serial.print("  ");Serial.print(boxPointer);Serial.print(" = ");Serial.print("boxSelected ");Serial.println(boxSelected);
          break;
        default:
          break;
      }
      //ucg.setRotate90();
      changebox = 0;
    }


    //ENCODER
    //////////////////////////////////////////////////////////////
    na = digitalRead(ENCODER_PIN_B);
    if ((encoder0PinBLast1 == LOW) && (na == HIGH)) {
      boxSelected++;
      delay(250);
    }
    encoder0PinBLast1 = na;

    //MIN-MAX dDaysel
    //////////////////////////////////////////////////////////////
    if (boxSelected < 0) {
      boxSelected = 4;
    } else if (boxSelected > 4) {
      boxSelected = 0;
    }

    //Adding boxPointer
    //////////////////////////////////////////////////////////////
    if (digitalRead(ENCODER_SWITCH) == LOW && (np == HIGH)) {
      ucg.setColor(255, 255, 255);      //Bianco
      ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , start_y + (offset_y * line) , dim_x , dim_y);
      boxPointer++;
      boxPointerView++;
      if (spacing1 == 0) {
        spacing1 = dim_x + spacing;
      } else {
        spacing1 = 0;
      }
      if (boxPointerView > 23) {
        boxPointerView = 0;
      }
      if (boxPointer > 23) {
        line = 1;
      }
      if (boxPointer > 47) {
        boxPointer = 0;
        line = 0;
      }
      //Serial.print("boxPointer= ");Serial.println(boxPointer);
      //Serial.print("boxPointerView= ");Serial.println(boxPointerView);
      //Serial.print("spacing1= ");Serial.println(spacing1);
      //Serial.print("line= ");Serial.println(line);
      changebox = 1;
    }

    np = digitalRead(ENCODER_SWITCH);


    //ESCAPE FROM WHILE with longpress
    //////////////////////////////////////////////////////////////

    //ESCAPE TO DAYS SELECTION
    if ((longpress >= 600 & longpress < 700) & np == HIGH) {
      ucg.setColor(0, 0, 0);                 //Nero
      ucg.drawBox(3, 217, 60, 21);           //Rettangolo basso sx
      longpress = 0;
      Serial.println("longpress 1");
      exitmain = 0;
      break;
    }
    else {
      pushed = 0;
    }

    //ESCAPE TO COPY DAY
    if ((longpress >= 700 & longpress < 800) & np == HIGH) {
      ucg.setColor(0, 0, 0);                 //Nero
      ucg.drawBox(3, 217, 60, 21);           //Rettangolo basso sx
      longpress = 0;
      Serial.println("longpress 3");
      copyDay(daySelected);
      exitmain = 0;
      break;
    }
    else {
      pushed = 0;
    }

    //ESCAPE TO PASTE DAY
    if ((longpress >= 800 & longpress < 900) & np == HIGH) {
      ucg.setColor(0, 0, 0);                 //Nero
      ucg.drawBox(3, 217, 60, 21);           //Rettangolo basso sx
      longpress = 0;
      Serial.println("longpress 4");
      pasteDay(daySelected);
      exitmain = 0;
      break;
    }
    else {
      pushed = 0;
    }

    //ESCAPE TO SAVE CRONO
    if ((longpress >= 900 & longpress < 1000) & np == HIGH) {
      ucg.setColor(0, 0, 0);                 //Nero
      ucg.drawBox(3, 217, 60, 21);           //Rettangolo basso sx
      longpress = 0;
      Serial.println("Saving Crono Program... ");
      Serial.println("longpress 2");
      //SPIFFS SAVE
      SaveCronoMatrixSPIFFS(ucg);
      exitmain = 0;
      break;
    }

    //ESCAPE TO MAIN
    if ((longpress >= 500 & longpress < 600) & np == HIGH) {
      longpress = 0;
      Serial.println("Exit to main... ");
      Serial.println("longpress 0");
      pushed = 1;
      exitmain = 1;
      break;
    }

    //ESCAPE COUNTER
    if (np == LOW) {
      longpress++;
      //Serial.print("longpress ");Serial.println(longpress);
    } else {
      longpress = 0;
      ucg.setColor(0, 0, 0);                 //Nero
      ucg.drawBox(3, 217, 60, 21);           //Rettangolo basso sx
    }



    //ESCAPE VISUALIZATION
    if (longpress > 500 & longpress < 600) {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_helvB10_hf);
      ucg.setColor(102, 255, 0);              // Verde Chiaro
      ucg.setPrintPos(5, 233);
      ucg.print("EXIT");
    } else if (longpress > 600 & longpress < 700) {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_helvB10_hf);
      ucg.setColor(102, 255, 0);              // Verde Chiaro
      ucg.setPrintPos(5, 233);
      ucg.print("DAYS");
    } else if (longpress > 700 & longpress < 800) {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_helvB10_hf);
      ucg.setColor(102, 255, 0);              // Verde Chiaro
      ucg.setPrintPos(5, 233);
      ucg.print("COPY");
    } else if (longpress > 800 & longpress < 900) {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_helvB10_hf);
      ucg.setColor(102, 255, 0);              // Verde Chiaro
      ucg.setPrintPos(5, 233);
      ucg.print("PASTE");
    } else if (longpress > 900 & longpress < 1000) {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_helvB10_hf);
      ucg.setColor(102, 255, 0);              // Verde Chiaro
      ucg.setPrintPos(5, 233);
      ucg.print("SAVE  ");
    }
    //EXIT TO MENU'

    //OnChange
    //////////////////////////////////////////////////////////////
    if (boxSelected != lastBoxsel) {
      changebox = 1;
    }
    lastBoxsel = boxSelected;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    delay(1);
  }
  ucg.setColor(255, 255, 255);      //Bianco
  ucg.drawBox(start_x + (offset_x * (boxPointerView / 2)) + spacing1 , start_y + (offset_y * line) , dim_x , dim_y);
  boxPointer = 0;
  boxPointerView = 0;
  pushed = 0;
}

//ESCAPE FROM CRONO BIT
boolean exitmainmenu() {
  on_item_ProgCrono_deselected();
  return exitmain;
}

void SaveCronoMatrixSPIFFS (Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  SPIFFS.begin();
  DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["Sp0"] = setP[0];
  root["Sp1"] = setP[1];
  root["Sp2"] = setP[2];
  root["Sp3"] = setP[3];

  JsonArray& cronomatrix = root.createNestedArray("cronomatrix");
  //JsonArray& cronomatrix = jsonBuffer.createArray();
  dS = 0;
  gS = 0;
  i = 0;
  yield;
  for (byte dS = 1; dS < 8; dS++) {
    for (byte gS = 0; gS < 48; gS++) {
      ucg.setColor(102, 255, 0);              //Verde Chiaro
      ucg.drawBox(3, 217, 60, 21);            //Rettangolo basso sx
      ucg.setColor(255, 255, 255);            //Bianco
      ucg.setFont(ucg_font_helvB10_hf);
      ucg.setPrintPos(5, 233);
      ucg.print("SAVING");
      cronomatrix.add(dHourSel[dS][gS]);
      byte pS = dHourSel[dS][gS];
      Serial.print("Saving in SPIFFS : "); Serial.print(i); Serial.print(" day "); Serial.print(dS); Serial.print(" hour/2 "); Serial.print(gS); Serial.print(" value "); Serial.println(pS);
      //testati delay fino a 100 !
      delay(1);
      i++;
      ucg.setColor(0, 0, 0);                  //Nero
      ucg.drawBox(3, 217, 60, 21);            //Rettangolo basso sx
    }
    gS = 0;
  }
  dS = 0;
  i = 0;

  Serial.print("Ecco i dati in json: ");
  //cronomatrix.printTo(Serial);
  root.printTo(Serial);
  char buffer[1000];
  root.printTo(buffer, sizeof(buffer));
  Serial.println();

  // open file for writing
  File sst_spiffs = SPIFFS.open("/sst_crono_matrix.json", "w");
  if (!sst_spiffs) {
    Serial.println("Failed to open sst_crono_matrix.json");
    return;
  }
  //qui salvo il buffer su file
  sst_spiffs.println(buffer);
  Serial.println("Salvo in SPIFFS il buffer con i settings :"); Serial.println(buffer);
  delay(1);
  //chiudo il file
  sst_spiffs.close();
}

void ReadCronoMatrixSPIFFS() {

  Serial.println("Read Crono Settings from SPIFFS...");
  File  cronomatrix_inlettura = SPIFFS.open("/sst_crono_matrix.json", "r");
  if (!cronomatrix_inlettura) {
    Serial.println("Failed to open sst_crono_matrix.json");
    return;
  }


  String risultatocronomatrix = cronomatrix_inlettura.readStringUntil('\n');
  //Serial.print("Ho letto dal file : ");Serial.println(risultatocronomatrix);
  char jsoncronomatrix[1000];
  risultatocronomatrix.toCharArray(jsoncronomatrix, 1000);
  Serial.print("Imposto i setpoint del crono: "); Serial.println(jsoncronomatrix);
  //StaticJsonBuffer<200> jsonBuffer_cronomatrix_inlettura;
  DynamicJsonBuffer jsonBuffer_cronomatrix_inlettura;

  JsonObject& rootcronomatrix_inlettura = jsonBuffer_cronomatrix_inlettura.parseObject(jsoncronomatrix);
  if (!rootcronomatrix_inlettura.success()) {
    Serial.println("parseObject() failed");
  }
  setP[0] = rootcronomatrix_inlettura["Sp0"];
  setP[1] = rootcronomatrix_inlettura["Sp1"];
  setP[2] = rootcronomatrix_inlettura["Sp2"];
  setP[3] = rootcronomatrix_inlettura["Sp3"];

  dS = 0;
  gS = 0;
  ii = 0;
  for (byte dS = 1; dS < 8; dS++) {
    for (byte gS = 0; gS < 48; gS++) {
      dHourSel[dS][gS] = rootcronomatrix_inlettura["cronomatrix"][ii];
      byte pS = dHourSel[dS][gS];
      Serial.print("Reading from SPIFFS : "); Serial.print(" day "); Serial.print(dS); Serial.print(" hour/2 "); Serial.print(gS); Serial.print(" value "); Serial.println(pS);
      delay(1);
      ii++;
    }
    gS = 0;
  }
  dS = 0;
  ii = 0;
  cronomatrix_inlettura.close();
}

void copyDay(int dayCopy) {
  for (int i = 0; i < 48; i++) {
    dHourSelCP[0][i] = dHourSel[dayCopy][i];
  }
}


void pasteDay(int dayPaste) {
  for (int i = 0; i < 48; i++) {
    dHourSel[dayPaste][i] = dHourSelCP[0][i];
  }
}

//Hook crono data from NTP Time
/*
  byte dHourSel[8][48]={0};   //Array Matrix
  float setP[5] = { 18.0,20.0,21.5,23.0 };                  //Setpoint Eco,Normal,Comfort,Comfort+
  char* descP[5] = {"Eco","Normal","Comfort","Comfort+"};   //Setpoint Eco,Normal,Comfort,Comfort+
*/
float checkNTPcrono(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  float getsetpoint;
  int deyweek = getNTPday();
  int hourday = getNTPhour();
  int minuteday = getNTPminute();
  int minute_30_59;
  int pointernow = dHourSel[deyweek][(hourday * 2) + minute_30_59];
  if (minuteday > 30) {
    minute_30_59 = 1;
  } else {
    minute_30_59 = 0;
  }
  //Serial.print("dayweek: ");Serial.print(deyweek);Serial.print("  hourday*2: ");Serial.print(hourday*2);Serial.print("  minuteday: ");Serial.println(minute_30_59);
  //Serial.print("pointernow ");Serial.println(pointernow);
  if (pointernow >= 0 && pointernow <= 4) {
    ucg.setColor(colour[pointernow - 1][0], colour[pointernow - 1][1], colour[pointernow - 1][2]);  //Colore Variabile
    switch (pointernow) {
      case 0:
        getsetpoint = (setP[0] - 2.0);         //da sistemare con taglio uscita T31
        Serial.println("CRONO: Off");
        break;
      case 1:
        getsetpoint = setP[0];
        Serial.println("CRONO: Attivo Eco");
        break;
      case 2:
        getsetpoint = setP[1];
        Serial.println("CRONO: Attivo Normal");
        break;
      case 3:
        getsetpoint = setP[2];
        Serial.println("CRONO: Attivo Comfort");
        break;
      case 4:
        getsetpoint = setP[3];
        Serial.println("CRONO: Attivo Comfort+");
        break;
      default:
        break;
    }
  }
  return getsetpoint;
}
float checkCronoStatus(Ucglib_ILI9341_18x240x320_HWSPI ucg) {
  float getsetpoint;
  int deyweek = getNTPday();
  int hourday = getNTPhour();
  int minuteday = getNTPminute();
  int minute_30_59;
  int pointernow = dHourSel[deyweek][(hourday * 2) + minute_30_59];
  
  return dHourSel[deyweek][(hourday * 2) + minute_30_59];;
}


