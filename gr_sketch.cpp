/*GR-KURUMI Sketch Template Version: V1.12*/
/* made by DIGI-P (C) 2016 */
/* This AS-IS file is offered with a BSD lite license */

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

// Pin 22,23,24 are assigned to RGB LEDs.
int led_red   = 22; // LOW active
int led_green = 23; // LOW active
int led_blue  = 24; // LOW active

char bit[62];
int phase = 0;
unsigned long cntTime = 0;
int prev = 0; 
int ready = 0;
unsigned char sec, min, hour;
unsigned int cnt = 0;

void myCycle( unsigned long msec )
{
  int curr = digitalRead(2);	
	
  if (curr < prev) { // UP
  	cntTime = 0;
  	cnt++;
  	if (cnt >= 8) cnt = 0;
  	digitalWrite(led_green, LOW);
  }
  if (curr > prev) { // DOWN
    int val = 0;
    digitalWrite(led_green, HIGH);
//  	Serial.println( cntTime );

  	if ((cntTime>50 ) && (cntTime<350)) { // Marker
  		val = -1;
  	} else
  	if ((cntTime>350) && (cntTime<650)) { // 1
  		val = 1;
  	} else
  	if ((cntTime>650) && (cntTime<950)) { // 0 
  		val = 0;
  	} else {
  		// error
  		phase = 0;
 //   	Serial.println( "E");
		cntTime = 0;
  		goto MY_EXIT;
  	}
  	cntTime = 0;
  	if (phase >= sizeof(bit) ) {
		cntTime = 0;
  		goto MY_EXIT;
  	}
    bit[phase] = val;
  	
    switch (phase) {
    case 0:
	    digitalWrite(led_red, HIGH);
    	if (val != -1) {
    		goto MY_EXIT;
    	}
 		for (int i=0; i<sizeof(bit); i++ ) {
 			bit[i] = 0;
 		}
    	phase++;
    	break;
    case 1:
    	if (val != -1) {
    		phase = 0;
			goto MY_EXIT;
    	}
    	
    	phase++;
  		digitalWrite(led_red, LOW);
//		Serial.println( "" );
//		Serial.print( "*" );
    	break;
    case 59:
//    	Serial.print( val );
    	min = 10*(bit[2]*4 + bit[3]*2 + bit[4])
    		+ bit[6]*8 + bit[7]*4 +bit[8]*2 + bit[9];
    	hour = 10*(bit[13]*2 + bit[14])
    		+ bit[16]*8 + bit[17]*4 +bit[18]*2 + bit[19];
    	ready = 1;
    	phase = 0;
		break;
		
    default:
//    	Serial.print( val );
//    	if (val == -1) Serial.println( "" );
   		phase++;
   		if (phase > 60) phase = 0;
    } 	
  }
  
  if (curr == prev) {
  	if (curr == 0) {
  		cntTime++;
  	}
  	if (curr == 1) {
  		cntTime = 0;
  	}
  }
  
MY_EXIT:
  prev = curr;
}


// the setup routine runs once when you press reset:
void setup() {
  //setPowerManagementMode(PM_STOP_MODE, 0, 1023); //Set CPU STOP_MODE in delay()
  //setOperationClockMode(CLK_LOW_SPEED_MODE); //Set CPU clock from 32MHz to 32.768kHz

  // initialize the digital pin as an output.
  Serial.begin(115200);
  Serial.println("GO!");
//  Serial.println(getTemperature(TEMP_MODE_CELSIUS)); //temperature from the sensor in MCU
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode( 2, INPUT);

  // turn the LEDs on, glow white.
  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, HIGH);
  digitalWrite(led_blue, HIGH);
  matrix.begin(0x70);
  matrix.clear();
  matrix.writeDisplay();

  attachIntervalTimerHandler( myCycle );
}

// the loop routine runs over and over again forever:
void loop() {
	int x,y;
	int v;
	
	for (x=0, v=cnt; x<8; x++) {
    	if (x == v) {
    	    matrix.drawPixel( x, 0, LED_RED );
    	    matrix.drawPixel( 7-x, 7, LED_RED );
    	} else {
    	    matrix.drawPixel( x, 0, LED_OFF );
    	    matrix.drawPixel( 7-x, 7, LED_OFF );
    	}
	}
    for (x=0; x<4; x++) {
        for ( y=0; y<6; y++) {
        	if (bit[1 + x + 10*y]) matrix.drawPixel( x, y+1, LED_YELLOW );
        	else matrix.drawPixel( x, y+1, LED_OFF );
        }
    }
    for (x=4; x<8; x++) {
        for ( y=0; y<6; y++) {
        	if (bit[2 + x + 10*y]) matrix.drawPixel( x, y+1, LED_GREEN );
        	else matrix.drawPixel( x, y+1, LED_OFF);
        }
    }
    matrix.drawPixel( 0, 1, LED_OFF );
    if (bit[54]) matrix.drawPixel( 5, 6, LED_RED );
    else matrix.drawPixel( 5, 6, LED_OFF );
    if (bit[55]) matrix.drawPixel( 6, 6, LED_RED );
    else matrix.drawPixel( 6, 6, LED_OFF );
    matrix.writeDisplay();

	delay(100);
	
	if (ready) {
		ready = 0;
#if 0
		Serial.println( "" );
    	Serial.println( "##########" );
    	Serial.print( hour );
	   	Serial.print( ":" );
    	Serial.println( min );
    	Serial.println( "##########" );
#endif
	}
}
