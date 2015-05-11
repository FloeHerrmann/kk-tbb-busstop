#include "LLAPSerial.h"	// include the library

#define DEVICEID "MA"
//#define DEVICEID "RE"

byte step = -1;
bool send = false;

void setup() {
	Serial.begin( 115200 );
        Serial.println( "RUNNING" );
        
        pinMode( 13 , OUTPUT );
        digitalWrite( 13 , LOW );
        
        pinMode( 2 , INPUT_PULLUP );
        
	pinMode( 8 , OUTPUT );
	digitalWrite( 8 , HIGH );

	LLAP.init( );

	//LLAP.sendMessage( F( "STARTED" ) );
}

void loop() {
  
  
  if( digitalRead( 2 ) == 0 ) {
    step++;
    delay( 250 );
    send = true;
  }
  
  if( step == 0 && send == true ) {
    LLAP.sendMessage(F("LEDRE"));
    send = false;delay( 1000 );
  } else if( step == 1 && send == true ) {
    LLAP.sendMessage(F("LEDGR"));
    send = false;delay( 1000 );
  } else if( step == 2 && send == true ) {
    LLAP.sendMessage(F("LEDBL"));
    send = false;delay( 1000 );
  } else if( step == 3 && send == true ) {
    LLAP.sendMessage(F("LEDREGR"));
    send = false;delay( 1000 );
  } else if( step == 4 && send == true ) {
    LLAP.sendMessage(F("LEDREBL"));
    send = false;delay( 1000 );
  } else if( step == 5 && send == true ) {
    LLAP.sendMessage(F("LEDGRBL"));
    send = false;delay( 1000 );
  } else if( step == 6 && send == true ) {
    LLAP.sendMessage(F("LEDREGRBL"));
    send = false;delay( 1000 );
  }  else if( step == 7 && send == true ) {
    LLAP.sendMessage(F("LEDOFF"));
    send = false;delay( 1000 );
    step = -1;
  }

}
