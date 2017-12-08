#include <Arduino.h>
#include <ESP8266WiFi.h>

#define VERSION "v1.2"
#define HOSTNAME "SST-Thermostat-OTA-" ///< Hostename. The setup_OTA_WBServer function adds the Chip ID at the end.

//DHT22
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DHTPIN 12
#define DHTTYPE DHT22

//ENCODER
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ENCODER_PIN_A   3
#define ENCODER_PIN_B   4
#define ENCODER_SWITCH  0

//SETPOINT
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MINSETPOINT 0
#define MAXSETPOINT 99


//DISPLAY
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TFT_DC 2
#define TFT_CS 15

//SLOT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SLOT_THERMOSTAT         0
#define SLOT_TEMPERATURE        5     // This is the memory slot used for the execution of the logic in network_address1
#define SLOT_HUMIDITY           7     // This is the memory slot used for the execution of the logic
#define SLOT_BRIGHT_DISPLAY     9
#define SLOT_AWAY               11


//T31 Commands
#define Souliss_T3n_InSetPoint      0x01
#define Souliss_T3n_OutSetPoint     0x02
#define Souliss_T3n_AsMeasured      0x03
#define Souliss_T3n_Cooling         0x04
#define Souliss_T3n_Heating         0x05
#define Souliss_T3n_FanOff          0x06
#define Souliss_T3n_FanLow          0x07
#define Souliss_T3n_FanMed          0x08
#define Souliss_T3n_FanHigh         0x09
#define Souliss_T3n_FanAuto         0x0A
#define Souliss_T3n_FanManual       0x0B
#define Souliss_T3n_SetTemp         0x0C
#define Souliss_T3n_ShutDown        0x0D

//Souliss Commands
#define Souliss_TRIGGED             0x01


//PIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// **** Define here the right pin for your ESP module ****
#define RELE     5
#define BACKLED  16


//TIMER DISPLAY
#define TIMER_DISPLAY_SETPOINT 1500
#define SERIAL_OUT Serial

//FONT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define FONT_BIG ucg_font_inb63_mn
//#define FONT_BIG ucg_font_inb46_mr
//#define FONT_BIG_MIN_50_PERCENT ucg_font_inb33_mr
#define FONT_BIG ucg_font_inb53_mn
#define FONT_BIG_MIN_50_PERCENT ucg_font_inb24_mr

#define FONT_BIG_SCALE2x2             1     //1 Scale - 0 Don't Scale
#define FONT_SMALL ucg_font_inb16_mr
#define FONT_SMALL_CENTIGRAD ucg_font_helvB10_hr
#define FONT_SMALLEST ucg_font_4x6_mf
#define FONT_SPLASH_SCREEN ucg_font_inb21_mr

#define FONT_SHIFT_POSITION_TO_SX_CENTIGRAD 5
