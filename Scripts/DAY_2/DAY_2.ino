// Edit by Serge Niko June 2015
// Edit by Matthew Kerfoot November 2016
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor!

#include <Adafruit_NeoPixel.h> // adds neopixel library

#define PIN 6 // LED strip data pin

Adafruit_NeoPixel strip = Adafruit_NeoPixel(447, PIN, NEO_GRB + NEO_KHZ800); // the first variable is the number of LEDs

// Variables
  unsigned long timeOut = 100;     // timestamp to remember when the PIR was triggered.
  int downUp            = 0;       // variable to rememer the direction of travel up or down the stairs
  int alarmPinTop       = 10;      // motion sensor at the top of the stairs
  int alarmPinBottom    = 11;      // motion sensor at the bottom of the stairs
  int alarmValueTop     = LOW;     // Variable to hold the PIR status
  int alarmValueBottom  = LOW;     // Variable to hold the PIR status
  int ledPin            = 13;      // LED on the arduino board flashes when PIR activated
  int LDRSensor         = A0;      // Light dependant resistor
  int LDRValue          = 0;       // Variable to hold the LDR value
  int change            = 1;       // used in 'breathing' the LED's
  int breathe           = 0;       // used in 'breathing' the LED's
  int colourArray[350];            // An array to hold RGB values
  
    void setup() {
         strip.begin();                           // send power to LED strip PIN
         strip.setBrightness(50);                 //adjust brightness here - goes to 255
         strip.show();                            // Initialize all pixels to 'off'
         Serial.begin(9600);                      // for serial debugging
         pinMode(ledPin, OUTPUT);                 // initialize pin 13(LED light)
         pinMode(alarmPinTop, INPUT_PULLUP);      // for PIR at top of stairs initialise the input pin and use the internal restistor
         pinMode(alarmPinBottom, INPUT_PULLUP);   // for PIR at bottom of stairs initialise the input pin and use the internal restistor
         delay (2000);                            // it takes the sensor 2 seconds to scan the area around it before it can detect infrared presence
    }

     void loop() {
      strip.setBrightness(155);
        if (timeOut+15700 < millis()) {           // idle state - 'breathe' the top and bottom LED to show program is looping
           uint16_t blue  = (0, 0, breathe);       // sets the LEDs blue
           uint32_t green = strip.Color(0, breathe, 0);      // sets the LEDs 
           breathe = breathe + change;
           strip.setPixelColor(0, blue);
           strip.setPixelColor(27, blue);
           strip.setPixelColor(28, green);
           strip.setPixelColor(57, green);
           strip.setPixelColor(58, blue);
           strip.setPixelColor(87, blue);
           strip.setPixelColor(88, green);
           strip.setPixelColor(117, green);
           strip.setPixelColor(118, blue);
           strip.setPixelColor(147, blue);
           strip.setPixelColor(148, green);
           strip.setPixelColor(177, green);
           strip.setPixelColor(178, blue);
           strip.setPixelColor(207, blue);
           strip.setPixelColor(208, green);
           strip.setPixelColor(237, green);
           strip.setPixelColor(238, blue);
           strip.setPixelColor(267, blue);
           strip.setPixelColor(268, green);
           strip.setPixelColor(297, green);
           strip.setPixelColor(298, blue);
           strip.setPixelColor(327, blue);
           strip.setPixelColor(328, green);
           strip.setPixelColor(357, green);
           strip.setPixelColor(358, blue);
           strip.setPixelColor(387, blue);
           strip.setPixelColor(388, green);
           strip.setPixelColor(417, green);
           strip.setPixelColor(418, blue);
           strip.setPixelColor(446, blue);
           strip.show();
           
        if (breathe == 100 || breathe == 0) change = -change;      // breathe the LED from 0 = off to 100 = fairly bright
        if (breathe == 100 || breathe == 0); delay (25);          // Pause at beginning and end of each breath
     }
  strip.setBrightness(50);
    alarmValueTop = digitalRead(alarmPinTop);        // Constantly poll the PIR at the top of the stairs
    Serial.println(alarmPinTop);                     // serial informational message
    alarmValueBottom = digitalRead(alarmPinBottom);  // Constantly poll the PIR at the bottom of the stairs
    Serial.println(alarmPinBottom);                  // informational serial message
    
    if (alarmValueTop == HIGH && downUp != 2)  {      // the 2nd term allows timeOut to be contantly reset if one lingers at the top of the stairs before decending but will not allow the bottom PIR to reset timeOut as you decend past it.
      timeOut=millis();                               // Timestamp when the PIR is triggered.  The LED cycle wil then start.
      downUp = 1;
      topdown();                                      // lights up the strip from top down
    }
 
    if (alarmValueBottom == HIGH && downUp != 1)  {    // the 2nd term allows timeOut to be contantly reset if one lingers at the bottom of the stairs before decending but will not allow the top PIR to reset timeOut as you decend past it.
      timeOut=millis();                                // Timestamp when the PIR is triggered.  The LED cycle wil then start.
      downUp = 2;
      bottomup();                                      // lights up the strip from bottom up
    }
    
    if (timeOut+10000 < millis() && timeOut+15000 < millis()) {    //switch off LED's in the direction of travel.
       if (downUp == 1) {
          colourWipeDown(strip.Color(0, 0, 0), 100);               // Off
       }
       if (downUp == 2)  {
        colourWipeUp(strip.Color(0, 0, 0), 100);                   // Off
       }
      downUp = 0;
    }
} 
 void topdown() {
    Serial.println ("detected top");
    colourWipeDown(strip.Color(0, 0, 255), 200 );                   // 4th number is delay
 }
 void bottomup() {
    Serial.println ("Motion detected from the bottom of the stairs");
    colourWipeUp(strip.Color(0, 0, 255), 200);
  }
 // Fade light each step strip
 void colourWipeDown(uint32_t c, uint16_t wait) {

  for (uint16_t j = 0; j < 15; j++){
  int start = strip.numPixels()/15 *j;
  Serial.println(j);
    
        for (uint16_t i = start; i < start + 30; i++){
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
          for (uint16_t i = start; i > start - 30; i--){
          strip.setPixelColor(i-1, c);
         }
          strip.show();
  delay(wait);
  }  
}
