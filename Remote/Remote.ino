#include "LLAPSerial.h"	// include the library

#define BUTTON 7
#define RADIO 8
#define LED_INT 13
#define LED_RED 2
#define LED_GREEN 3
#define LED_BLUE 4

#define ANSWER_TIMEOUT 10000

bool isOn = false;
bool send = false;
bool waitForAnswer = false;
unsigned long answerTimer = 0;

void setup() {
	
	Serial.begin( 115200 );
	Serial.println( "RUNNING" );
			
	pinMode( LED_INT , OUTPUT );		
	pinMode( LED_RED , OUTPUT );        
	pinMode( LED_GREEN , OUTPUT );
	pinMode( LED_BLUE , OUTPUT );
	pinMode( BUTTON , INPUT_PULLUP );
	pinMode( RADIO , OUTPUT );

	LED( false , false , false );
	digitalWrite( RADIO , HIGH );
	LLAP.init( );

	startUpSignal();

}

void loop() {
	
	if( waitForAnswer == true && send == false ) {
		if (LLAP.bMsgReceived) {
			if( LLAP.sMessage.equals( "TBBONOK--" ) ) {
				LED( false , true , false );
				waitForAnswer = false;
			} else if( LLAP.sMessage.equals( "TBBOFFOK-" ) ) {
				LED( false , true , false );
				delay( 1000 );
				waitForAnswer = false;
				LED( false , false , false );
			}
			LLAP.bMsgReceived = false;
		} else {
			if( (millis() - answerTimer) > ANSWER_TIMEOUT ) {
				LEDError();
				waitForAnswer = false;
			}
		}
	} else {
		if( digitalRead( BUTTON ) == 0 ) {
			isOn = !isOn;
			send = true;
			delay( 200 );
		}
		 
		if( isOn == true && send == true ) {
			LLAP.sendMessage(F("BUSTBB"));
			LED( false , false , true );
			send = false;
			waitForAnswer = true;
			answerTimer = millis();
			delay( 500 );
		} else if( isOn == false && send == true ) {
			LLAP.sendMessage(F("------"));
			LED( false , false , false );
			send = false;
			waitForAnswer = true;
			answerTimer = millis();
			delay( 500 );
		}
	}
}

void startUpSignal() {
	digitalWrite( LED_INT , HIGH );
	delay( 266 );
	digitalWrite( LED_INT , LOW );
	delay( 100 );
			
	digitalWrite( LED_INT , HIGH );
	delay( 266 );
	digitalWrite( LED_INT , LOW );
	delay( 100 );
			
	digitalWrite( LED_INT , HIGH );
	delay( 266 );
	digitalWrite( LED_INT , LOW );
}

void LED( bool red , bool green , bool blue ) {
	if( red == true ) digitalWrite( LED_RED , LOW );
	else digitalWrite( LED_RED , HIGH );
	
	if( green == true ) digitalWrite( LED_GREEN , LOW );
	else digitalWrite( LED_GREEN , HIGH );
	
	if( blue == true ) digitalWrite( LED_BLUE , LOW );
	else digitalWrite( LED_BLUE , HIGH );
}

void LEDError() {
	LED( true , false , false );
	delay( 1000 );
	LED( false , false , false );
	delay( 1000 );

	LED( true , false , false );
	delay( 1000 );
	LED( false , false , false );
	delay( 1000 );

	LED( true , false , false );
	delay( 1000 );
	LED( false , false , false );

}