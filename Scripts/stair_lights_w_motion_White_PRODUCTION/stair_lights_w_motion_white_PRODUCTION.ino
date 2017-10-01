// Edit by Serge Niko June 2015

#include <Adafruit_NeoPixel.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(447, PIN, NEO_GRB + NEO_KHZ800); // the first variable is the number of LEDs

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// Set up Variables
 unsigned long timeOut=60000; // timestamp to remember when the PIR was triggered.
 int downUp = 0;              // variable to rememer the direction of travel up or down the stairs
 int alarmPinTop = 10;        // PIR at the top of the stairs
 int alarmPinBottom =11;      // PIR at the bottom of the stairs
 int alarmValueTop = LOW;    // Variable to hold the PIR status
 int alarmValueBottom = LOW; // Variable to hold the PIR status
 int ledPin = 13;           // LED on the arduino board flashes when PIR activated
 int LDRSensor = A0;        // Light dependant resistor
 int LDRValue = 0;          // Variable to hold the LDR value
 int colourArray[350];      // An array to hold RGB values
 int change = 1;            // used in 'breathing' the LED's
 int breathe = 0;           // used in 'breathing' the LED's
 

void setup() {
   strip.begin();
   strip.setBrightness(255); //adjust brightness here
   strip.show(); // Initialize all pixels to 'off'
   Serial.begin (9600);  // only requred for debugging
   pinMode(ledPin, OUTPUT);  // initilise the onboard pin 13 LED as an indicator
   pinMode(alarmPinTop, INPUT_PULLUP);     // for PIR at top of stairs initialise the input pin and use the internal restistor
   pinMode(alarmPinBottom, INPUT_PULLUP);  // for PIR at bottom of stairs initialise the input pin and use the internal restistor
   delay (200); // it takes the sensor 2 seconds to scan the area around it before it can 2000 was default
   //detect infrared presence.

}

 void loop() {

    if (timeOut+15700 < millis()) {        // idle state - 'breathe' the top and bottom LED to show program is looping
       uint32_t blue = (0, 0, breathe);
       breathe = breathe + change;
       strip.setPixelColor(0, blue);
       strip.setPixelColor(27, blue);
       strip.setPixelColor(28, blue);
       strip.setPixelColor(57, blue);
       strip.setPixelColor(58, blue);
       strip.setPixelColor(87, blue);
       strip.setPixelColor(88, blue);
       strip.setPixelColor(117, blue);
       strip.setPixelColor(118, blue);
       strip.setPixelColor(147, blue);
       strip.setPixelColor(148, blue);
       strip.setPixelColor(177, blue);
       strip.setPixelColor(178, blue);
       strip.setPixelColor(207, blue);
       strip.setPixelColor(208, blue);
       strip.setPixelColor(237, blue);
       strip.setPixelColor(238, blue);
       strip.setPixelColor(267, blue);
       strip.setPixelColor(268, blue);
       strip.setPixelColor(297, blue);
       strip.setPixelColor(298, blue);
       strip.setPixelColor(327, blue);
       strip.setPixelColor(328, blue);
       strip.setPixelColor(357, blue);
       strip.setPixelColor(358, blue);
       strip.setPixelColor(387, blue);
       strip.setPixelColor(388, blue);
       strip.setPixelColor(417, blue);
       strip.setPixelColor(418, blue);
       strip.setPixelColor(446, blue);
       strip.show();
       if (breathe == 100 || breathe == 0) change = -change;      // breathe the LED from 0 = off to 100 = fairly bright
       if (breathe == 100 || breathe == 0); delay (100);           // Pause at beginning and end of each breath
     delay(10);

   
  }
  
    alarmValueTop = digitalRead(alarmPinTop);    // Constantly poll the PIR at the top of the stairs
    //Serial.println(alarmPinTop);
    alarmValueBottom = digitalRead(alarmPinBottom);  // Constantly poll the PIR at the bottom of the stairs
    //Serial.println(alarmPinBottom);
    
    if (alarmValueTop == HIGH && downUp != 2)  {      // the 2nd term allows timeOut to be contantly reset if one lingers at the top of the stairs before decending but will not allow the bottom PIR to reset timeOut as you decend past it.
      timeOut=millis();  // Timestamp when the PIR is triggered.  The LED cycle wil then start.
      downUp = 1;
      //clearStrip();
      topdown();         // lights up the strip from top down
    }
 
    if (alarmValueBottom == HIGH && downUp != 1)  {    // the 2nd term allows timeOut to be contantly reset if one lingers at the bottom of the stairs before decending but will not allow the top PIR to reset timeOut as you decend past it.
      timeOut=millis();    // Timestamp when the PIR is triggered.  The LED cycle wil then start.
      downUp = 2;
      //clearStrip();
      bottomup();         // lights up the strip from bottom up
    }

    if (timeOut+10000 < millis() && timeOut+15000 < millis()) {    //switch off LED's in the direction of travel.
       if (downUp == 1) {
          colourWipeDown(strip.Color(0, 0, 0), 200); // Off
       }
       if (downUp == 2)  {
        colourWipeUp(strip.Color(0, 0, 0), 200);   // Off
       }
      downUp = 0;
      
    }
            
   

} 

 void topdown() {
    Serial.println ("detected top");                  // Helpful debug message
    colourWipeDown(strip.Color(10, 10, 10), 300 );    // Warm White - RGB - then 4th number is delay
    //for(int i=0; i<3; i++) {                        // Helpful debug indication flashes led on Arduino board twice
      //digitalWrite(ledPin,HIGH);
     // delay(1000);
      //digitalWrite(ledPin,LOW);   
      //delay(1000);
    //}
 }



 void bottomup() {
    Serial.println ("detected bottom");            // Helpful debug message
    colourWipeUp(strip.Color(10, 10, 10), 300);    // Warm White
    //for(int i=0; i<3; i++) {                     // Helpful debug indication flashes led on Arduino board twice
      //digitalWrite(ledPin,HIGH);
      // delay(1000);
      //digitalWrite(ledPin,LOW);
      //delay(200);
    //}
  }

 // Fade light each step strip
 void colourWipeDown(uint32_t c, uint16_t wait) {

  for (uint16_t j = 0; j < 15; j++){
  int start = strip.numPixels()/15 *j;
  Serial.println(j);
    
        for (uint16_t i = start; i < start + 35; i++){
    strip.setPixelColor(i, c);
        }
      strip.show();  
  delay(wait);
  }
 
 }


void clearStrip(){
  for (int l=0; l<strip.numPixels(); l++){
    strip.setPixelColor(l, (0,0,0));
  }
       
}
 // Fade light each step strip
 void colourWipeUp(uint32_t c, uint16_t wait) {
   for (uint16_t j = 15; j > 0; j--){
   int start = strip.numPixels()/15 *j;
   Serial.println(j);
      //start = start-1;
          for (uint16_t i = start; i > start - 35; i--){
          strip.setPixelColor(i-1, c);
         }
          strip.show();
  delay(wait);
  }  
  
 }

 


