#include "LLAPSerial.h"	// include the library

//#define DEBUG
#define RADIO

// Pin Funkmodul
#define RADIO 8

// Pin On-Board LED
#define LED_ON_BOARD 13

// Pins LED
#define LED_RED 2
#define LED_GREEN 3
#define LED_BLUE 4

// Zustaende fuer LEDs
#define ON true
#define OFF false

void setup() {

	// Serielle schnittstelle initialisieren
	// Kommunikation mit dem Funkmodul
	Serial.begin(115200);

	// Funktion der Pins definieren
	pinMode( LED_ON_BOARD , OUTPUT );
	pinMode( LED_RED , OUTPUT );
	pinMode( LED_GREEN , OUTPUT );
	pinMode( LED_BLUE , OUTPUT );
	pinMode( RADIO , OUTPUT );
				
	// LED ausschalten
	LED( OFF , OFF , OFF );
				
	#ifdef RADIO
		// Funkmodul einschalten
		digitalWrite( RADIO , HIGH );
	#endif
				
	// Funkmodul initialisieren
	LLAP.init( );
			
	// Anzeigen das Programm gestartet
	startUpSignal();

}

void loop() {
	
	if (LLAP.bMsgReceived) {
		if( LLAP.sMessage.equals( "TBBSTOP--" ) ) {
			LED( OFF , ON , OFF );
			LLAP.sendMessage( F("TBBSTOPOK") );
		} else if( LLAP.sMessage.equals( "TBBCLEAR-" ) ) {
			LED( OFF , OFF , OFF );
			LLAP.sendMessage( F("TBBOFFOK") );
		}
		LLAP.bMsgReceived = false;
	}

}

// Per On-Board LED anzeigen das Programm laeuft
// LED 3x An + Aus
void startUpSignal() {
	for( byte i = 0 ;  i < 3 ; i++ ) {
		digitalWrite( LED_ON_BOARD , HIGH );
		delay( 266 );
		digitalWrite( LED_ON_BOARD , LOW );
		delay( 100 );
	}
}

// LED ansteuern
void LED( bool red , bool green , bool blue ) {
	if( red == true ) digitalWrite( LED_RED , LOW );
	else digitalWrite( LED_RED , HIGH );
		
	if( green == true ) digitalWrite( LED_GREEN , LOW );
	else digitalWrite( LED_GREEN , HIGH );
		
	if( blue == true ) digitalWrite( LED_BLUE , LOW );
	else digitalWrite( LED_BLUE , HIGH );

}

// Fehler per LED anzeigen
void LEDError() {
	for( byte i = 0 ;  i < 3 ; i++ ) {
		LED( ON , OFF , OFF );
		delay( 700 );
		LED( OFF , OFF , OFF );
		delay( 700 );
	}
}
