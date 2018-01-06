#include <Arduino.h>

//LANGUAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef LANG_en_US
	//en_US locale
	// @rmackinnon: Text is monospace, should probably keep this same width as original.
	#define SPLASH_GW_LINE1 "Gateway Mode" 
	#define SPLASH_GW_LINE2 "Preparing..." 

	#define SPLASH_PEER_LINE1 "Peer Mode" 
	#define SPLASH_PEER_LINE2 "Connecting... " 

	#define SPLASH_NEED_CONF_LINE1 "Please perform "
	#define SPLASH_NEED_CONF_LINE2 "configuration "
	#define SPLASH_NEED_CONF_LINE3 "via WEB "
	#define SPLASH_SSID_LINE1 "Connected "
	//Per @mcbittech: two spaces to cover humidity value below this layer. Hack to not require clearscreen.
	#define SYSTEM_OFF_TEXT "OFF       " 
	//Per @mcbittech: without two spaces the numbers would range over Layour2
	#define SYSTEM_OFF_TEXT_LAYOUT2 "OFF   "
	#define CHILDLOCK_TEXT "LOCKED"
	#define SETPOINT_TEXT_SETPOINT_SCREEN "SETPOINT"
	#define SETPOINT_TEXT "Set: "
#else
	//Defaults to Italian
	#define SPLASH_GW_LINE1 "Modo Gateway" 
	#define SPLASH_GW_LINE2 "Preparazione..." 

	#define SPLASH_PEER_LINE1 "Modo Peer" 
	#define SPLASH_PEER_LINE2 "Connessione... " 

	#define SPLASH_NEED_CONF_LINE1 "Eseguire la "
	#define SPLASH_NEED_CONF_LINE2 "configurazione "
	#define SPLASH_NEED_CONF_LINE3 "via WEB "
	#define SPLASH_SSID_LINE1 "Connessione "

	#define SYSTEM_OFF_TEXT "SPENTO  " //due spazi in più per coprire il valore dell'umidità, che rimane sotto. Preferisco non dare il clearscreen. 
	#define SYSTEM_OFF_TEXT_LAYOUT2 "SPENTO" //senza i due spazi in più che vanno sopra il cerchio di layout2
	#define CHILDLOCK_TEXT "LOCKED"
	#define SETPOINT_TEXT_SETPOINT_SCREEN "SETPOINT"
	#define SETPOINT_TEXT "Set: "
#endif

#define TEMP_TEXT "T: "
#define HUM_TEXT "H: "

//MENU
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_TEXT_ROOT "Smart Souliss Thermostat"
#ifdef LANG_en_US
	#define MENU_TEXT_ON "ON"
	#define MENU_TEXT_OFF "OFF"
	#define MENU_TEXT_MENU "Menu"
	#define MENU_TEXT_BACK "Back"
	#define MENU_TEXT_CRONO_PROGRAM "Program"
	#define MENU_TEXT_CRONO_ENABLE "Timer"
	#define MENU_TEXT_LEARN "Self-Learning"
	#define MENU_TEXT_CLOCK "Clock"
	#define MENU_TEXT_BRIGHT "Brightness"
	#define MENU_TEXT_SYSTEM_ENABLED "Device"
	#define MENU_TEXT_LAYOUTS "Layouts"
	#define MENU_TEXT_LAYOUT_1 "Linear"
	#define MENU_TEXT_LAYOUT_2 "Circular"
	#define MENU_TEXT_SYSTEM "System"
	#define MENU_TEXT_SYSTEM_1 "Factory Default"
	#define MENU_TEXT_TIMEZONE_SET "Timezone"
	#define MENU_TIMEZONE_0 "UTC+0 Eu West,Greenw."
	#define MENU_TIMEZONE_1 "UTC+1 Eu Cent,Italia"
	#define MENU_TIMEZONE_2 "UTC+2 Eu Est,Romania"
	#define MENU_TIMEZONE_3 "UTC+3 Eu Est,Bel,Russia"
	#define MENU_TIMEZONE_4 "UTC+4 Turchia,Azerb."
	#define MENU_TEXT_DAYLIGHTSAVINGTIME_SET "Summer"
	#define MENU_DAYLIGHTSAVINGTIME_ON "ON"
	#define MENU_DAYLIGHTSAVINGTIME_OFF "OFF"
#else
	#define MENU_TEXT_ON "ON"
	#define MENU_TEXT_OFF "OFF"
	#define MENU_TEXT_MENU "Menu"
	#define MENU_TEXT_BACK "Torna"
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
#endif