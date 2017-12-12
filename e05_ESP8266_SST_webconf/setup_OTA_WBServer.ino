#include "constants.h"
#include "preferences.h"
#include "ntp.h"


void setup_OTA_WBServer(){

  // Init the OTA + WebServer
  // Set Hostname.
  String hostNAME(HOSTNAME);
  hostNAME += String(ESP.getChipId(), HEX);
  SERIAL_OUT.print("set OTA+WiFi hostname: "); SERIAL_OUT.println(hostNAME);
  WiFi.hostname(hostNAME);
  ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
  ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char p[32];
    sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
    events.send(p, "ota");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
    else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
    else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
    else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
    else if(error == OTA_END_ERROR) events.send("End Failed", "ota");
  });
  ArduinoOTA.setHostname((const char *)hostNAME.c_str());
  ArduinoOTA.begin();

  MDNS.addService("http","tcp",80);

  ws.onEvent(onWsEvent);
  web_server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *clientSST){
    clientSST->send("hello!",NULL,millis(),1000);
  });
  web_server.addHandler(&events);

  web_server.addHandler(new SPIFFSEditor(http_username,http_password));

  web_server.on("/", [](AsyncWebServerRequest *request)
    {       
        if (!request->authenticate(http_username,http_password))          
            {
            Serial.printf("Autentication required \n");
            return request->requestAuthentication();//request->redirect("/login");
            }
        else{
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.htm", "text/html");
       request->send(response);
        }
    }); 
    
   web_server.on("/index.htm", [](AsyncWebServerRequest *request)
    {       
        if (!request->authenticate(http_username,http_password))          
            {
            Serial.printf("Autentication required \n");
            return request->requestAuthentication();//request->redirect("/login");
            }
        else{
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.htm", "text/html");
       request->send(response);
        }
    });
     web_server.on("/crono.htm", [](AsyncWebServerRequest *request)
    {       
        if (!request->authenticate(http_username,http_password))          
            {
            Serial.printf("Autentication required \n");
            return request->requestAuthentication();//request->redirect("/login");
            }
        else{
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/crono.htm", "text/html");
       request->send(response);
        }
    });
     web_server.on("/crono1.htm", [](AsyncWebServerRequest *request)
    {       
        if (!request->authenticate(http_username,http_password))          
            {
            Serial.printf("Autentication required \n");
            return request->requestAuthentication();//request->redirect("/login");
            }
        else{
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/crono1.htm", "text/html");
       request->send(response);
        }
    });
    web_server.on("/setting.htm", [](AsyncWebServerRequest *request)
    {       
        if (!request->authenticate(http_username,http_password))          
            {
            Serial.printf("Autentication required \n");
            return request->requestAuthentication();//request->redirect("/login");
            }
        else{
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/setting.htm", "text/html");
       request->send(response);
        }
    });
    web_server.on("/graph.htm", [](AsyncWebServerRequest *request)
    {       
        if (!request->authenticate(http_username,http_password))          
            {
            Serial.printf("Autentication required \n");
            return request->requestAuthentication();//request->redirect("/login");
            }
        else{
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/graph.htm", "text/html");
       request->send(response);
        }
    });
  
  web_server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.printf("GET /heap");
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });
    
  web_server.on("/away_ON", HTTP_GET, [](AsyncWebServerRequest *request){
    B_away_WBS=1;
    Serial.printf("\nGET /away_ON");
    request->redirect("/index.htm");
  });
  
  web_server.on("/away_OFF", HTTP_GET, [](AsyncWebServerRequest *request){
    B_away_WBS=0;
    Serial.printf("\nGET /away_OFF");
    request->redirect("/index.htm");
  });

  web_server.on("/powerfull_ON", HTTP_GET, [](AsyncWebServerRequest *request){
    B_powerfull_WBS=1;
    Serial.printf("\nGET /powerfull_ON");
    request->redirect("/index.htm");
  });
  
  web_server.on("/powerfull_OFF", HTTP_GET, [](AsyncWebServerRequest *request){
    B_powerfull_WBS=0;
    Serial.printf("\nGET /powerfull_OFF");
    request->redirect("/index.htm");
  });
  
  web_server.on("/all", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.printf("\nGET /all");
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    json += ", \"S_temperature_WBS\":"+S_temperature_WBS;
    json += ", \"S_setpoint_WBS\":"+S_setpoint_WBS;
    json += ", \"S_humidity_WBS\":"+S_humidity_WBS;
    json += ", \"S_relestatus_WBS\":" +String(S_relestatus_WBS);
    json += ", \"B_is_away_WBS\":" +String(B_is_away_WBS);
    json += ", \"B_is_powerfull_WBS\":" +String(B_is_powerfull_WBS);
    json += "}";
    Serial.printf("Json: \n");
    request->send(200, "application/json", json);
  });

    web_server.serveStatic("/", SPIFFS, "/"); //.setDefaultFile("index.htm");

  web_server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf(" POST http://%s%s\n", "Host",request->host().c_str());
      //Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  web_server.onFileUpload([](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  web_server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      AsyncWebHeader* h = request->getHeader("Referer");
      Serial.printf("MyHeader: %s\n", h->value().c_str());
      String cerca=h->value().c_str();
      if ( cerca.indexOf("crono")==-1)
      {
        Serial.printf("Crono non trovato: %s\n", h->value().c_str());
      }
      else {
        Serial.printf("Crono trovato: %s\n", h->value().c_str());
        cerca=(char*)data;
        int totale=cerca.indexOf("]}");
        totale = totale +2;
        String cerca_stringa=cerca.substring(0,totale)+"\n";
        Serial.printf("Body: %u\n", total);
        Serial.printf("Body cerca: %u\n", totale);
        Serial.println("cerca: "+cerca+"\n");
        Serial.println("cerca_stringa: "+cerca_stringa+"\n");
        if(!index)
          Serial.printf("Request : ",request);
          Serial.printf("BodyStart: %u\n", index);
          Serial.printf("scrivo file: sst_crono_matrix.json");
          Serial.printf("%s", (const char*)data);
          String S_filena_WBS = "/sst_crono_matrix.json";
          fsUploadFile = SPIFFS.open(S_filena_WBS, "w");
          if (!fsUploadFile) 
            Serial.println("file open failed");
          fsUploadFile.println(cerca_stringa); 
          Serial.printf("BodyEnd: %u\n",totale);
          fsUploadFile.close();
          delay (1000);
          request->redirect("/salvato.htm");
          delay (1000);
          ReadCronoMatrixSPIFFS();
        }
        if (cerca.indexOf("setting")==-1)
        { 
        Serial.printf("Setting non trovato: %s\n", h->value().c_str());
        } 
        else{
            Serial.printf("Setting trovato: %s\n", h->value().c_str());
            if(!index)
            Serial.printf("Request : ",request);
            Serial.printf("BodyStart: %u\n", index);
            Serial.printf("scrivo file: sst_setting.json");
            Serial.printf("%s", (const char*)data);
            String S_filena_WBS = "/sst_settings.json";
            fsUploadFile = SPIFFS.open(S_filena_WBS, "w");
            if (!fsUploadFile) 
              Serial.println("file open failed");
            fsUploadFile.printf("%s",(const char*)data);  
            if(index + len == total)
              Serial.printf("BodyEnd: %u\n", total);
              fsUploadFile.close();
              delay (1000);
              request->redirect("/salvato.htm");
              delay (1000);
              ReadAllSettingsFromSPIFFS();
        } 
  });
  



  //Client
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  #ifdef TTD
    Serial.println("SEND BOOTUP");
    HTTPClient clienthttp_SST;
    const char* host="http://www.google-analytics.com/collect";
    String eventData = "v=1&t=event&tid=UA-89261240-1&cid=555&ec=SST"+String(VERSION)+"&ea=BOOTUP&el="+String(ESP.getChipId(),HEX);
    clienthttp_SST.begin(host);
    clienthttp_SST.addHeader("User-agent", "Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0");
    clienthttp_SST.POST(eventData);
    clienthttp_SST.writeToStream(&Serial);
    clienthttp_SST.end();
    Serial.println("BOOTUP CLOSED");
  #endif
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  web_server.begin();
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
}  
