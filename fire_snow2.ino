/*
 * Fire-snow concentric style
 * OK on blackSNOW. Better on e-aster.
 * 
 * warmbit @ 2021OCT06
 * 
 * ref: sun radiation circular
 * https://wokwi.com/arduino/projects/288437917385228808
 * 
 */

#include "FastLED.h"

#define LED_PIN       A1
#define COLOR_ORDER   GRB
#define LED_TYPE      WS2812B

#define BRIGHTNESS    255

// just for mapping
#define NUM_ROWS 13 
#define NUM_COLS 11
const uint8_t kMatrixWidth = NUM_COLS;
const uint8_t kMatrixHeight = NUM_ROWS;

//#define NUM_LEDS (NUM_ROWS * NUM_COLS)
#define NUM_LEDS 25

CRGB leds[NUM_LEDS];
byte bump[(NUM_COLS+2)*(NUM_ROWS+2)];
CRGB cHsvLUT[256];

void setup() {
  
  delay(3000); // sanity delay
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  generateCHsvLUT();
}

void loop() {
  generateBump();
  bumpMap();
  FastLED.show();
}

void generateCHsvLUT() {
  for (int j= 0; j<256; j++) {
    cHsvLUT[j] = HeatColor(j/1.4);
  }
}

void generateBump () {
  int t = millis()/2;
  int index = 0;
  for (byte j = 0; j < (NUM_ROWS+2); j++) {
    for (byte i = 0; i < (NUM_COLS+2); i++) {                     
      byte col = (inoise8_raw(i*40, j*40, t))/2; 
      bump[index++] = col;
    } 
  }
} 

void bumpMap() {
  int yindex=(NUM_COLS+3);
  int8_t vly=-(NUM_ROWS/2+1);
  for (byte y = 0; y < NUM_ROWS; y++) { 
    ++vly;
    int8_t vlx=-(NUM_COLS/2+1);
    for (byte x = 0; x < NUM_COLS; x++) {
      ++vlx;
      int8_t nx=bump[x+yindex+1]-bump[x+yindex-1];
      int8_t ny=bump[x+yindex+(NUM_COLS+2)]-bump[x+yindex-(NUM_COLS+2)];
      byte difx=abs8(vlx*7-nx);
      byte dify=abs8(vly*7-ny);
      int temp = difx*difx+dify*dify;
      
      //int col = 255-temp/13;
      //int col = 255-temp/11;
      // reverse: edge hotter, inner darker
      int col = temp/11;
      
      if (col<0) col=0;
      
      uint8_t index = XY(x, y);
      if (index != 25) {
        leds[index] = cHsvLUT[col];
      }
    }
    yindex += (NUM_COLS+2);
  }
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
