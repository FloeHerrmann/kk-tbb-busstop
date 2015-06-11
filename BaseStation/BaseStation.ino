#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Suli.h>

#define USE_GSM
#define SEND_MSGS

// GSM Modem
#define GSM_TX 7
#define GSM_RX 6
#define GSM_PWR	9
#define GSM_MESSAGE_LENGTH 160

// Pins Status LED 1 = Rot, 2 = Grün, 3 = Blau
#define LED_TBB_RED 2
#define LED_TBB_GREEN 3
#define LED_TBB_BLUE 4

#define LED_LAU_RED 5
#define LED_LAU_GREEN 10
#define LED_LAU_BLUE 11

// Kommandos
#define SND_STOP_COMMAND_TBB "STOP_TBB"
#define RCV_OK_COMMAND_TBB "TBB_STOP"
#define RCV_RESET_COMMAND_TBB "TBB_RESET"

#define SND_STOP_COMMAND_LAU "STOP_LAU"
#define RCV_OK_COMMAND_LAU "LAU_STOP"
#define RCV_RESET_COMMAND_LAU "LAU_RESET"

// Nummer der Basisstation
#define RECEIVER_NUMBER_TBB "+4915755662566"

// Für Lauda keine Nummer verschicken
#define RECEIVER_NUMBER_LAU ""

// Pin für den Reset-Button
#define REQUEST_BUTTON_TBB 12
#define REQUEST_BUTTON_LAU 13

#define RESPONSE_TIMEOUT 60000
#define ERROR_TIMER 20000

char phone[16];
char datetime[24];
char message[ GSM_MESSAGE_LENGTH ];

bool gotAnswerTBB = false;
bool gotAnswerLAU = false;

bool waitForAnswerTBB = false;
bool waitForAnswerLAU = false;

bool showErrorTBB = false;
bool showErrorLAU = false;

unsigned long timerForTBB = 0;
unsigned long timerForLAU = 0;

GPRS modem( GSM_TX , GSM_RX , 9600 );

void setup() {

	// Funk Modem deaktivieren
	pinMode( 8 , OUTPUT );
	digitalWrite( 8 , LOW );

	// Serielle Schnittstelle initialisieren
	#ifdef SERIAL_OUTPUT
		Serial.begin( 57600 );
	#endif

	// GSM Modem Power Button	
	pinMode( GSM_PWR , OUTPUT );
	// LED Pins
	pinMode( LED_TBB_RED , OUTPUT );
	pinMode( LED_TBB_GREEN , OUTPUT );
	pinMode( LED_TBB_BLUE , OUTPUT );
	// LED Pins
	pinMode( LED_LAU_RED , OUTPUT );
	pinMode( LED_LAU_GREEN , OUTPUT );
	pinMode( LED_LAU_BLUE , OUTPUT );
	// Request Buttons
	pinMode( REQUEST_BUTTON_TBB , INPUT_PULLUP );
	pinMode( REQUEST_BUTTON_LAU , INPUT_PULLUP );

	// Alle LEDs ausschalten
	digitalWrite( LED_TBB_RED , HIGH );
	digitalWrite( LED_TBB_GREEN , HIGH );
	digitalWrite( LED_TBB_BLUE , HIGH );
	digitalWrite( LED_LAU_RED , HIGH );
	digitalWrite( LED_LAU_GREEN , HIGH );
	digitalWrite( LED_LAU_BLUE , HIGH );

	StartUpSignal();

	#ifdef USE_GSM
		ModemPowerUp();
		
		int modemState = 1;
		while( modemState != 0 ) {
			// Status LED Blau
			LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , false , true );
			LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , false , true );
			delay( 500 );
			// Status LED Aus
			LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , false , false );
			LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , false , false );
			delay( 500 );

			modemState = modem.init();

			// Keine SIM Karte, Falsche PIN, etc.
			if( modemState == 3 ) {
				// Status LED Blau
				LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , true , false , false );
				LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , true , false , false );
				while( true ) {}
			}
		}
	#endif

	ReadySignal();

}

void loop() {

	#ifdef USE_GSM
		int messageIndex = modem.isSMSunread();
	
		if( messageIndex > 0 ) {

			modem.readSMS( messageIndex , message , GSM_MESSAGE_LENGTH , phone , datetime );
			String messageS = message;

			if( messageS.equals( RCV_OK_COMMAND_TBB ) ) {
				if( waitForAnswerTBB == true ) {
					LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , true , false );
					waitForAnswerTBB = false;
					showErrorTBB = false;
					gotAnswerTBB = true;
				}
			}
			if( messageS.equals( RCV_OK_COMMAND_LAU ) ) {
				if( waitForAnswerLAU == true ) {
					LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , true , false );
					waitForAnswerLAU = false;
					showErrorLAU = false;
					gotAnswerLAU = true;
				}
			}

			if( messageS.equals( RCV_RESET_COMMAND_TBB ) ) {
				if( gotAnswerTBB == true ) {
					LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , false , false );
					gotAnswerTBB = false;
				}
			}
			if( messageS.equals( RCV_RESET_COMMAND_LAU ) ) {
				if( gotAnswerLAU == true ) {
					LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , false , false );
					gotAnswerLAU = false;
				}
			}

			modem.deleteSMS( messageIndex );
		}
	#endif

	// Zeitraum zwischen Anfrage und Antwort erfassen
	// Wenn die Antwort zu lange braucht einen Fehler anzeigen
	if( waitForAnswerTBB == true && gotAnswerTBB == false ) {
		if( (millis() - timerForTBB) >= RESPONSE_TIMEOUT ) {
			showErrorTBB = true;
			timerForTBB = millis();
		}
	}
	if( waitForAnswerLAU == true && gotAnswerLAU == false ) {
		if( (millis() - timerForLAU) >= RESPONSE_TIMEOUT ) {
			showErrorLAU = true;
			timerForLAU = millis();
		}
	}

	// Fehler anzeigen wenn Antwort zu lange gebraucht hat
	if( showErrorTBB == true ) {
		waitForAnswerTBB = false;
		LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , true , false , false );
		if( (millis() - timerForTBB) >= ERROR_TIMER ) {
			LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , false , false );
			showErrorTBB = false;
		}
	}
	if( showErrorLAU == true ) {
		waitForAnswerLAU = false;
		LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , true , false , false );
		if( (millis() - timerForLAU) >= ERROR_TIMER ) {
			LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , false , false );
			showErrorLAU = false;
		}
	}

	// Prüfen ob ein Taster gedrückt wurde
	if( digitalRead( REQUEST_BUTTON_TBB ) == 0 && waitForAnswerTBB == false ) {
		if( gotAnswerTBB == false ) {
			LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , true , false , true );
			SendShortMessage( 0 );
			waitForAnswerTBB = true;
			showErrorTBB = false;
			gotAnswerTBB = false;
			timerForTBB = millis();
		}
	}
	if( digitalRead( REQUEST_BUTTON_LAU ) == 0 && waitForAnswerLAU == false ) {
		if( gotAnswerLAU == false ) {
			LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , true , false , true );
			SendShortMessage( 1 );
			waitForAnswerLAU = true;
			showErrorLAU = false;
			gotAnswerLAU = false;
			timerForLAU = millis();
		}
	}

}

// GSM Modem einschalten
void ModemPowerUp(){
	digitalWrite( GSM_PWR , HIGH );
	delay( 2000 );
	digitalWrite( GSM_PWR , LOW );
	delay( 2000 );
}

// Nachricht verschicken
void SendShortMessage( int msgNumber ) {
	#ifdef SEND_MSGS
		if( msgNumber == 0 ) {
			modem.sendSMS( RECEIVER_NUMBER_TBB , SND_STOP_COMMAND_TBB );
		} else if( msgNumber == 1 ) {
			//modem.sendSMS( RECEIVER_NUMBER_LAU , SND_STOP_COMMAND_LAU );
		}
 	#endif
}

// Zeigt den Start des Systems an
void StartUpSignal() {
	for( byte i = 0 ;  i < 3 ; i++ ) {
		// Status LED Blau & Grün
		LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , true , true );
		LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , true , true );
		delay( 266 );
		// Status LED Aus
		LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , false , false );
		LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , false , false );
		delay( 100 );
	}
}

// Zeigt die Bereitschaft des Systems an
void ReadySignal() {
	for( byte i = 0 ;  i < 3 ; i++ ) {
		// Stauts LED Grün
		LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , true , false );
		LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , true , false );
		delay( 266 );
		// Status LED Aus
		LED( LED_TBB_RED , LED_TBB_GREEN , LED_TBB_BLUE , false , false , false );
		LED( LED_LAU_RED , LED_LAU_GREEN , LED_LAU_BLUE , false , false , false );
		delay( 100 );
	}
}

// LED ansteuern
void LED( int pinR , int pinG , int pinB , bool red , bool green , bool blue ) {
	if( pinR != 0 ) {
		if( red == true ) digitalWrite( pinR , LOW );
		else if( red == false ) digitalWrite( pinR , HIGH );
	}
	if( pinG != 0 ) {
		if( green == true ) digitalWrite( pinG , LOW );
		else if( green == false )  digitalWrite( pinG , HIGH );
	}
	if( pinB != 0 ) {
		if( blue == true ) digitalWrite( pinB , LOW );
		else if( blue == false )  digitalWrite( pinB , HIGH );
	}
}
