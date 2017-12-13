/**************************************************************************
    Souliss - Pubblicazione dati float tramite metodo subscribe

***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "bconf/MCU_ESP8266.h"              // ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS

// Include framework code and libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "**********"
#define WiFi_Password           "**********"


/*** All configuration includes should be above this line ***/
#include "Souliss.h"

#define PRESSURE0             0               // Leave 2 slots for T58
#define TEMP0                 2               // Leave 2 slots for T52
#define HUMIDITY0             4

Adafruit_BME280 bme(2); // hardware SPI

U8 pressureArray[2];
U8 temperatureArray[2];
U8 humidityArray[2];
uint16_t temperature;
uint16_t humidity;
uint16_t pressure;

#define TOPIC1 0xF001,0x05 //Topic Standard SST thermostat
#define TOPIC2 0xF001,0x06
#define TOPIC3 0xF001,0x07
#define TOPIC4 0xF001,0x08
#define TOPIC5 0xF001,0x09
#define TOPIC6 0xF001,0x0A

short ctr = 0;
void setup()
{
  Serial.begin(115200);
  bool status;
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );
  Initialize();


  // Get the IP address from DHCP
  GetIPAddress();
  SetAsGateway(myvNet_dhcp);                  // Set this node as gateway for SoulissApp

  Set_Pressure(PRESSURE0);
  Set_Temperature(TEMP0);
  Set_Humidity(HUMIDITY0);
}

void loop()
{
  // Here we start to play
  EXECUTEFAST() {
    UPDATEFAST();
    // Execute the code every 1 time_base_fast
    FAST_10ms() {
      // Just process communication as fast as the logics
      ProcessCommunication();
    }

    // Process the other Gateway stuffs
    FAST_GatewayComms();

    FAST_910ms()    {
      float pressureValue = bme.readPressure() / 100.0f;
      yield();
      float temperatureValue = bme.readTemperature();
      yield();
      float humidityValue = bme.readHumidity();
      yield();

      Serial.println(pressureValue);
      Serial.println(temperatureValue);
      Serial.println(humidityValue);

      yield();
      float16(&pressure, &pressureValue);
      pressureArray[0] = C16TO8L(pressure);
      pressureArray[1] = C16TO8H(pressure);
      float16(&temperature, &temperatureValue);
      temperatureArray[0] = C16TO8L(temperature);
      temperatureArray[1] = C16TO8H(temperature);
      float16(&humidity, &humidityValue);
      humidityArray[0] = C16TO8L(humidity);
      humidityArray[1] = C16TO8H(humidity);

      yield();
      
      switch(ctr){
        case 0:
          pblshdata(TOPIC3, humidityArray, 2);
          break;
        case 1:    
          pblshdata(TOPIC2, temperatureArray, 2);
          break;
        case 2:
          pblshdata(TOPIC1, pressureArray, 2);
          break;
      }
      ctr = ctr+1>2 ? 0 : ctr+1;

      yield();

      ImportAnalog(PRESSURE0, &pressureValue);
      ImportAnalog(TEMP0, &temperatureValue);
      ImportAnalog(HUMIDITY0, &humidityValue);

    }
  }
  EXECUTESLOW()
  {
    UPDATESLOW();

    SLOW_10s()  {

      Logic_Pressure(PRESSURE0);
      Logic_Temperature(TEMP0);
      Logic_Humidity(HUMIDITY0);
    }
    SLOW_50s() {

    }
  }
}

