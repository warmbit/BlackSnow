/*
 *  snowflake 2023 warmbit
 *  
 *  the base sketch is on the github, snow_twinkle03.ino
 *  
 *  
 */
/*
 * File: fadein
 * 
 * By: Andrew Tuline
 * 
 * Date: April, 2019
 * 
 * Based previous work (namely twinklefox) by Mark Kriegsman, this program shows how you can fade-in twinkles by using the fact that a random number generator
 * with the same seed will generate the same numbers every time. Combine that with millis and a sine wave and you have twinkles fading in/out.
 * 
 * Consider this a poor man's version of twinklefox.
 * 
 * Source: https://gist.github.com/atuline
 * 
 * the palette, the changing od color is smooth fade in and out.
 * 
 * THIS is GOOD.
 * 
 */

#include "FastLED.h"                                          // FastLED library.

// Fixed definitions cannot change on the fly.
#define LED_PIN     A1                                             // Data pin to connect to the strip.
#define COLOR_ORDER GRB                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE    WS2812B                                       // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define NUM_LEDS    50   // Number of LED's.

#define BRIGHTNESS  64
#define MIN_BRIGHTNESS  8
#define MAX_BRIGHTNESS  128 

// Global variables can be changed on the fly.
uint8_t max_bright = 128;                                     // Overall brightness.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

// Palette definitions
CRGBPalette16 currentPalette = PartyColors_p;
CRGBPalette16 targetPalette;
TBlendType    currentBlending = LINEARBLEND;                  // NOBLEND or LINEARBLEND

#define UPDATES_PER_SECOND 10

const uint8_t rings[5][6] = {
{ 0, 0, 0, 0, 0, 0},
{ 1, 2, 3, 4, 5, 6},
{ 9,12,15,18,21,24},
{ 8,11,14,17,20,23},
{ 7,10,13,16,19,22},
};

// 0 is always at the center
const uint8_t shortBranches[6][3] = {
{ 9,  8,  7},
{12, 11, 10},
{15, 14, 13},
{18, 17, 16},
{21, 20, 19},
{24, 23, 22},
};

// both short and long branches does not have all LEDs on the board
const uint8_t longBranches[3][7] = {
{ 7,  8,  9, 0, 18, 17, 16},
{10, 11, 12, 0, 21, 20, 19},
{13, 14, 15, 0, 24, 23, 22}, 
};

void setup() {

  LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
 
  FastLED.setBrightness(max_bright);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);               // FastLED Power management set at 5V, 500mA.

  SetupMyPalette();
  
} // setup()

uint8_t gCurrentPatternNumber = 0;
uint8_t InitNeeded = 1;

static uint8_t colorMotion = 0;

#define ARRAY_SIZE(A) (sizeof(A)/sizeof((A)[0]))
typedef void (*PatternList[])();
PatternList gPatterns = {
  ringout,
  pacifica_loop,
  rainbowbeat,
  twinkle,
  pride,
  breathe
};

void loop() {
  // breathe
  //static uint8_t start = 0;
  //breathe(start);  
  //start += 16;

  // rainbowmarch
  //rainbowmarch(200, 10);
  //rainbowbeat();
  //twinkle();
  //pride();
  //FastLED.show();
  /*
  ChangePalettePeriodically();
  uint8_t maxChanges = 24; 
  nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  */
  EVERY_N_SECONDS(60) {
    gCurrentPatternNumber = (gCurrentPatternNumber+1)%ARRAY_SIZE(gPatterns);  
  }

  gPatterns[gCurrentPatternNumber]();

  if (gCurrentPatternNumber == 0) {
    colorMotion += 2; // steps to move     
  }
  FastLED.show();
}

void test_loop() {

  ChangePalettePeriodically();
  uint8_t maxChanges = 24; 
  nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  
  static uint8_t startIndex = 0;
  //twinkle();
  //FillRingsFromPaletteColors(startIndex);

  startIndex = startIndex + 2; /* motion speed */
  
  FastLED.show();
  FastLED.delay(1000/UPDATES_PER_SECOND);
  
} // loop()

// the direction is from the center to outlines
//void FillRingsFromPaletteColors( uint8_t colorIndex)
void ringout()
{
    uint8_t steps = (millis()/125)%256;
    
    uint8_t colorIndex = colorMotion;
    // LINEARBLEND is better than NOBLEND, it makes the animation smoother.
    uint8_t i, j, index;
    CRGB c1 = ColorFromPalette( currentPalette, (steps)%256, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c2 = ColorFromPalette( currentPalette, (steps+4)%256, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c3 = ColorFromPalette( currentPalette, (steps+8)%256, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c4 = ColorFromPalette( currentPalette, (steps+12)%256, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c5 = ColorFromPalette( currentPalette, (steps+16)%256, MAX_BRIGHTNESS, LINEARBLEND);

    for (i = 0; i < 5; i++) {
      for (j = 0; j < 6; j++) {
        index = rings[i][j];
        switch(i) {
          case 0:
            leds[index] = c1;
            leds[index+25] = c1;
            break;
          case 1: 
            leds[index] = c2;
            leds[index+25] = c2;
            break;
          case 2:
            leds[index] = c3;
            leds[index+25] = c3;
            break;
          case 3: 
            leds[index] = c4;
            leds[index+25] = c4;
            break;
          case 4:
            leds[index] = c5;
            leds[index+25] = c5;
            break;
          default:
            leds[index] = CRGB::Gray;
            break;
        }
      }
      //colorIndex += 2; // 4, small steps better?
    }
}

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 120;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    CRGB r = CHSV( HUE_RED, 255, 255);
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB y = CHSV( HUE_YELLOW, 255, 255);
    CRGB o = CHSV( HUE_ORANGE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB b = CRGB::Blue;
    CRGB black = CRGB::Black;
    CRGB w = CRGB::White;
    CRGB gray = CRGB::Gray;
    
    if( secondHand ==  0)  { targetPalette = RainbowColors_p; }
    if( secondHand == 20)  { targetPalette = CRGBPalette16( r,r,y,y, p,p,o,o, g,g,b,b, r,g,b,black); }
    if( secondHand == 40)  { targetPalette = CRGBPalette16( black,w,black,w, r,y,b,p, p,b,o,y, g,r,g,r); }
    //if( secondHand == 60)  { targetPalette = LavaColors_p; }
    if( secondHand == 60)  { targetPalette = CRGBPalette16( black,gray,black,r, r,gray,b,gray, p,w,o,w, g,w,g,w); }
    //if( secondHand == 80)  { targetPalette = CloudColors_p; }
    if( secondHand == 80)  { targetPalette = CRGBPalette16( r,black,p,w, black,y,g,o, r,gray,b,o, black,r,g,r); }
    //if( secondHand == 100)  { targetPalette = PartyColors_p; }
    if( secondHand == 100)  { targetPalette = CRGBPalette16( r,b,y,b, p,o,b,r, g,r,b,r, r,g,b,black); }
    
  }
}

void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

void SetupMyPalette()
{
    CRGB r = CHSV( HUE_RED, 255, 255);
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB y = CHSV( HUE_YELLOW, 255, 255);
    CRGB o = CHSV( HUE_ORANGE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB b = CRGB::Blue;
    /*
    currentPalette = CRGBPalette16(g, g, r,  r,
                                   y, y, b,  b,
                                   p, p, r,  b,
                                   b, r, b,  y);
    */                               
    currentPalette = RainbowColors_p;
}

void twinkle() {

  random16_set_seed(535);                                                           // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.

  for (int i = 0; i<NUM_LEDS; i++) {
    uint8_t fader = sin8(millis()/random8(10,20));                                  // The random number for each 'i' will be the same every time.
    leds[i] = ColorFromPalette(currentPalette, i*20, fader, currentBlending);       // Now, let's run it through the palette lookup.
  }

  random16_set_seed(millis());                                                      // Re-randomizing the random number seed for other routines.

} // twinkle()

// breathing with changing color
void breathe() {
  uint8_t start = random8();
  uint8_t brightness = (exp(sin(millis() / 2000.0 * PI)) - 0.368) * 42.546;
  
  FastLED.setBrightness(brightness);
  
  //Serial.println(brightness);
  uint8_t flag = 1;

  if (brightness < 1 && flag == 1) {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV((i*20+start)%255,255,255);
    }
    flag = 0;
  }
  if (brightness > 90 && flag == 0) {
    flag = 1;  
  }

  // used as one pattern
  //FastLED.show();      
}

// this is too fast
void rainbowmarch(uint8_t thisdelay, uint8_t deltahue) {
  uint8_t thishue = millis()*(256-thisdelay)/255;
  //thishue = beatsin8(50,0,255);
  //thishue = beat8(50); 
  fill_rainbow(leds, NUM_LEDS, thishue, deltahue);
}

void rainbowbeat() {
  uint8_t beatA = beatsin8(17, 0, MAX_BRIGHTNESS);
  uint8_t beatB = beatsin8(13, 0, MAX_BRIGHTNESS);
  fill_rainbow(leds, NUM_LEDS, (beatA+beatB)/2, 8);  
}

void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}

void fadeIn(byte steps) {
  for (byte i = steps+1; i>=0; i--) {
    gPatterns[gCurrentPatternNumber]();
    byte fadeOut = lerp8by8(MAX_BRIGHTNESS, 0, 255*i/steps);
    FastLED.setBrightness(fadeOut);
    FastLED.show();
  }  
}

void fadeOut(byte steps) {
  for (byte i = 0; i<=steps; i++) {
    gPatterns[gCurrentPatternNumber]();
    byte fadeOut = lerp8by8(MAX_BRIGHTNESS, 0, 255*i/steps);
    FastLED.setBrightness(fadeOut);
    FastLED.show();
  }  
}

// pacifica
CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };

void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}
