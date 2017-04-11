// Edit by Serge Niko June 2015
// Edit by Matthew Kerfoot November 2016
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor!

#include <Adafruit_NeoPixel.h> // adds neopixel library
#define PIN 6                  // LED strip data pin

Adafruit_NeoPixel strip = Adafruit_NeoPixel(447, PIN, NEO_GRB + NEO_KHZ800); // the first variable is the number of LEDs

  unsigned long timeOut = 0;     // variable used to extend or shorten the length in time the LEDs stay lite
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
  int colorArray[350];             // An array to hold RGB values
  
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
                      if (timeOut+15700 < millis()) {                  // idle state - 'breathe' the top and bottom LED to show program is looping
                         uint16_t blue  = (0, 0, breathe);             // sets the LEDs blue
                         uint32_t green = strip.Color(0, breathe, 0);  // sets the LEDs 
                         breathe = breathe + change;                   // every loop the it adds a change or fade to the breathe(RGB color)
                         strip.setPixelColor(0, blue);                 // each individual LED used in the idle breathe pattern is called out with the color
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
                         strip.show();                                           // This puts into effect everything called out above it.           
                    if ( breathe == 100 || breathe == 0 ) change = -change;      // breathe the LED from 0 = off to 100 = fairly bright
                    if ( breathe == 100 || breathe == 0 ); delay (25);           // Pause at beginning and end of each breath
     }                                                                           // end of breathe\idle action
     
    alarmValueTop = digitalRead(alarmPinTop);        // Constantly poll the PIR at the top of the stairs
    alarmValueBottom = digitalRead(alarmPinBottom);  // Constantly poll the PIR at the bottom of the stairs
    
    if ( alarmValueTop == HIGH && downUp != 2 )  {                                                // the 2nd term allows timeOut to be contantly reset if one lingers at the top of the stairs before decending but will not allow the bottom PIR to reset timeOut as you decend past it.
                                                    timeOut=millis();                             // Timestamp when the PIR is triggered.  The LED cycle wil then start.
                                                    downUp = 1;                                   // when downUp equals 1 the stair lights will turn off from the top of the stairs to the bottom
                                                    Serial.println ("downUp = 1");                // informational debugging message
                                                    topdown();                                    // lights up the strip from top down
                                                    Serial.println ("topdown()");                 // informational debugging message
    }                                                                                             // end of if statement
 
    if (alarmValueBottom == HIGH && downUp != 1)  {                                               // the 2nd term allows timeOut to be contantly reset if one lingers at the bottom of the stairs before decending but will not allow the top PIR to reset timeOut as you decend past it.
                                                    timeOut=millis();                             // Timestamp when the PIR is triggered.  The LED cycle wil then start
                                                    downUp = 2;                                   // when downUp equals 2 the stair lights will turn off from the bottom of the stairs to the top
                                                    Serial.println ("downUp = 2");                // informational debugging message
                                                    bottomup();                                   // lights up the strip from bottom up
                                                    Serial.println ("bottomup()");                // informational debugging message
    }                                                                                             // end of if statement
    
    if (timeOut+10000 < millis() && timeOut+15000 < millis()) {                                                                 // switch off LED's in the direction of travel.
                                                                 if (downUp == 1) {                                             // when downUp equals 1 the stair lights will turn off from the top of the stairs to the bottom
                                                                                     colorWipeDown(strip.Color(0, 0, 0), 100);  // Off
                                                                                           strip.setPixelColor(436, 0,0,0);
                                                                                           strip.setPixelColor(437, 0,0,0);
                                                                                           strip.setPixelColor(438, 0,0,0);
                                                                                           strip.setPixelColor(439, 0,0,0);
                                                                                           strip.setPixelColor(440, 0,0,0);
                                                                                           strip.setPixelColor(441, 0,0,0);
                                                                                           strip.setPixelColor(442, 0,0,0);
                                                                                           strip.setPixelColor(443, 0,0,0);
                                                                                           strip.setPixelColor(444, 0,0,0);
                                                                                           strip.setPixelColor(445, 0,0,0);
                                                                                           strip.setPixelColor(446, 0,0,0);
                                                                                           strip.setPixelColor(447, 0,0,0);
                                                                                         strip.show();
                                                                 }                                                              // end of if statement
                                                                   if (downUp == 2)  {                                          // when downUp equals 2 the stair lights will turn off from the bottom of the stairs to the top
                                                                                        colorWipeUp(strip.Color(0, 0, 0), 100); // Off
                                                                                              strip.setPixelColor(436, 0,0,0);
                                                                                              strip.setPixelColor(437, 0,0,0);
                                                                                              strip.setPixelColor(438, 0,0,0);
                                                                                              strip.setPixelColor(439, 0,0,0);
                                                                                              strip.setPixelColor(440, 0,0,0);
                                                                                              strip.setPixelColor(441, 0,0,0);
                                                                                              strip.setPixelColor(442, 0,0,0);
                                                                                              strip.setPixelColor(443, 0,0,0);
                                                                                              strip.setPixelColor(444, 0,0,0);
                                                                                              strip.setPixelColor(445, 0,0,0);
                                                                                              strip.setPixelColor(446, 0,0,0);
                                                                                              strip.setPixelColor(447, 0,0,0);
                                                                                            strip.show();
                                                                   }                                                            // end of if statement
                                                                    downUp = 0;                                                 // resets downUp to 0
    }                                                                                                                           // end of if statement
}                                                                                                                               // end of if statement

 void topdown() {                                                                // When the infrared sensor at the top of the stairs is triggered the stairs will light up from the top to the bottom
                  Serial.println ("motion detected from the TOP of the stairs"); // informational serial message
                  colorWipeDown(strip.Color(0, 0, 255), 220 );                    // RGB values - 4th number is delay
                        strip.setPixelColor(436, 0,0,255);
                        strip.setPixelColor(437, 0,0,255);
                        strip.setPixelColor(438, 0,0,255);
                        strip.setPixelColor(439, 0,0,255);
                        strip.setPixelColor(440, 0,0,255);
                        strip.setPixelColor(441, 0,0,255);
                        strip.setPixelColor(442, 0,0,255);
                        strip.setPixelColor(443, 0,0,255);
                        strip.setPixelColor(444, 0,0,255);
                        strip.setPixelColor(445, 0,0,255);
                        strip.setPixelColor(446, 0,0,255);
                        strip.setPixelColor(447, 0,0,255);
                      strip.show();
 }                                                                               // end of topdown

 void bottomup() {                                                                    // When the infrared sensor at the bottom of the stairs is triggered the stairs will light up from the bottom to the top
                    Serial.println ("motion detected from the BOTTOM of the stairs"); // informational serial message
                    strip.setBrightness(10);
                    colorWipeUp(strip.Color(255, 255, 255), 220 );                         // RGB color then 4th number is delay between steps
                          strip.setPixelColor(0, 255, 255, 255);
                          strip.setPixelColor(1, 255, 255, 255);
                          strip.setPixelColor(2, 255, 255, 255);
                          strip.setPixelColor(3, 255, 255, 255);
                          strip.setPixelColor(4, 255, 255, 255);
                          strip.setPixelColor(5, 255, 255, 255);
                          strip.setPixelColor(6, 255, 255, 255);
                          strip.setPixelColor(7, 255, 255, 255);
                          strip.setPixelColor(8, 255, 255, 255);
                          strip.setPixelColor(9, 255, 255, 255);
                          strip.setPixelColor(10, 255, 255, 255);
                          strip.setPixelColor(11, 255, 255, 255);
                          strip.setPixelColor(12, 255, 255, 255);
                          strip.setPixelColor(13, 255, 255, 255);
                          strip.setPixelColor(14, 255, 255, 255);
                          strip.setPixelColor(15, 255, 255, 255);
                          strip.setPixelColor(16, 255, 255, 255);
                          strip.setPixelColor(17, 255, 255, 255);
                          strip.setPixelColor(18, 255, 255, 255);
                          strip.setPixelColor(19, 255, 255, 255);
                          strip.setPixelColor(20, 255, 255, 255);
                          strip.setPixelColor(21, 255, 255, 255);
                          strip.setPixelColor(22, 255, 255, 255);
                          strip.setPixelColor(23, 255, 255, 255);
                          strip.setPixelColor(24, 255, 255, 255);
                          strip.setPixelColor(25, 255, 255, 255);
                          strip.setPixelColor(26, 255, 255, 255);
                          strip.setPixelColor(27, 255, 255, 255);
                          strip.setPixelColor(28, 255, 255, 255);
                          strip.setPixelColor(29, 255, 255, 255);
                          strip.setPixelColor(436, 255, 255, 255);
                          strip.setPixelColor(437, 255, 255, 255);
                          strip.setPixelColor(438, 255, 255, 255);
                          strip.setPixelColor(439, 255, 255, 255);
                          strip.setPixelColor(440, 255, 255, 255);
                          strip.setPixelColor(441, 255, 255, 255);
                          strip.setPixelColor(442, 255, 255, 255);
                          strip.setPixelColor(443, 255, 255, 255);
                          strip.setPixelColor(444, 255, 255, 255);
                          strip.setPixelColor(445, 255, 255, 255);
                          strip.setPixelColor(446, 255, 255, 255);
                          strip.setPixelColor(447, 255, 255, 255);
                        strip.show(); 
 }                                                                                    // end of bottomup

 void colorWipeDown(uint32_t c, uint16_t wait) {   // Turns stairs of one by one from the top of the stairs to the bottom
  for (uint16_t j = 0; j < 15; j++){               // turns steps on one by one
  int start = strip.numPixels()/15 *j;             // creates the steps.
  Serial.println(j);                               // informational message
    
        for (uint16_t i = start; i < start + 30; i++){ 
                                                        strip.setPixelColor(i, c);
        }
      strip.show();  
  delay(wait);
  } 
 }                                                                                  // end of colorWipeDown
 
void clearStrip(){
  for (int l=0; l<strip.numPixels(); l++){
    strip.setPixelColor(l, (0,0,0));
  }
} 
 // Fade light each step strip
 void colorWipeUp(uint32_t c, uint16_t wait) {
   for (uint16_t j = 15; j > 0; j--){
   int start = strip.numPixels()/15 *j;
   Serial.println(j);
      //start = start-1;
          for (uint16_t i = start; i > start - 30; i--){
          strip.setPixelColor(i+1, c);
         }
         strip.setPixelColor(0, 255, 255, 255);
        //                  strip.setPixelColor(1, 0, 0, 0);
          //                strip.setPixelColor(2, 0, 0, 0);
            //              strip.setPixelColor(3, 0, 0, 0);
              //            strip.setPixelColor(4, 0, 0, 0);
                //          strip.setPixelColor(5, 0, 0, 0);
                  //        strip.setPixelColor(6, 0, 0, 0);
                    //      strip.setPixelColor(7, 0, 0, 0);
                      //    strip.setPixelColor(8, 0, 0, 0);
                        //  strip.setPixelColor(9, 0, 0, 0);
    //                      strip.setPixelColor(10, 0, 0, 0);
      //                    strip.setPixelColor(11, 0, 0, 0);
        //                  strip.setPixelColor(12, 0, 0, 0);
          //                strip.setPixelColor(13, 0, 0, 0);
            //              strip.setPixelColor(14, 0, 0, 0);
              //            strip.setPixelColor(15, 0, 0, 0);
                //          strip.setPixelColor(16, 0, 0, 0);
                  //        strip.setPixelColor(17, 0, 0, 0);
                    //      strip.setPixelColor(18, 0, 0, 0);
                      //    strip.setPixelColor(19, 0, 0, 0);
                        //  strip.setPixelColor(20, 0, 0, 0);
                          //strip.setPixelColor(21, 0, 0, 0);
  //                        strip.setPixelColor(22, 0, 0, 0);
    //                      strip.setPixelColor(23, 0, 0, 0);
     //                     strip.setPixelColor(24, 0, 0, 0);
       //                   strip.setPixelColor(25, 0, 0, 0);
         //                 strip.setPixelColor(26, 0, 0, 0);
           //               strip.setPixelColor(27, 0, 0, 0);
             //             strip.setPixelColor(28, 0, 0, 0);
               //           strip.setPixelColor(29, 0, 0, 0);
          strip.show();
  delay(wait);
  }  
}
