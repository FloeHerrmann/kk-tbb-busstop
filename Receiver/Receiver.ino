#include "LLAPSerial.h"	// include the library

#define DEVICEID "RE"
//#define DEVICEID "RE"

void setup() {
	Serial.begin(115200);
        Serial.println( "STARTED" );

        pinMode( 13 , OUTPUT );
        pinMode( 2 , OUTPUT );
        pinMode( 3 , OUTPUT );
        pinMode( 4 , OUTPUT );
        pinMode( 8 , OUTPUT );
        
        digitalWrite( 2 , HIGH );
        digitalWrite( 3 , HIGH );
        digitalWrite( 4 , HIGH );
        
        digitalWrite( 8 , HIGH );
        
        LLAP.init( );
      
        digitalWrite( 13 , HIGH );
        delay( 266 );
        digitalWrite( 13 , LOW );
        delay( 100 );
        
        digitalWrite( 13 , HIGH );
        delay( 266 );
        digitalWrite( 13 , LOW );
        delay( 100 );
        
        digitalWrite( 13 , HIGH );
        delay( 266 );
        digitalWrite( 13 , LOW );
}

void loop() {
  
	if (LLAP.bMsgReceived) {
	    Serial.print("Message is:");
            Serial.println(LLAP.sMessage);
            if( LLAP.sMessage.equals( "LEDRE----" ) ) {
              digitalWrite( 2 , LOW );
              digitalWrite( 3 , HIGH );
              digitalWrite( 4 , HIGH );
            } else if( LLAP.sMessage.equals( "LEDGR----" ) ) {
              digitalWrite( 2 , HIGH );
              digitalWrite( 3 , LOW );
              digitalWrite( 4 , HIGH );
            } else if( LLAP.sMessage.equals( "LEDBL----" ) ) {
              digitalWrite( 2 , HIGH );
              digitalWrite( 3 , HIGH );
              digitalWrite( 4 , LOW );
            } else if( LLAP.sMessage.equals( "LEDREGR--" ) ) {
              digitalWrite( 2 , LOW );
              digitalWrite( 3 , LOW );
              digitalWrite( 4 , HIGH );
            } else if( LLAP.sMessage.equals( "LEDREBL--" ) ) {
              digitalWrite( 2 , LOW );
              digitalWrite( 3 , HIGH );
              digitalWrite( 4 , LOW );
            } else if( LLAP.sMessage.equals( "LEDGRBL--" ) ) {
              digitalWrite( 2 , HIGH );
              digitalWrite( 3 , LOW );
              digitalWrite( 4 , LOW );
            } else if( LLAP.sMessage.equals( "LEDREGRBL" ) ) {
              digitalWrite( 2 , LOW );
              digitalWrite( 3 , LOW );
              digitalWrite( 4 , LOW );
            } else if( LLAP.sMessage.equals( "LEDOFF---" ) ) {
              digitalWrite( 2 , HIGH );
              digitalWrite( 3 , HIGH );
              digitalWrite( 4 , HIGH );
            }
            LLAP.bMsgReceived = false;
	}
	/*
	static unsigned long lastTime = millis();
	if (millis() - lastTime >= 5000) {
		lastTime = millis();
		LLAP.sendMessage(F("HELLO"));
	}
        */
}
