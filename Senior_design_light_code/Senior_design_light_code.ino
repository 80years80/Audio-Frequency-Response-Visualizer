#include <FastLED.h>

#define LED_PIN     PORTD5
#define NUM_LEDS    300
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 colorArray;
TBlendType    currentBlending;



void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    CRGB white  = CRGB::White;
    CRGB red    = CHSV( HUE_RED, 255, 255);
    CRGB blue   = CHSV( HUE_BLUE, 255, 255);
    CRGB yellow = CHSV( HUE_YELLOW, 255, 255);
    CRGB pink   = CHSV( HUE_PINK, 255, 255);
    colorArray = CRGBPalette16(
                               green, purple, red, blue,
                               yellow, pink, white, black,
                               green, purple, red, blue,
                               yellow, pink, white, black );
    
    
    
    
    currentBlending = NOBLEND;
    int looper = 0;
}

void loop()
{
  static uint8_t looper = 0;
  for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = colorArray[looper];
        //if(i % 30 == 0)  leds[i] = CRGB::Red;
   } 
  FastLED.show(); 
  FastLED.delay(10000 / UPDATES_PER_SECOND);
  //bloomEffect(colorArray, looper);
  sendDown(colorArray, looper);
  looper++;
  if(looper == 8)
    looper = 0;
  FastLED.show(); 
}


void bloomEffect(CRGBPalette16 color, uint8_t index)
{
    CRGB current = color[index+1];
   
    int bloom = NUM_LEDS / 2;
    int downIndex = bloom;
    int startIndex = 0;
    for( bloom; bloom < NUM_LEDS; bloom++)
    {
        if(downIndex > 0)
          leds[downIndex - 1] = current;
          
          leds[bloom + 1] = current;
        
          
         
         downIndex--;
         FastLED.delay(500 / UPDATES_PER_SECOND);
    }
    

}

void sendDown(CRGBPalette16 color, uint8_t index)
{
  CRGB black = color[7];
  CRGB current = color[index+1];
  int upIndex = NUM_LEDS / 2;
  int downIndex = upIndex; 
  int startIndex = 0;
  for(upIndex; upIndex < NUM_LEDS; upIndex++)
  {
     if(downIndex > 5)
     {
        leds[downIndex - 1] = current;
        leds[downIndex - 2] = current;
        leds[downIndex - 3] = current;
        leds[downIndex - 4] = current;
        leds[downIndex - 5] = current;
     }
     if(upIndex < NUM_LEDS - 5)
     {
        leds[upIndex + 1] = current;
        leds[upIndex + 2] = current;
        leds[upIndex + 3] = current;
        leds[upIndex + 4] = current;
        leds[upIndex + 5] = current;
     }
     //if(startIndex > 0)
     //{
        leds[downIndex] = black;
        leds[upIndex] = black;
     //}
     downIndex--;
     startIndex++;
     FastLED.delay(500 / UPDATES_PER_SECOND);
  }
  
   
}
