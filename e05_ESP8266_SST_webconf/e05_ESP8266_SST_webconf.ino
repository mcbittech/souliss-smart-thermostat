/**************************************************************************
  Wi-Fi Smart Thermostat based on Souliss IoT Framework
    -Olimex ESP8266-EVB
    -ILI9341 with SPI connection, via UEXT connector
    -Rotary Encoder with pushbutton & status LED
    -DHT22 Temperature & Humidity Sensor
  This example is only supported on ESP8266.
  Developed by mcbittech & fazioa
***************************************************************************/
// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// Configure the Souliss framework
#include "bconf/MCU_ESP8266.h"              /** Load the code directly on the ESP8266 */
#include "preferences.h"
#include "multiClick.h"

#if(DYNAMIC_CONNECTION)
#define USEEEPROM_INSKETCH
#define USEEEPROM             1
#include "conf/RuntimeGateway.h"            // This node is a Peer and can became a Gateway at runtime
#include "conf/DynamicAddressing.h"         // Use dynamically assigned addresses
//#include "conf/WEBCONFinterface.h"          // Enable the WebConfig interface
#include "conf/WEBCONFASYNCinterface.h"     // Enable the WebConfig interface
#include "connection_dynamic.h"
#else
#include "conf/IPBroadcast.h"
#include "connection_static.h"
#endif



#include "Souliss.h"
#include "encoder.h"
#include "constants.h"
#include "display.h"
#include "display2.h"
#include "displayTopics.h"
#include "language.h"
#include "ntp.h"
#include <TimeLib.h>
#include <MenuSystem.h>
#include "menu.h"
#include "crono.h"
#include "datalogger.h"
#include "read_save.h"
#include "topics.h"
#include "pagesStruct.h"
//*************************************************************************
//*************************************************************************
Page SSTPage;

DHT dht(DHTPIN, DHTTYPE);
float temperature = 0;
float humidity = 0;
float fVal = 0;
float setpoint = 0;
float encoderValue_prec = 0;

//DISPLAY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SPI.h>
#include <Arduino.h>
#include "Ucglib.h"

int backLEDvalue = 0;
int backLEDvalueHIGH = BRIGHT_MAX;
int backLEDvalueLOW = BRIGHT_MIN_DEFAULT;
bool FADE = 1;
uint8_t mypayload_len = 0;
uint8_t mypayload[2];
float fTopic_C1_Output;
float fTopic_C2_Output;
float fTopic_C3_Output;
float fTopic_C4_Output;
float fTopic_C5_Output;
float fTopic_C6_Output;

boolean bChildLock = false;

// Menu
MenuSystem* myMenu;

// Use hardware SPI
Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 2 , /*cs=*/ 15);

//WBServer
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef DYNAMIC_CONNECTION
AsyncWebServer web_server(80);
//#endif
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
File fsUploadFile;
uint8_t buff{};

String S_temperature_WBS = "22.3";
String S_setpoint_WBS = "22.4";
bool S_relestatus_WBS = 0;
String S_humidity_WBS = "58.8";
String S_nextstep_WBS;
String S_filena_WBS;
bool B_away_WBS = 0;
bool B_powerfull_WBS = 0;
bool B_is_away_WBS = 0;
bool B_is_powerfull_WBS = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup()
{

  SERIAL_OUT.begin(115200);
  Serial.setDebugOutput(true);  //debug WBServer


  //SPIFFS
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  SPIFFS.begin();
  File  sst_spiffs_verifica = SPIFFS.open("/sst_settings.json", "r");
  if (!sst_spiffs_verifica) {
    Serial.println(" ");
    Serial.println("Non riesco a leggere sst_settings.json! formatto la SPIFFS...");
    SPIFFS.format();
    Serial.println("Spiffs formatted");
    ReadAllSettingsFromPreferences();
    ReadCronoMatrixSPIFFS();
  }
  else
  {
    ReadAllSettingsFromSPIFFS();
    ReadCronoMatrixSPIFFS();
    backLEDvalueLOW = getDisplayBright();
    setpoint = getSetpointFromSPIFFS();
    setEncoderValue(setpoint);
  }

  //DISPLAY INIT
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  ucg.begin(UCG_FONT_MODE_SOLID);

  //SPI Frequency
  SPI.setFrequency(80000000);

  ucg.setColor(0, 0, 0);
  ucg.setRotate90();
  //BACK LED
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  digitalWrite(BACKLED, HIGH);
  pinMode(BACKLED, OUTPUT);                     // Background Display LED

  display_print_splash_screen(ucg);
  Initialize();
  extern AsyncWebServer web_server;
#if(DYNAMIC_CONNECTION)
  DYNAMIC_CONNECTION_Init();
#else
#if(DHCP_OPTION)
  STATIC_CONNECTION_Init_DHCP();
#else
  STATIC_CONNECTION_Init_STATICIP();
#endif
#endif


  //*************************************************************************
  //*************************************************************************
  Set_T52(SLOT_TEMPERATURE);
  Set_T53(SLOT_HUMIDITY);
  Set_T19(SLOT_BRIGHT_DISPLAY);
  Set_T11(SLOT_AWAY);

  //set default mode
  Set_Thermostat(SLOT_THERMOSTAT);
  set_ThermostatModeOn(SLOT_THERMOSTAT);
  set_DisplayMinBright(SLOT_BRIGHT_DISPLAY, backLEDvalueLOW);
  //*************************************************************************
  //*************************************************************************

  // Define output pins
  pinMode(RELE, OUTPUT);    // Heater
  dht.begin();

  //ENCODER
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode (ENCODER_PIN_A, INPUT_PULLUP);
  pinMode (ENCODER_PIN_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderFunction, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoderFunction, CHANGE);
  // SWITCH ENCODER
  digitalWrite(BACKLED, HIGH);
  pinMode(ENCODER_SWITCH, INPUT);

  //NTP
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  initNTP();
  delay(1000);
  //*************************************************************************
  //*************************************************************************

  //MENU
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  initMenu();
  myMenu = getMenu();


  //OTA-WBServer
  setup_OTA_WBServer();

  // Init HomeScreen
  initScreen();
}

void loop()
{
  EXECUTEFAST() {
    UPDATEFAST();

    switch (SSTPage.actualPage) {
      case PAGE_MENU :
        if (getMenuEnabled() && !bChildLock) {
          //Bright high if menu enabled
          FADE = 1;
          //Menu Command Section
          if (getEncoderValue() != encoderValue_prec)
          {
            if (getEncoderValue() > encoderValue_prec) {
              //Menu DOWN
              myMenu->next();
            } else {
              //Menu UP
              myMenu->prev();
            }
            printMenuMove(ucg);
            encoderValue_prec = getEncoderValue();
          }
        }
        if (getMenuEnabled() && !digitalRead(ENCODER_SWITCH)) {
          //IF MENU ENABLED
          myMenu->select(true);
          ucg.clearScreen();
          printMenu(ucg);
        }

    }

    SHIFT_50ms(0) {
      //set point attuale
      setpoint = Souliss_SinglePrecisionFloating(memory_map + MaCaco_OUT_s + SLOT_THERMOSTAT + 3);
      //Stampa il setpoint solo se il valore dell'encoder è diverso da quello impostato nel T31
      switch (SSTPage.actualPage) {
        case PAGE_MENU :
          if (!getMenuEnabled()) {
            SSTPage.actualPage = PAGE_HOME;
            initScreen();
            setUIChanged();
          }
          break;
        case PAGE_CRONO :
          break;
        case PAGE_HOME:
          if (arrotonda(getEncoderValue()) != arrotonda(encoderValue_prec) && !bChildLock) {
            FADE = 1;
            //TICK TIMER
            timerDisplay_setpoint_Tick();
            //SETPOINT PAGE ////////////////////////////////////////////////////////////////
            if (getLayout1()) {
              SERIAL_OUT.println("display_setpointPage - layout 1");
              display_layout1_background(ucg, arrotonda(getEncoderValue()) - arrotonda(setpoint));
              display_layout1_setpointPage(ucg, getEncoderValue(), Souliss_SinglePrecisionFloating(memory_map + MaCaco_OUT_s + SLOT_THERMOSTAT + 1), humidity, getSoulissSystemState());
            }
            else if (getLayout2()) {
              SERIAL_OUT.println("display_setpointPage - layout 2");
              display_layout2_Setpoint(ucg, getEncoderValue(), getSoulissSystemState(), bChildLock, B_away_WBS == 1, B_powerfull_WBS == 1);
            }
          }
          encoderValue_prec = getEncoderValue();
          break;
        default :
          break;
      }
    }
    SHIFT_50ms(3) {
      Logic_T19(SLOT_BRIGHT_DISPLAY);
      Logic_T11(SLOT_AWAY);
    }

    SHIFT_110ms(0) {
      if (!getMenuEnabled()) {
        if (timerDisplay_setpoint()) {
          //timeout scaduto
          display_layout1_background_black(ucg);
          setEncoderValue(setpoint);
        } else {
          //timer non scaduto. Memorizzo
          setpoint = getEncoderValue();
          //memorizza il setpoint nel T31
          setSetpoint(setpoint);

          // Trig the next change of the state
          setSoulissDataChanged();
        }
      }
      S_setpoint_WBS = setpoint;
    }


    SHIFT_110ms(4) {
      //SWITCH ENCODER
      //Al click in base al valore attuale di SSTPage, si imposta la pagina successiva

      int b = checkButton(ENCODER_SWITCH);
      if (b == 2) SERIAL_OUT.println("Double Click");
      if (b == 3) {
        SERIAL_OUT.println("Hold");
        bChildLock = !bChildLock;
        SERIAL_OUT.print("Child Lock: "); SERIAL_OUT.println(bChildLock);
        ucg.clearScreen();
        setUIChanged();
        if (getLayout2()) {
          //Reinizializzo la Home per mostrare subito il cambio stato locked/unlocked e non aspettare il refresh
          initScreen();
        }
      }
      if (b == 4) SERIAL_OUT.println("Long Hold");

      if (b == 1) {
        switch (SSTPage.actualPage) {
          case PAGE_HOME:
            if (ACTIVATETOPICSPAGE > 0) {
              SERIAL_OUT.println("from PAGE_HOME to PAGE_TOPICS1");
              SSTPage.actualPage = PAGE_TOPICS1;
              setUIChanged();
            } else {
              SERIAL_OUT.println("from PAGE_HOME to PAGE_MENU");
              SSTPage.actualPage = PAGE_MENU;
              setUIChanged();
              ucg.clearScreen();
              setMenuEnabled();
              //se system and UI changed
              setUIChanged();
              SERIAL_OUT.println("Print Menu");
              printMenu(ucg);
            }
            break;
          case PAGE_TOPICS1:
            if (TOPICSPAGESNUMBER == 1) {
              SERIAL_OUT.println("from PAGE_TOPICS1 to PAGE_MENU");
              SSTPage.actualPage = PAGE_MENU;

              setUIChanged();
              ucg.clearScreen();
              setMenuEnabled();

              SERIAL_OUT.println("Print Menu");
              printMenu(ucg);
            }
            else if (TOPICSPAGESNUMBER == 2) {
              SERIAL_OUT.println("from PAGE_TOPICS1 to PAGE_TOPICS2");
              SSTPage.actualPage = PAGE_TOPICS2;
              setUIChanged();
            }
            break;
          case PAGE_TOPICS2:
            SERIAL_OUT.println("from PAGE_TOPICS2 to PAGE_MENU");
            SSTPage.actualPage = PAGE_MENU;
            setUIChanged();
            ucg.clearScreen();
            setMenuEnabled();
            //se system and UI changed
            setUIChanged();
            SERIAL_OUT.println("Print Menu");
            printMenu(ucg);
            break;
          case PAGE_MENU:

            break;
          case PAGE_CRONO:

            break;
        }
      } else {
        //se progCrono è attivo allora passo alla pagina CRONO
        if (getProgCrono()) {
          SSTPage.actualPage = PAGE_CRONO;
          byte menu;
          SERIAL_OUT.println("Crono");
          ucg.clearScreen();
          drawCrono(ucg);
          SERIAL_OUT.println("drawCrono ok");
          menu = 1;
          //save encoder value before crono programmation
          setpoint = getEncoderValue();
          while (menu == 1 && exitmainmenu() == 0) {
            setDay(ucg);
            drawBoxes(ucg);
            setBoxes(ucg);
            if (exitmainmenu())
            {
              SERIAL_OUT.println("from PAGE_CRONO to PAGE_HOME");
              SSTPage.actualPage = PAGE_HOME;
              initScreen();
              setUIChanged();
              menu = 0;
            }
            yield();
          }
          //restore encoder value
          setEncoderValue(setpoint);
        }
      }
    }

    SHIFT_210ms(0) {
      //FADE
      if (FADE == 0) {
        //Raggiunge il livello di luminosità minima, che può essere variata anche da SoulissApp
        if ( backLEDvalue != backLEDvalueLOW) {
          if ( backLEDvalue > backLEDvalueLOW) {
            backLEDvalue -= BRIGHT_STEP_FADE_OUT;
          } else {
            backLEDvalue += BRIGHT_STEP_FADE_OUT;
          }
          bright(backLEDvalue);
        }
      } else  if (FADE == 1 && backLEDvalue < backLEDvalueHIGH) {
        backLEDvalue +=  BRIGHT_STEP_FADE_IN;
        bright(backLEDvalue);
      }
    }

    SHIFT_210ms(2) {   // We process the logic and relevant input and output
      // Update topics in layout2 home page
      if (getLayout2()) {
        if (ACTIVATETOPICSPAGE == 1 && SSTPage.actualPage == PAGE_HOME) {
          displayTopicsHomePageLayout2(ucg, fTopic_C1_Output, fTopic_C2_Output, fTopic_C3_Output, fTopic_C4_Output, fTopic_C5_Output, fTopic_C6_Output);
        }
      }
    }

    FAST_510ms() {
      //*************************************************************************
      //*************************************************************************
      Logic_Thermostat(SLOT_THERMOSTAT);
      // Start the heater and the fans
      nDigOut(RELE, Souliss_T3n_HeatingOn, SLOT_THERMOSTAT);    // Heater
      S_relestatus_WBS = (mOutput(SLOT_THERMOSTAT) & Souliss_T3n_HeatingOn); //to WBServer


      // We are not handling the cooling mode, if enabled by the user, force it back
      // to disable
      if (mOutput(SLOT_THERMOSTAT) & Souliss_T3n_CoolingOn)
        mOutput(SLOT_THERMOSTAT) &= ~Souliss_T3n_CoolingOn;

      //if menu disabled and nothing changed
      if (SSTPage.actualPage != PAGE_MENU) {
        if (!getSystemChanged()) {
          if (getLocalSystem() != getSoulissSystemState())
            setSystem(getSoulissSystemState());
        }
      }

      //*************************************************************************
      //*************************************************************************

      Logic_T52(SLOT_TEMPERATURE);
      Logic_T53(SLOT_HUMIDITY);
    }

    FAST_710ms() {
      //HOMESCREEN ////////////////////////////////////////////////////////////////
      ///update homescreen only if menu exit
      if (getSystemChanged()) {
        //EXIT MENU - Actions
        //write min bright on T19
        memory_map[MaCaco_OUT_s + SLOT_BRIGHT_DISPLAY + 1] = getDisplayBright();
        SERIAL_OUT.print("Set Display Bright: "); SERIAL_OUT.println(memory_map[MaCaco_OUT_s + SLOT_BRIGHT_DISPLAY + 1]);

        //write system ON/OFF
        if (getLocalSystem()) {
          //ON
          SERIAL_OUT.println("Set system ON ");
          set_ThermostatModeOn(SLOT_THERMOSTAT);        // Set System On

        } else {
          //OFF
          SERIAL_OUT.println("Set system OFF ");
          set_ThermostatOff(SLOT_THERMOSTAT);
        }
        memory_map[MaCaco_IN_s + SLOT_THERMOSTAT] = Souliss_T3n_RstCmd;          // Reset
        // Trig the next change of the state
        setSoulissDataChanged();
        SERIAL_OUT.println("Init Screen");
        setUIChanged();
        initScreen();
        resetSystemChanged();
      }
    }

    FAST_2110ms() {

      //Crono Status in Layout 2
      if (getCrono() && getLayout2() && SSTPage.actualPage == PAGE_HOME ) {

        if (checkCronoStatus(ucg) == 0) //OFF
        {
          ucg.setColor(0, 0, 0);       // black
          ucg.drawDisc(156, 50, 5, UCG_DRAW_ALL);
          ucg.drawDisc(165, 62, 6, UCG_DRAW_ALL);
          ucg.drawDisc(173, 77, 7, UCG_DRAW_ALL);
          ucg.drawDisc(179, 95, 8, UCG_DRAW_ALL);
        }

        if (checkCronoStatus(ucg) == 1) //ECO
        {
          ucg.setColor( 102, 255, 0);       // Verde
          ucg.drawDisc(156, 50, 5, UCG_DRAW_ALL);
          ucg.setColor(0, 0, 0);       // black
          ucg.drawDisc(165, 62, 6, UCG_DRAW_ALL);
          ucg.drawDisc(173, 77, 7, UCG_DRAW_ALL);
          ucg.drawDisc(179, 95, 8, UCG_DRAW_ALL);
        }

        if (checkCronoStatus(ucg) == 2) //NORMAL
        {
          ucg.setColor(255, 255, 153);       // Giallo
          ucg.drawDisc(156, 50, 5, UCG_DRAW_ALL);
          ucg.drawDisc(165, 62, 6, UCG_DRAW_ALL);
          ucg.setColor(0, 0, 0);       // black
          ucg.drawDisc(173, 77, 7, UCG_DRAW_ALL);
          ucg.drawDisc(179, 95, 8, UCG_DRAW_ALL);
        }
        if (checkCronoStatus(ucg) == 3) //COMFORT
        {
          ucg.setColor(255, 204, 0);       // Arancio
          ucg.drawDisc(156, 50, 5, UCG_DRAW_ALL);
          ucg.drawDisc(165, 62, 6, UCG_DRAW_ALL);
          ucg.drawDisc(173, 77, 7, UCG_DRAW_ALL);
          ucg.setColor(0, 0, 0);       // black
          ucg.drawDisc(179, 95, 8, UCG_DRAW_ALL);
        }
        if (checkCronoStatus(ucg) == 4) //COMFORT+
        {
          ucg.setColor(255, 0, 0);       // Rosso
          ucg.drawDisc(156, 50, 5, UCG_DRAW_ALL);
          ucg.drawDisc(165, 62, 6, UCG_DRAW_ALL);
          ucg.drawDisc(173, 77, 7, UCG_DRAW_ALL);
          ucg.drawDisc(179, 95, 8, UCG_DRAW_ALL);
        }
      }
    }


    SHIFT_210ms(3) {
      if (timerDisplay_setpoint()) {
        //if timeout read value of T19
        backLEDvalueLOW =  memory_map[MaCaco_OUT_s + SLOT_BRIGHT_DISPLAY + 1];
        FADE = 0;
        //HOMESCREEN ////////////////////////////////////////////////////////////////
        switch (SSTPage.actualPage) {
          case PAGE_HOME:
            if (getLayout1()) {
              display_layout1_HomeScreen(ucg, temperature, humidity, setpoint, getSoulissSystemState(), bChildLock);
            } else if (getLayout2()) {
              display_layout2_Setpoint(ucg, getEncoderValue(), getSoulissSystemState(), bChildLock, B_away_WBS == 1 , B_powerfull_WBS == 1);
            }
            break;
          case PAGE_TOPICS1:
            //************************************************
            //TOPICS PAGE n.1
            //************************************************
            if (ACTIVATETOPICSPAGE == 1) {
              displayTopics(ucg, fTopic_C1_Output, fTopic_C2_Output, fTopic_C3_Output);
            }
            break;

          case PAGE_TOPICS2:
            //************************************************
            //TOPICS PAGE n.2
            //************************************************
            if (ACTIVATETOPICSPAGE == 1) {
              displayTopicsPage2(ucg, fTopic_C4_Output, fTopic_C5_Output, fTopic_C6_Output);
            }
            break;
        }
      } else {
        //segnala che la pagina attuale al termine del timeut deve essere aggiornata
        setUIChanged();
      }
    }

    SHIFT_910ms(1) {
      subscribeTopics();
      if (getDoSystemReset()) SST_Reset();
    }

    FAST_7110ms() {
      //PUBLISH MESSAGE WHEN HEATING ON OR OFF
      publishHeating_ON_OFF();
    }

#if(DYNAMIC_CONNECTION)
    DYNAMIC_CONNECTION_fast();
#else
    STATIC_CONNECTION_fast();
#endif
  }

  EXECUTESLOW() {
    UPDATESLOW();

    SLOW_50s() {
      getTemp();

      switch (SSTPage.actualPage) {
        case PAGE_HOME:
          on_change_setPoint(arrotonda(setpoint));//Provo a salvare il nuovo setpoint.
          if (getLayout2()) {
            display_layout2_print_circle_white(ucg);
            display_layout2_print_circle_black(ucg);
            display_layout2_HomeScreen(ucg, temperature, humidity, setpoint);
            display_layout2_print_datetime(ucg);
            ucg.setColor(0, 0, 0);       // black
            ucg.drawDisc(156, 50, 5, UCG_DRAW_ALL);
            ucg.drawDisc(165, 62, 6, UCG_DRAW_ALL);
            ucg.drawDisc(173, 77, 7, UCG_DRAW_ALL);
            ucg.drawDisc(179, 95, 8, UCG_DRAW_ALL);
            yield();
            display_layout2_print_circle_green(ucg);

            if (ACTIVATETOPICSPAGE == 1) {
              alwaysdisplayTopicsHomePageLayout2(ucg, fTopic_C1_Output, fTopic_C2_Output, fTopic_C3_Output, fTopic_C4_Output, fTopic_C5_Output, fTopic_C6_Output);
            }

          }
      }
      if ((B_away_WBS == 0 && B_powerfull_WBS == 0)) {
        if (getCrono()) {
          B_is_away_WBS = 0;
          B_is_powerfull_WBS = 0;
          mOutput(SLOT_AWAY) = Souliss_T1n_OffCmd;
          Serial.println("CRONO: aggiornamento");
          setSetpoint(checkNTPcrono(ucg));
          setEncoderValue(checkNTPcrono(ucg));
          Serial.print("CRONO: setpoint: "); Serial.println(setpoint);
        }
      } else {
        //getAWAYtemperature
        //getPOWERFULLtemperature
        if (B_is_away_WBS == 1 && memory_map[MaCaco_OUT_s + SLOT_AWAY] == 0) {
          B_away_WBS = 0;
          B_is_away_WBS = 0;
          mOutput(SLOT_AWAY) = Souliss_T1n_OffCmd;
          Serial.println("AWAY function OFF");
        }
        if (B_away_WBS == 1 || memory_map[MaCaco_OUT_s + SLOT_AWAY] == 1) {
          B_is_away_WBS = 1;
          B_powerfull_WBS = 0;
          B_is_powerfull_WBS = 0;
          mOutput(SLOT_AWAY) = Souliss_T1n_OnCmd;
          setSetpoint(getAWAYtemperature());
          setEncoderValue(getAWAYtemperature());
          Serial.print("AWAY function ON, Setpoint to: "); Serial.println(setpoint);
        }
        if (B_powerfull_WBS == 1) {
          B_away_WBS = 0;
          B_is_away_WBS = 0;
          B_is_powerfull_WBS = 1;
          setSetpoint(getPOWERFULLtemperature());
          setEncoderValue(getPOWERFULLtemperature());
          Serial.print("Powerfull function ON, Setpoint to: "); Serial.println(setpoint);
        }
      }

    }//end SLOW_50s

    SLOW_70s() {
      switch (SSTPage.actualPage) {
        case PAGE_HOME:
          if (getLayout1()) {
            //
          } else if (getLayout2()) {
            calcoloAndamento(ucg, temperature);
            display_layout2_print_datetime(ucg);
            display_layout2_print_circle_green(ucg);
            if (ACTIVATETOPICSPAGE == 1) {
              alwaysdisplayTopicsHomePageLayout2(ucg, fTopic_C1_Output, fTopic_C2_Output, fTopic_C3_Output, fTopic_C4_Output, fTopic_C5_Output, fTopic_C6_Output);
            }
          }
      }


    }

    SLOW_x10s(59) {
      //DATALOGGER
      save_datalogger(setpoint, temperature, humidity, (mOutput(SLOT_THERMOSTAT) & Souliss_T3n_HeatingOn));
    }

    SLOW_15m() {
      //NTP
      yield();
      initNTP();
    }

#if(DYNAMIC_CONNECTION==1)
    DYNAMIC_CONNECTION_slow();
#endif

  }

  // Look for a new sketch to update over the air
  ArduinoOTA.handle();
  yield();
}


void SST_Reset() {
  spiffs_Reset();
  ESP.reset();
}

void subscribeTopics() {
  if (sbscrbdata(TOPIC1, mypayload, &mypayload_len)) {
    float32((uint16_t*) mypayload,  &fTopic_C1_Output);
    SERIAL_OUT.print("TOPIC1: "); SERIAL_OUT.println(fTopic_C1_Output);
  } else if (sbscrbdata(TOPIC2, mypayload, &mypayload_len)) {
    float32((uint16_t*) mypayload,  &fTopic_C2_Output);
    SERIAL_OUT.print("TOPIC2: "); SERIAL_OUT.println(fTopic_C2_Output);
  } else if (sbscrbdata(TOPIC3, mypayload, &mypayload_len)) {
    float32((uint16_t*) mypayload,  &fTopic_C3_Output);
    SERIAL_OUT.print("TOPIC3: "); SERIAL_OUT.println(fTopic_C3_Output);
  } else if (sbscrbdata(TOPIC4, mypayload, &mypayload_len)) {
    float32((uint16_t*) mypayload,  &fTopic_C4_Output);
    SERIAL_OUT.print("TOPIC4: "); SERIAL_OUT.println(fTopic_C4_Output);
  } else if (sbscrbdata(TOPIC5, mypayload, &mypayload_len)) {
    float32((uint16_t*) mypayload,  &fTopic_C5_Output);
    SERIAL_OUT.print("TOPIC5: "); SERIAL_OUT.println(fTopic_C5_Output);
  } else if (sbscrbdata(TOPIC6, mypayload, &mypayload_len)) {
    float32((uint16_t*) mypayload,  &fTopic_C6_Output);
    SERIAL_OUT.print("TOPIC6: "); SERIAL_OUT.println(fTopic_C6_Output);
  }
}


void setSoulissDataChanged() {
  if (data_changed != Souliss_TRIGGED) {

    data_changed = Souliss_TRIGGED;
  }
}

void set_ThermostatModeOn(U8 slot) {
  SERIAL_OUT.println("set_ThermostatModeOn");
  memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_SystemOn;
  memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_HeatingMode;

  // Trig the next change of the state
  setSoulissDataChanged();
}
void set_ThermostatOff(U8 slot) {
  //memory_map[MaCaco_IN_s + slot] = Souliss_T3n_ShutDown;
  memory_map[MaCaco_OUT_s + SLOT_THERMOSTAT] &= ~ (Souliss_T3n_SystemOn | Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3 | Souliss_T3n_CoolingOn | Souliss_T3n_HeatingOn);
  setSoulissDataChanged();
}
void set_DisplayMinBright(U8 slot, U8 val) {
  memory_map[MaCaco_OUT_s + slot + 1] = val;
  // Trig the next change of the state

  setSoulissDataChanged();
}

void encoderFunction() {
  encoder(getMenuEnabled());
}

boolean getSoulissSystemState() {
  return memory_map[MaCaco_OUT_s + SLOT_THERMOSTAT] & Souliss_T3n_SystemOn;
}

boolean T_or_H_isNan = false;
boolean bFlagBegin = false;
void getTemp() {
  // Read temperature value from DHT sensor and convert from single-precision to half-precision
  fVal = dht.readTemperature();
  SERIAL_OUT.print("acquisizione Temperature: "); SERIAL_OUT.println(fVal);
  if (!isnan(fVal)) {
    temperature = fVal + getOffsetDHT(); //memorizza temperatura se non è Not A Number + Offset DHT
    //Import temperature into T31 Thermostat
    ImportAnalog(SLOT_THERMOSTAT + 1, &temperature);
    ImportAnalog(SLOT_TEMPERATURE, &temperature);
    S_temperature_WBS = temperature; //to WBServer
  } else {
    bFlagBegin = true;
  }

  // Read humidity value from DHT sensor and convert from single-precision to half-precision
  fVal = dht.readHumidity();
  SERIAL_OUT.print("acquisizione Humidity: "); SERIAL_OUT.println(fVal);
  if (!isnan(fVal)) {
    humidity = fVal;
    ImportAnalog(SLOT_HUMIDITY, &humidity);
    S_humidity_WBS = humidity; //to WBServer
  } else {
    bFlagBegin = true;
  }

  if ( bFlagBegin) {
    //if DHT fail then try to reinit
    yield();
    dht.begin();
    SERIAL_OUT.println(" dht.begin();");
    yield();
  }
}


void initScreen() {
  ucg.clearScreen();
  SERIAL_OUT.println("clearScreen ok");
  if (getLayout1()) {
    SERIAL_OUT.println("HomeScreen Layout 1");

    display_layout1_HomeScreen(ucg, temperature, humidity, setpoint, getSoulissSystemState(), bChildLock);
    getTemp();
  }
  else if (getLayout2()) {
    SERIAL_OUT.println("HomeScreen Layout 2");
    getTemp();
    display_layout2_HomeScreen(ucg, temperature, humidity, setpoint);
    display_layout2_print_circle_white(ucg);
    display_layout2_print_datetime(ucg);
    if (ACTIVATETOPICSPAGE == 1) {
      alwaysdisplayTopicsHomePageLayout2(ucg, fTopic_C1_Output, fTopic_C2_Output, fTopic_C3_Output, fTopic_C4_Output, fTopic_C5_Output, fTopic_C6_Output);
    }
    display_layout2_print_circle_black(ucg);
    yield();
    display_layout2_print_circle_green(ucg);
  }
}


void setSetpoint(float setpoint) {
  Souliss_HalfPrecisionFloating((memory_map + MaCaco_OUT_s + SLOT_THERMOSTAT + 3), &setpoint);
}


void bright(int lum) {
  int val = ((float)lum / 100) * 1023;
  if (val > 1023) val = 1023;
  if (val < 0) val = 0;
  analogWrite(BACKLED, val);
}

void publishHeating_ON_OFF() {
  //code from Souliss_nDigOut(...
  //nDigOut(RELE, Souliss_T3n_HeatingOn, SLOT_THERMOSTAT);    // Heater

  if (memory_map[MaCaco_OUT_s + SLOT_THERMOSTAT] & Souliss_T3n_HeatingOn)
    pblshdata(SST_HEAT_ONOFF, &HEAT_ON, 1);
  else
    pblshdata(SST_HEAT_ONOFF, &HEAT_OFF, 1);
}

//WBServer
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void onWsEvent(AsyncWebSocket * web_server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("ws[%s][%u] connect\n", web_server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("ws[%s][%u] disconnect: %u\n", web_server->url(), client->id());
  } else if (type == WS_EVT_ERROR) {
    Serial.printf("ws[%s][%u] error(%u): %s\n", web_server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if (type == WS_EVT_PONG) {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", web_server->url(), client->id(), len, (len) ? (char*)data : "");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len) {
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", web_server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0) {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", web_server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", web_server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", web_server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len) {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", web_server->url(), client->id(), info->num, info->len);
        if (info->final) {
          Serial.printf("ws[%s][%u] %s-message end\n", web_server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}
void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  Serial.printf("Void HandleBody:");
  if (!index) {
    Serial.printf("BodyStart: %u B\n", total);
  }
  for (size_t i = 0; i < len; i++) {
    Serial.write(data[i]);
  }
  if (index + len == total) {
    Serial.printf("BodyEnd: %u B\n", total);
  }
  yield();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
