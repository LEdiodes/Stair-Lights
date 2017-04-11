#include <Adafruit_NeoPixel.h>

#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#include <Adafruit_NeoPixel.h>

/*
 * Description: Motion activated stair lights.
 * Author: Dean Montgomery
 * Version: 2.2
 * 
 * Date: Feb 11, 2016
 * 
 * 2 PIR sesors at the top and bottom of the stairs.
 * WS28012B Addressable RGB lights - 2 LEDs on each stair - This spread out the strip of 30 and left 2-pairs for spare bulbs.
 * My Arduino is at the top of the stairs and the RGB strip is connected at the top.
 * This will cycle through several varitions of stair walkers.
 * 
 * Version 2 is a rewrite to properly handle multi-tasking the PIR sensors in parallel with LED updates.
 * TODO: Do some code cleanup, variable naming etc.
 * 
*/

#include "FastLED.h"
//#include <avr/eeprom.h>

#define NUM_LEDS 447
//#define NUM_LEDS 14
#define LEDS_PER_STAIR 30        // Number of Leds per stair.  Not yet currenlty changable - just noteable
#define BRIGHTNESS  255        // 0...255  ( used in fade7 )
#define PIN_LED 2               // LED Data pin
#define PIN_PIR_DOWN 10          // PIR Downstairs Pin
#define PIN_PIR_UP 11            // PIR Upstairs Pin
#define GO_UP -1                // Direction control - Arduino at top of stairs
#define GO_DOWN 2               // Direction control - Arduino at top of stairs
uint8_t gHue = 0;               // track color shifts.
int8_t gStair = 0;             // track curent stair.
uint8_t gBright = 0;            // track brightness
uint8_t gUpDown[NUM_LEDS];      // directional array to walk/loop up or down stairs.
int8_t gupDownDir = 1;
CRGB    leds[NUM_LEDS];         // setup leds object to access the string
CRGBPalette16 gPalette;         // some favorite and random colors for display.
CRGBPalette16 fade6 =         (CRGB( BRIGHTNESS, 0, 0),      CRGB(BRIGHTNESS,BRIGHTNESS,0), CRGB(0,BRIGHTNESS,0),
                              CRGB(0,BRIGHTNESS,BRIGHTNESS), CRGB(0,0,BRIGHTNESS),         CRGB(BRIGHTNESS, 0, BRIGHTNESS),
                              CRGB( BRIGHTNESS, 0, 0));
CRGBPalette16 z;
int8_t  gLastPalette = 15;      // track last chosen palette.
uint8_t gLastWalk = 1;
unsigned long currentMillis = millis(); // define here so it does not redefine in the loop.
long    previousMillis = 0;
long    previousOffMillis = 0; // countdown power off timer
long    offInterval = 30000; // 1000mills * 30sec
//long offInterval = 7000;
long    interval = 40;
enum Effects { ewalk, eflicker, efade6 };
Effects effect = ewalk;
enum WalkEffects { sparkle, pulsate1, pulsate2, flash };
WalkEffects walk_effect = sparkle;
// Stages of the animation.  Allows for PIR sensor to re-activation the run stage of the animation.
enum Stage { off, stage_init, stage_grow, stage_init_run, stage_run, stage_init_dim, stage_dim };
Stage stage = off;
int i = 0;
int x = 0;
uint8_t var = 0;
uint8_t valTop = 200;
uint8_t rnd = 0;
uint8_t r = 0, g = 0, b = 0, h = 0, s = 0, v = 0;
int8_t stair = 0;
CRGB c1;
CRGB c2;
CRGB trans;
CRGB trans2;


void setup() {
  delay (3000); // Power Up 3 second safety delay.
  //Serial.begin(57600);
  randomSeed(millis());
  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);  // NOTE set LED string type here. 
  FastLED.setDither( 0 );  // Stops flikering in animations.
  pinMode(PIN_PIR_DOWN, INPUT);
  pinMode(PIN_PIR_UP, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(PIN_PIR_DOWN, LOW);
  digitalWrite(PIN_PIR_UP, LOW);
  welcomeRainbow();             // rainbow - give time for PIR sensors to colibrate.
  setUpDown(GO_DOWN);           // populate the array index used for stair direction.
  setPalette();                 // setup some favorite & random colors
  stage = off;
}

// Main Loop track PIR sensors.
void loop() {
  currentMillis = millis();
  readSensors();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    update_effect(); 
    FastLED.show();
  }
  if((currentMillis - previousOffMillis > offInterval) && stage == stage_run){
    stage = stage_init_dim;
    i = 0; r = 0; g = 0; b = 0;
  }
}

void readSensors(){
  if ( digitalRead(PIN_PIR_UP) == HIGH ){  // Walk Down.
    previousOffMillis = currentMillis;
    //if ( stage == stage_run ){
    //  return;  //keep the animation fast.
    //} else 
    if ( stage == off ){
      chooseEffects();
      stage = stage_init;
      setUpDown(GO_DOWN);
    } else if ( stage == stage_dim || stage == stage_init_dim ){
      stage = stage_init_run;
    }
  } else if ( digitalRead(PIN_PIR_DOWN) == HIGH  ){ // Walk Up.
    previousOffMillis = currentMillis;
    if ( stage == off ){
      chooseEffects();
      stage = stage_init;
      setUpDown(GO_UP);
    } else if ( stage == stage_dim || stage == stage_init_dim){
      stage = stage_init_run;
    }
  }
}
   
void chooseEffects(){
  randomSeed(millis());
  r = random8(1, 255);
  //effect = efade6;
  //return;
  if ( r >= 0 && r <= 100 ){
    effect = ewalk;  // My favorite transition with random effect variations
  } else if ( r > 100 && r <= 175 ){
    effect = eflicker;  // Candle with embers.
  } else {
    effect = efade6;  // hueshift rainbow.
  } 
}

void update_effect(){
  if ( effect == ewalk ){
    walk();  
  } else if ( effect == eflicker ){
    flicker();
  } else if ( effect == efade6 ){
    fade();
  }
}

// setup walking gUpDown array in forward: 0,1,2,3... or reverse:  ...3,2,1,0
void setUpDown(int8_t upDownDir){
  gupDownDir = upDownDir;
  uint8_t gStairStart = 0;
  if (upDownDir == GO_UP){
    for ( gStair = NUM_LEDS -1; gStair >= 0; gStair-- ){
      gUpDown[gStair] = gStairStart++;
    }
  } else {
    for ( gStair = 0; gStair <= NUM_LEDS; gStair++ ){
      gUpDown[gStair] = gStairStart++;
    }  
  }
}

// Increment to the next color pair in the palette.
void choosePalette(){
  if ( gLastPalette >= 15 ) {
    gLastPalette = 0;
  } else {
    gLastPalette+=2;
  }
}

// Fill a palette with some colors that my wife picked.
void setPalette(){
  /*
   * Jenn's colors RGB  0 0 81  BLUE
   * 0 100 100 Teal 006464
   * 60 100 100 Cool White 3C6464
   * 60 10 100 Violet 3C0A64
   * 60 0 50 Purple 3C0032
   * start white fades to Teal
   * violet to purple
   * teal to blue
   * red to blue
   */
  uint8_t r = random8(1, 255); // call it once first.
  fill_solid( gPalette, 16, CRGB::Red);
  gPalette[0] = CRGB( 60, 100, 100 ); // Jenn cool white
  gPalette[1] = CRGB( 0, 90, 90 );    // Jenn teal
  gPalette[2] = CRGB( 60, 10, 100 );  // Jenn violet
  gPalette[3] = CRGB( 60, 0, 50 );    // Jenn purple
  gPalette[4] = CRGB( 0, 0, 81);      // Jenn blue
  gPalette[5] = CRGB( 100, 0, 0);     // Red
  gPalette[6] = CRGB( 0, 0, 100);     // Blue
  gPalette[7] = CRGB( 120, 0, 120);
  // Random fill the rest.
  for (uint8_t i = 8; i<16; i++){
    gPalette[i] = CRGB(random8(3,100), random8(3,100), random8(3,100));
  }
}

// Walk the stairs adding random effects.
void walk() {
  
  if ( stage == stage_init ){
    valTop = 200;
    // Pick two colors from the palette. 
    choosePalette();
    c1 = gPalette[gLastPalette];
    c2 = gPalette[gLastPalette+1];
    // chance of a random palette
    if ( random8( 5 ) == 3 ){
      c1 = CRGB(random8(3,100),random8(3,100),random8(3,100));
      c2 = CRGB(random8(3,100),random8(3,100),random8(3,100));
    }
    // fix random Black palette.
    if ( (int(c1.r) + int(c1.g) + int(c1.b)) < 8 ){
      c1 = gPalette[2];
      c2 = gPalette[4];
    }
    trans = CRGB::Black;
    trans2 = CRGB::Black;
    z[0] = c2;
    z[1] = c1;
    z[2] = CRGB(random8(2,100),random8(2,100),random8(2,100));
    z[3] = c1;
    z[4] = c2;
    //(r2-r1)/ticks * tick)
    gStair=0;
    gBright=0;
    interval=5;
    i = 0;
    x = 0;
    r = 0;
    g = 0;
    b = 0;
    walk_effect = (WalkEffects)random8( 0, 4 );
    stage = stage_grow;
  } else if ( stage == stage_grow ) {
    if (gBright < 255){
      if ( gStair < NUM_LEDS ){
        trans = blend(CRGB::Black,c1,gBright); // fade in next two
        leds[gUpDown[gStair]] = trans;
        leds[gUpDown[gStair + 1]] = trans;
      }
      if (  gStair >= 2 ) { // shift last two stairs to the 2nd color.
        trans2 = blend(c1,c2,gBright);
        leds[gUpDown[gStair - 1]] = trans2;
        leds[gUpDown[gStair - 2]] = trans2;
      }
      gBright = qadd8(gBright, 4);
    } else {
      if ( gStair < NUM_LEDS - 2 ) {
        gStair+=2;  //next stair.
      } else {
        stage = stage_init_run;
        gStair = 0;
      }
      gBright = 0;
    }
  } else if ( stage == stage_init_run ) {
    fill_solid(leds, NUM_LEDS, c2);
    x = 0;
    stage = stage_run;
  } else if ( stage == stage_run ) {
    trans2 = c2;
    randomEffect();  // waits for timer to run out.
  } else if ( stage == stage_init_dim ) {
    interval = 3;
    for(b=0; b<255; b++) {
      trans = blend(trans2,c2,b);
      fill_solid(leds, NUM_LEDS, trans);
      FastLED.show();
      FastLED.delay(8);
    }
    interval = 8;
    gBright = 0;
    gStair = 0;
    stage = stage_dim;
  } else if ( stage == stage_dim ) {
    if ( gBright <= valTop  ) {
      if ( gStair < NUM_LEDS ){
        leds[gUpDown[gStair]].fadeToBlackBy( 6 );
        leds[gUpDown[gStair + 1]].fadeToBlackBy( 6 );
        gBright+=4;
      } else {
        stage = off;
      }
    } else {
      leds[gUpDown[gStair]] = CRGB( 0, 0, 0);
      leds[gUpDown[gStair + 1]] = CRGB( 0, 0, 0);
      gStair += 2; 
      gBright = 0;
    }
  } else {
    stage = off;
  }  
}

// Random effects for the walk() stair function.
void randomEffect(){
  if ( walk_effect == sparkle ) { 
    interval = 8;
    fill_solid(leds, NUM_LEDS, c2);
    addGlitter(80);
  } else if ( walk_effect == pulsate1 ) {
    interval = 10;
    if ( b < 255 ){
      if ( i < 4 ) {
        trans2 = blend(z[i],z[i+1],b);
        fill_solid(leds, NUM_LEDS, trans2);
        b=qadd8(b,1);
      } else {
        i = 0;
      }
    } else {
      i++;
      b=0;
    }
  } else if ( walk_effect == pulsate2 ) {
    interval = 5;
    for(gStair=0; gStair < NUM_LEDS; gStair++) {
      trans2 = blend(c1,c2,quadwave8(r+=( -20 * gupDownDir )));
      leds[gStair] = trans2;
    }
    gStair = 0;
    r = ++g;
  } else if ( walk_effect == flash ) {
    if ( x == 0 ) {
      for(gStair=0; gStair < NUM_LEDS; gStair+=2) {
        leds[gUpDown[gStair]] = CRGB( 100, 100, 100);
        leds[gUpDown[gStair + 1]] = CRGB( 100, 100, 100);
        FastLED.show();
        FastLED.delay(1);
      }
      for(gStair=0; gStair < NUM_LEDS; gStair+=2) {
        leds[gUpDown[gStair]] = c2;
        leds[gUpDown[gStair+1]] = c2;
        FastLED.show();
        FastLED.delay(1);
      }
      x = 1;
      gStair=0;
    } 
  }
}

// Sparkle rainbow welcome give delay to calibrate pir sensors.  This also indicates if program crashed.
void welcomeRainbow(){
  for ( int i = 0; i < 500; i++ ){
    rainbowWithGlitter();
    FastLED.show();
    FastLED.delay(8.3);
    EVERY_N_MILLISECONDS( 20 ) { gHue++; }
  }
  for (int tick=0; tick < 64; tick++){ 
    for ( uint8_t i = 0; i < NUM_LEDS; i++ ){
      leds[i].fadeToBlackBy( 64 );
      FastLED.show();
      FastLED.delay(1);
    }
  }
}

// built-in FastLED rainbow, plus some random sparkly glitter
void rainbowWithGlitter() {
  rainbow();
  addGlitter(80);
}

// paint rainbow
void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

// Add random glitter
void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB(100,100,100);
  }
}

// Candle flicker, blown out, + ember glow
void flicker(){
  if ( stage == stage_init ){
    i = 0;
    rnd = 0;
    r = 0; g = 0; b = 0;
    stair = 0;
    gStair = 0;
    x = 0;
    gBright = 0;
    interval = 27;
    stage = stage_grow;
  } else if ( stage == stage_grow ){
    if ( i <= 10 ){  // number of flicker between steps
      if ( gStair < NUM_LEDS ){  // for each step
        for ( stair = 0; stair <= gStair; stair +=2 ){  // up to currently lit step.
          rnd = random8(1, 4);
          if ( rnd == 2 ){
            gBright = random8(110,140);
            leds[gUpDown[stair]] = CHSV( 60, 200, gBright );
            leds[gUpDown[stair + 1]] = CHSV( 60, 200, gBright );
          }
        }
        i++;
      } else {
        stage = stage_init_run;
      }
    } else {
      i = 0;
      gStair += 2;
    }
  } else if ( stage == stage_init_run ){
    stage = stage_run;
  } else if ( stage == stage_run ){
    for( gStair = 0; gStair < NUM_LEDS; gStair+=2) {  
      rnd = random8(1, 4);
      if ( rnd == 2 ){
        gBright = random8(110,140);
        leds[gStair] = CHSV( 60, 200, gBright );
        leds[gStair+1] = CHSV( 60, 200, gBright );
      }
    }
  } else if ( stage == stage_init_dim ){
    // Blow out candles and leave an ember.
    for(gStair=0; gStair < NUM_LEDS; gStair+=2) {
      rnd = random8(4, 6);
      r = rnd+1;
      g = rnd-2;
      leds[gUpDown[gStair]] = CRGB( r,g,0 );
      leds[gUpDown[gStair + 1]] = CRGB( r,g,0 );
      FastLED.show();
      FastLED.delay(50);
    }
    i = 0;
    gStair=0;
    stage = stage_dim;
  } else if ( stage == stage_dim ){
    if ( i <= 150 ){
      rnd = random8(0, NUM_LEDS);
      leds[gUpDown[rnd]].fadeToBlackBy( 3 );
      i++;
    } else {
      fill_solid(leds, NUM_LEDS, CRGB( 0, 0, 0 ));
      FastLED.show();
      stage = off;
    }
  } else {
    stage = off;
  }  
}

// Fade6 effect with each led using a hue shift
void fade(){
  if ( stage == stage_init ){
    gBright = 0;
    gStair = 0;
    interval = 5;
    h = 128;
    s = 140;
    v = BRIGHTNESS;
    r = 0;
    g = ( random8() < 120 );
    stage = stage_grow;
  } else if ( stage == stage_grow ){
    if ( gBright<255 ){
      if ( gStair < NUM_LEDS ){
        trans = blend(CHSV(h,s,0),CHSV(h,s,v),gBright);
        leds[gUpDown[gStair]] = trans;
        leds[gUpDown[gStair + 1]] = trans;
        gBright = qadd8(gBright, 1);
      } else {
        stage = stage_init_run;
        gBright=0;
        gStair=0;
      }
      gBright = qadd8(gBright, 2);
    } else {
      gBright = 0;
      gStair += 2;
    }
  } else if ( stage == stage_init_run ) {
    v = BRIGHTNESS;
    interval = 70;
    stage = stage_run;
  } else if ( stage == stage_run ){
    r = h;
    for(gStair=0; gStair < NUM_LEDS; gStair++) {
        h+=(3*gupDownDir); // left PIR go down
        leds[gUpDown[gStair]] = CHSV(h, s, v);
    }
    h = r + (3*gupDownDir*-1);
  } else if ( stage == stage_init_dim ){
    interval = 7;
    h = h - gStair;
    gStair = 0;
    stage = stage_dim;
  } else if ( stage == stage_dim ){
    if ( v > 0 ) {
      if ( gStair < NUM_LEDS ){
        leds[gUpDown[gStair]] = CHSV(gStair + h, s, v);
        leds[gUpDown[gStair + 1]]= CHSV(gStair + h, s, v);
        v = qsub8(v, 1);
      } else {
        stage = off;
      }
    } else {
      leds[gUpDown[gStair]] = CRGB( 0, 0, 0);
      leds[gUpDown[gStair + 1]] = CRGB( 0, 0, 0);
      gStair += 2; 
      v = BRIGHTNESS;
      h+=2;
    }
  } else {
    stage = off;
  }
}



