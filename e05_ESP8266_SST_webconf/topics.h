#include <Arduino.h>
#include "tools/types.h"

//PUBLISH&SUBSCRIBE TOPICS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int TOPICS[6][3] = {
   0xF001,0x05, 0xf55e7a, //TOPIC1
   0xF001,0x06, 0xf55e6b, //TOPIC2
   0xF001,0x07, 0xf55e7c, //TOPIC3
   0xF001,0x08, 0xf55e80, //TOPIC4
   0xF001,0x09, 0xf55e91, //TOPIC5
   0xF001,0x0A, 0xf55e9b  //TOPIC6
};

#define SST_HEAT_ONOFF 0xF001,0x0B
U8 HEAT_ON = 0X01;
U8 HEAT_OFF = 0X00;

//*****************************************************
//  ES:
//#define TEMPERATURE_TOPIC_NODE_DHT_RELE 0xF001,0x15
//#define HUMIDITY_TOPIC_NODE_DHT_RELE 0xF001,0x16
