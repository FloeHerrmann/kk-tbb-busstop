#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Suli.h>

//#define SERIAL_OUTPUT
#define USE_GSM
//#define SEND_MSGS

// GSM Modem
#define GSM_TX 7
#define GSM_RX 6
#define GSM_PWR	9
#define GSM_MESSAGE_LENGTH 160

// Pins Status LED 1 = Rot, 2 = Grün, 3 = Blau
#define LED_STATE_GREEN 2
#define LED_STATE_BLUE 3
#define LED_STOP_1 4
#define LED_STOP_2 5

// Kommandos
#define RCV_STOP_COMMAND "STOP_TBB"
#define RCV_RESET_COMMAND "RESET_TBB"
#define ANS_OK_COMMAND "TBB_OK"
#define ANS_ERROR_COMMAND "TBB_ERROR"
#define ANS_RESET_COMMAND "TBB_RESET"


// Nummer der Basisstation
#define BASESTATION_NUMBER "+491624220560"

// Pin für den Reset-Button
#define BUTTON 10

char phone[16];
char datetime[24];
char message[ GSM_MESSAGE_LENGTH ];

bool receivedStopCommand = false;

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
	pinMode( LED_STATE_GREEN , OUTPUT );
	pinMode( LED_STATE_BLUE , OUTPUT );
	pinMode( LED_STOP_1 , OUTPUT );
	pinMode( LED_STOP_2 , OUTPUT );
	// Reset Button
	pinMode( BUTTON , INPUT_PULLUP );

	// Alle LEDs ausschalten
	digitalWrite( LED_STATE_GREEN , HIGH );
	digitalWrite( LED_STATE_BLUE , HIGH );
	digitalWrite( LED_STOP_1 , HIGH );
	digitalWrite( LED_STOP_2 , HIGH );

	StartUpSignal();

	#ifdef USE_GSM
		ModemPowerUp();
		
		while( modem.init() != 0 ) {
			// Status LED Blau
			LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , false , true );
			delay( 500 );
			// Status LED Aus
			LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , false , false );
			delay( 500 );

			#ifdef SERIAL_OUTPUT
				Serial.println( F( "Modem Not Ready!" ) );
			#endif
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

			#ifdef SERIAL_OUTPUT
				Serial.print( F( "Received A Text Message > " ) );
				Serial.print( phone ); Serial.print( F( " : " ) );
				Serial.print( messageS );
			#endif

			if( messageS.equals( RCV_STOP_COMMAND ) ) {
				if( receivedStopCommand == false ) {
					SwitchStopSign( true );
					SendShortMessage( 0 );
					receivedStopCommand = true;
				}
			}

			modem.deleteSMS( messageIndex );
		}
	#endif

	if( ( receivedStopCommand == true ) && ( digitalRead( BUTTON ) == 0 )  ) {
		SwitchStopSign( false );
		SendShortMessage( 2 );
		receivedStopCommand = false;
		delay( 500 );
	}

}

// GSM Modem einschalten
void ModemPowerUp(){
	digitalWrite( GSM_PWR , HIGH );
	delay( 2000 );
	digitalWrite( GSM_PWR , LOW );
	delay( 2000 );
}

// GSM Modem ausschalten
void ModemPowerDown(){
	digitalWrite( GSM_PWR , HIGH );
	delay( 2000 );
	digitalWrite( GSM_PWR , LOW );
	delay( 4000 );
}

// Nachricht verschicken
void SendShortMessage( int msgNumber ) {

	#ifdef SERIAL_OUTPUT
		Serial.print( F( "Send A Text Message > " ) );
		Serial.print( BASESTATION_NUMBER ); Serial.print( F( " : " ) );
		if( msgNumber == 0 ) Serial.print( ANS_OK_COMMAND );
		if( msgNumber == 1 ) Serial.print( ANS_ERROR_COMMAND );
		if( msgNumber == 2 ) Serial.print( ANS_RESET_COMMAND );
	#endif

	#ifdef SEND_MSGS
		if( msgNumber == 0 ) {
			modem.sendSMS( BASESTATION_NUMBER , ANS_OK_COMMAND );
		} else if( msgNumber == 1 ) {
			modem.sendSMS( BASESTATION_NUMBER , ANS_ERROR_COMMAND );
		} else if( msgNumber == 2 ){
			modem.sendSMS( BASESTATION_NUMBER , ANS_RESET_COMMAND );
		}
 	#endif
}

// Zeigt den Start des Systems an
void StartUpSignal() {

	for( byte i = 0 ;  i < 3 ; i++ ) {
		// Status LED Blau & Grün
		LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , true , true );
		delay( 266 );
		// Status LED Aus
		LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , false , false );
		delay( 100 );
	}
}

// Zeigt die Bereitschaft des Systems an
void ReadySignal() {

	for( byte i = 0 ;  i < 3 ; i++ ) {
		// Stauts LED Grün
		LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , true , false );
		delay( 266 );
		// Status LED Aus
		LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , false , false );
		delay( 100 );
	}
}

// Stop Zeichen an-/ausschalten
void SwitchStopSign( bool state ){

	if( state == true ) {
		LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , true , false );
		LED( 0 , LED_STOP_1 , 0 , false , true , false );
		LED( 0 , LED_STOP_2 , 0 , false , true , false );
	} else if( state == false ) {
		LED( 0 , LED_STATE_GREEN , LED_STATE_BLUE , false , false , false );
		LED( 0 , LED_STOP_1 , 0 , false , false , false );
		LED( 0 , LED_STOP_2 , 0 , false , false , false );
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
