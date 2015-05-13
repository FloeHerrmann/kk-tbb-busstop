#include "LLAPSerial.h"

//#define DEBUG
#define RADIO

// Pin Funkmodul
#define RADIO 8

// Pin On-Board LED
#define LED_ON_BOARD 13

// Pins LED und Taster TBB
#define LED_TBB 100
#define LED_TBB_RED 2
#define LED_TBB_GREEN 3
#define LED_TBB_BLUE 4
#define BUTTON_TBB 9

// Pins LED und Taster LAU
#define LED_LAU 101
#define LED_LAU_RED 5
#define LED_LAU_GREEN 6
#define LED_LAU_BLUE 7
#define BUTTON_LAU 10

// Zustaende fuer LEDs
#define ON true
#define OFF false

// Antwort Timeout
#define ANSWER_TIMEOUT 10000

// Hilfsvariablen
bool isOnTBB = false;
bool isOnLAU = false;

bool sendTBB = false;
bool sendLAU = false;

bool waitForAnswerTBB = false;
bool waitForAnswerLAU = false;

unsigned long answerTimerTBB = 0;
unsigned long answerTimerLAU = 0;

void setup() {
	
	// Serielle schnittstelle initialisieren
	// Kommunikation mit dem Funkmodul
	Serial.begin( 115200 );
			
	// Funktion der Pins definieren
	pinMode( LED_ON_BOARD, OUTPUT );
	pinMode( RADIO , OUTPUT );
	pinMode( BUTTON_LAU , INPUT_PULLUP );
	pinMode( BUTTON_TBB , INPUT_PULLUP );
	pinMode( LED_TBB_RED , OUTPUT );
	pinMode( LED_TBB_GREEN , OUTPUT );
	pinMode( LED_TBB_BLUE , OUTPUT );
	pinMode( LED_LAU_RED , OUTPUT );
	pinMode( LED_LAU_GREEN , OUTPUT );
	pinMode( LED_LAU_BLUE , OUTPUT );
	
	// LEDs ausschalten
	LED( LED_TBB , OFF , OFF , OFF );
	LED( LED_LAU , OFF , OFF , OFF );

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
	
	if( digitalRead( BUTTON_TBB ) == 0 && waitForAnswerTBB == false ) {
		isOnTBB = !isOnTBB;
		sendTBB = true;
		delay( 200 );
	}

	if( digitalRead( BUTTON_LAU ) == 0 && waitForAnswerLAU == false ) {
		isOnLAU = !isOnLAU;
		sendLAU = true;
		delay( 200 );
	}

	#ifdef DEBUG
		Serial.print( "IsOn > " );
		Serial.print( isOnTBB ); Serial.print( " / " );
		Serial.print( isOnLAU ); Serial.print( " / " );

		Serial.print( "Send > " );
		Serial.print( sendTBB ); Serial.print( " / " );
		Serial.print( sendLAU ); Serial.print( " / " );
	#endif
	
	if( sendTBB== true ) {
		if( isOnTBB == true ) {
			LLAP.sendMessage( F("TBBSTOP") );
			LED( LED_TBB , OFF , OFF , ON );
		}
		if( isOnTBB == false ) {
			LLAP.sendMessage(F("TBBCLEAR"));
			LED( LED_TBB , OFF , OFF , OFF );
		}
		sendTBB = false;
		waitForAnswerTBB = true;
		answerTimerTBB = millis();
		delay( 500 );
	}

	if( sendLAU == true ) {
		if( isOnLAU == true ) {
			LLAP.sendMessage( F("LAUSTOP") );
			LED( LED_LAU , OFF , OFF , ON );
		} else if( isOnLAU == false ) {
			LLAP.sendMessage(F("LAUCLEAR"));
			LED( LED_LAU , OFF , OFF , OFF );
		}
		sendLAU = false;
		waitForAnswerLAU = true;
		answerTimerLAU = millis();
		delay( 500 );
	}

	#ifdef DEBUG
		Serial.print( "Wait > " );
		Serial.print( waitForAnswerTBB ); Serial.print( " / " );
		Serial.print( waitForAnswerLAU ); Serial.print( " / " );

		Serial.print( "Timer > " );
		Serial.print( answerTimerTBB ); Serial.print( " / " );
		Serial.print( answerTimerLAU ); Serial.print( " / " );

		Serial.print( millis() - answerTimerTBB ); Serial.print( " / " );
		Serial.println( millis() - answerTimerLAU );
		delay( 1000 );
	#endif

	if( waitForAnswerLAU == true || waitForAnswerTBB == true  ) {

		if (LLAP.bMsgReceived) {
			if( LLAP.sMessage.equals( "TBBSTOPOK" ) ) {
				LED( LED_TBB , OFF , ON , OFF );
				waitForAnswerTBB = false;
			} else if( LLAP.sMessage.equals( "TBBOFFOK-" ) ) {
				LED( LED_TBB , OFF , ON , OFF );
				delay( 700 );
				LED( LED_TBB , OFF , OFF , OFF );
				waitForAnswerTBB = false;
			} else if( LLAP.sMessage.equals( "LAUSTOPOK" ) ) {
				LED( LED_LAU , OFF , ON , OFF );
				waitForAnswerLAU = false;
			} else if( LLAP.sMessage.equals( "LAUOFFOK-" ) ) {
				LED( LED_LAU , OFF , ON , OFF );
				delay( 700 );
				LED( LED_LAU , OFF , OFF , OFF );
				waitForAnswerLAU = false;
			}
			LLAP.bMsgReceived = false;
		}
		
		if( waitForAnswerTBB == true ) {
			if( (millis() - answerTimerTBB) > ANSWER_TIMEOUT ) {
				waitForAnswerTBB = false;
				answerTimerTBB = 0;
				isOnTBB = false;
				LEDError( LED_TBB );
			}
		}

		if( waitForAnswerLAU == true ) {
			if( (millis() - answerTimerLAU) > ANSWER_TIMEOUT ) {
				waitForAnswerLAU = false;
				answerTimerLAU = 0;
				isOnLAU = false;
				LEDError( LED_LAU );
			}
		}

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
void LED( int targetLED , bool red , bool green , bool blue ) {
	if( targetLED == LED_TBB ) {
		if( red == true ) digitalWrite( LED_TBB_RED , LOW );
		else digitalWrite( LED_TBB_RED , HIGH );
		
		if( green == true ) digitalWrite( LED_TBB_GREEN , LOW );
		else digitalWrite( LED_TBB_GREEN , HIGH );
		
		if( blue == true ) digitalWrite( LED_TBB_BLUE , LOW );
		else digitalWrite( LED_TBB_BLUE , HIGH );
	} else if( targetLED == LED_LAU ) {
		if( red == true ) digitalWrite( LED_LAU_RED , LOW );
		else digitalWrite( LED_LAU_RED , HIGH );
		
		if( green == true ) digitalWrite( LED_LAU_GREEN , LOW );
		else digitalWrite( LED_LAU_GREEN , HIGH );
		
		if( blue == true ) digitalWrite( LED_LAU_BLUE , LOW );
		else digitalWrite( LED_LAU_BLUE , HIGH );
	}
}

// Fehler per LED anzeigen
void LEDError( int targetLED) {
	for( byte i = 0 ;  i < 3 ; i++ ) {
		LED( targetLED , ON , OFF , OFF );
		delay( 700 );
		LED( targetLED , OFF , OFF , OFF );
		delay( 700 );
	}
}