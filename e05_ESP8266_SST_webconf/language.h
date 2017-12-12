#include <Arduino.h>

//LANGUAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SPLASH_GW_LINE1 "Modo Gateway" 
#define SPLASH_GW_LINE2 "Preparazione..." 

#define SPLASH_PEER_LINE1 "Modo Peer" 
#define SPLASH_PEER_LINE2 "Connessione... " 

#define SPLASH_NEED_CONF_LINE1 "Eseguire la "
#define SPLASH_NEED_CONF_LINE2 "configurazione "
#define SPLASH_NEED_CONF_LINE3 "via WEB "
#define SPLASH_SSID_LINE1 "Connessione "


#define SETPOINT_TEXT_SETPOINT_SCREEN "SETPOINT"
#define SETPOINT_TEXT "Set: "
#define TEMP_TEXT "T: "
#define HUM_TEXT "H: "
#define SYSTEM_OFF_TEXT "SPENTO  " //due spazi in più per coprire il valore dell'umidità, che rimane sotto. Preferisco non dare il clearscreen. 
#define SYSTEM_OFF_TEXT_LAYOUT2 "SPENTO" //senza i due spazi in più che vanno sopra il cerchio di layout2
#define CHILDLOCK_TEXT "LOCKED"
#define AWAY_TEXT_LAYOUT2 " AWAY "
#define POWERFULL_TEXT_LAYOUT2 "P.FULL"



//MENU
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_TEXT_ROOT "Smart Souliss Thermostat"
#define MENU_TEXT_ON "ON"
#define MENU_TEXT_OFF "OFF"
#define MENU_TEXT_BACK "Torna"
#define MENU_TEXT_MENU "Menu"
#define MENU_TEXT_CRONO_PROGRAM "Programmazione Crono"
#define MENU_TEXT_CRONO_ENABLE "Crono"
#define MENU_TEXT_LEARN "Autoapprendimento"
#define MENU_TEXT_CLOCK "Orologio"
#define MENU_TEXT_BRIGHT "Luminosita'"
#define MENU_TEXT_SYSTEM_ENABLED "Dispositivo"
#define MENU_TEXT_LAYOUTS "Layouts"
#define MENU_TEXT_LAYOUT_1 "Lineare"
#define MENU_TEXT_LAYOUT_2 "Circolare"
#define MENU_TEXT_SYSTEM "Sistema"
#define MENU_TEXT_SYSTEM_1 "Imp.Fabbrica"
#define MENU_TEXT_TIMEZONE_SET "Timezone"
#define MENU_TIMEZONE_0 "UTC+0 Eu West,Greenw."
#define MENU_TIMEZONE_1 "UTC+1 Eu Cent,Italia"
#define MENU_TIMEZONE_2 "UTC+2 Eu Est,Romania"
#define MENU_TIMEZONE_3 "UTC+3 Eu Est,Bel,Russia"
#define MENU_TIMEZONE_4 "UTC+4 Turchia,Azerb."
#define MENU_TEXT_DAYLIGHTSAVINGTIME_SET "Ora Legale"
#define MENU_DAYLIGHTSAVINGTIME_ON "ON"
#define MENU_DAYLIGHTSAVINGTIME_OFF "OFF"
