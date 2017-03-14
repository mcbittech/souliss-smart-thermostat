#include <Arduino.h>
#include "tools/types.h"

//PUBLISH&SUBSCRIBE TOPICS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int TOPICS[6][2] = {
   0xF001,0x05, //TOPIC1
   0xF001,0x06, //TOPIC2
   0xF001,0x07, //TOPIC3
   0xF001,0x08, //TOPIC4
   0xF001,0x09, //TOPIC5
   0xF001,0x0A  //TOPIC6
};

#define SST_HEAT_ONOFF 0xF001,0x0B
U8 HEAT_ON = 0X01;
U8 HEAT_OFF = 0X00;

//*****************************************************
//  ES:
//#define TEMPERATURE_TOPIC_NODE_DHT_RELE 0xF001,0x15
//#define HUMIDITY_TOPIC_NODE_DHT_RELE 0xF001,0x16
