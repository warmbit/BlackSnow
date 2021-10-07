/*
 * fire on blacksnow
 * 
 * 2021Oct04 warmbit
 * 
 * 2021OCT04
 * this is just too random for snowflake.
 * it requires a mapping to reflect the structure of snowflake.
 * 
 * 2021OCT04
 * all experiments are not good enough. Perhaps it is good to mapped to e-aster.
 * - just map e-aster to an approximate XY matrix
 * - make a rotating fire wheel using e-aster, that will be interesting
 *   rotation + fire simulation two in one.
 *   
 * 2021OCT06  
 * use this as a fire demo for blacksnow and e-aster
 * for e-aster, need a new mapping!
 * 
 */
#include "FastLED.h"

#define LED_PIN       A1
#define COLOR_ORDER   GRB
#define LED_TYPE      WS2812B

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND   60

#define NUM_ROWS 13 
#define NUM_COLS 11
const uint8_t kMatrixWidth = NUM_COLS;
const uint8_t kMatrixHeight = NUM_ROWS;

#define SNOW_LEDS      25
#define NUM_SNOW      20
#define NUM_LEDS      (NUM_SNOW*SNOW_LEDS + 1)

// fire simulation uses this!
bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

// Fire2012 with programmable Color Palette
//
// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function.
//
// Four different static color palettes are provided here, plus one dynamic one.
// 
// The three static ones are: 
//   1. the FastLED built-in HeatColors_p -- this is the default, and it looks
//      pretty much exactly like the original Fire2012.
//
//  To use any of the other palettes below, just "uncomment" the corresponding code.
//
//   2. a gradient from black to red to yellow to white, which is
//      visually similar to the HeatColors_p, and helps to illustrate
//      what the 'heat colors' palette is actually doing,
//   3. a similar gradient, but in blue colors rather than red ones,
//      i.e. from black to blue to aqua to white, which results in
//      an "icy blue" fire effect,
//   4. a simplified three-step gradient, from black to red to white, just to show
//      that these gradients need not have four components; two or
//      three are possible, too, even if they don't look quite as nice for fire.
//
// The dynamic palette shows how you can change the basic 'hue' of the
// color palette every time through the loop, producing "rainbow fire".

CRGBPalette16 gPal;

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  // This first palette is the basic 'black body radiation' colors,
  // which run from black to red to bright yellow to white.
  gPal = HeatColors_p;
  
  // These are other ways to set up the color palette for the 'fire'.
  // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  
  // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  
  // Third, here's a simpler, three-step gradient, from black to red to white
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);

}

void loop() {
  int  a = millis();
  int  a1 = a/3;
  for (uint8_t j = 0; j < NUM_ROWS; j++) { 
    for (uint8_t i = 0; i < NUM_COLS; i++) {
      uint16_t ledIndex = XY(i,j);
      if (ledIndex != SNOW_LEDS) {
        for (uint8_t n = 0; n < NUM_SNOW; n++) {
          leds[n*NUM_SNOW+ledIndex] = HeatColor(qsub8 (inoise8 (i * 50, j * 50+a, a1), abs8(j - (NUM_ROWS-1)) * 255 / (NUM_ROWS+20)));
        }
      }
    }
  }

  FastLED.show();
}

uint8_t XY (uint8_t x, uint8_t y) {
  // 25 is a guard, there is no actual LED there for snowflake
  // if 25 returned, it is not visible at all.
  //
  // this is a mapping from xy matrix to snowflake
  const uint8_t XYTable[] = {
    25,  25,  25,  25,  25,  10,  25,  25,  25,  25,  25,
    25,  25,  25,  25,  25,  11,  25,  25,  25,  25,  25,
     7,  25,  25,  25,  25,  25,  25,  25,  25,  25,  13,
    25,  25,  25,  25,  25,  12,  25,  25,  25,  25,  25, 
    25,   8,  25,  25,  25,  25,  25,  25,  25,  14,  25,
    25,  25,   9,  25,   1,  25,   2,  25,  15,  25,  25,
    25,  25,  25,   6,  25,   0,  25,   3,  25,  25,  25,
    25,  25,  24,  25,   5,  25,   4,  25,  18,  25,  25,
    25,  23,  25,  25,  25,  25,  25,  25,  25,  17,  25,
    25,  25,  25,  25,  25,  21,  25,  25,  25,  25,  25,
    22,  25,  25,  25,  25,  25,  25,  25,  25,  25,  16,
    25,  25,  25,  25,  25,  20,  25,  25,  25,  25,  25,
    25,  25,  25,  25,  25,  19,  25,  25,  25,  25,  25
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

/*
 
void loop()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());

  // Fourth, the most sophisticated: this one sets up a new palette every
  // time through the loop, based on a hue that changes every time.
  // The palette is a gradient from black, to a dark color based on the hue,
  // to a light color based on the hue, to white.
  //
  //   static uint8_t hue = 0;
  //   hue++;
  //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
  //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
  //   gPal = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);


  Fire2012WithPalette(); // run simulation frame, using palette colors
  
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}


// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      uint8_t colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

*/
