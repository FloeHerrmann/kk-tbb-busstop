#include "LLAPSerial.h"	// include the library

#define DEVICEID "RE"
//#define DEVICEID "RE"

#define RADIO 8
#define LED_INT 13
#define LED_GREEN 2

void setup() {
	Serial.begin(115200);
        Serial.println( "STARTED" );

        pinMode( LED_INT , OUTPUT );
        pinMode( LED_GREEN , OUTPUT );
        pinMode( RADIO , OUTPUT );
        
        digitalWrite( LED_GREEN , HIGH );
        
        digitalWrite( RADIO , HIGH );
        
        LLAP.init( );
      
        startUpSignal();
}

void loop() {
  
	if (LLAP.bMsgReceived) {
	    //Serial.print("Message is:");
            //Serial.println(LLAP.sMessage);
            if( LLAP.sMessage.equals( "BUSTBB---" ) ) {
              digitalWrite( LED_GREEN , LOW );
              LLAP.sendMessage( F("TBBONOK") );
            } else if( LLAP.sMessage.equals( "---------" ) ) {
              digitalWrite( LED_GREEN , HIGH );
              LLAP.sendMessage( F("TBBOFFOK") );
            }
            LLAP.bMsgReceived = false;
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
