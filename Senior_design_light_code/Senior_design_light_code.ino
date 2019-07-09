#include <FastLED.h>
//
//
//http://nuewire.com/info-archive/msgeq7-by-j-skoba/
int analogPin = PORTC1;// MSGEQ7 OUT analog pin 1, right channel
int analogPin2 = PORTC0; // MSGEQ7 OUT analog pin 0, left channel
int strobePin = PORTD4; // MSGEQ7 STROBE digital pin 6
int resetPin = PORTD6; // MSGEQ7 RESET digital pin 6
int spectrumValueRight[7]; // for right channell frequencies
int spectrumValueLeft[7];
int spectrumValue[7];
int lowerBright = PORTD2;
int upBright = PORTD3; 
#define LED_PIN     PORTD5
#define NUM_LEDS    300
#define BRIGHTNESS  155
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 colorArray;
TBlendType    currentBlending;
//http://www.instructables.com/id/Blinking-LEDs-to-the-Frequency-of-Musi/
// MSGEQ7 OUT pin produces values around 50-80
// when there is no input, so use this value to
// filter out a lot of the chaff.
int filterValue = 100;
int upState = 0;
int downState = 0;
int brightLevel[5] = {55, 105, 155, 205, 255};
volatile byte brightIndex = 3;
volatile bool flag = true;
// LED pins connected to the PWM pins on the Arduino
/* 
int ledPinR = 9;
int ledPinG = 10;
int ledPinB = 11;
 */
void setup()
{
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
   
    attachInterrupt(0, lower, FALLING);
    attachInterrupt(1, upper, FALLING);
    
    
    
    currentBlending = NOBLEND;

//   Serial.begin(9600);
    // Read from MSGEQ7 OUT
    pinMode(analogPin, INPUT);
    pinMode(analogPin2, INPUT);
    // Write to MSGEQ7 STROBE and RESET
    pinMode(strobePin, OUTPUT);
    pinMode(resetPin, OUTPUT);
    pinMode(lowerBright, INPUT);
    pinMode(upBright, INPUT);
    /*int upState = 0;
    int downState = 0;*/
    // Set analogPin's reference voltage
    analogReference(DEFAULT); // 5V
  
    // Set startup values for pins
    digitalWrite(resetPin, LOW); 
    digitalWrite(strobePin, HIGH); 
}
 
void loop()
{
  // Set reset pin low to enable strobe
  digitalWrite(resetPin, HIGH);  
  digitalWrite(resetPin, LOW);  
  //63 Hz, 160 Hz, 400 Hz, 1kHz, 2.5kHz, 6.25kHz, 16kHz
  // Get all 7 spectrum values from the 
  /*upState = digitalRead(upBright);
  downState = digitalRead(lowerBright);
  static uint8_t brightIndex = 3;
  if(upState == HIGH && brightIndex < 5)
  {
    brightIndex++;
    FastLED.setBrightness( brightLevel[brightIndex] );
  }
  if(downState == HIGH && brightIndex > 0)
  {
    brightIndex--;
    FastLED.setBrightness( brightLevel[brightIndex] );
  }
  */
  static uint8_t midCount = 0;
  static uint8_t looper = 0;
  for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = colorArray[looper];
        //if(i % 30 == 0)  leds[i] = CRGB::Red;
   } 
  FastLED.show(); 
  //FastLED.delay(10000 / UPDATES_PER_SECOND);
  //bloomEffect(colorArray, looper);
 /* sendDown(colorArray, looper);
  looper++;
  if(looper == 8)
    looper = 0;
  FastLED.show(); */
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // Allow output to settle
 
    spectrumValueRight[i] = analogRead(analogPin);
    spectrumValueLeft[i] = analogRead(analogPin2);
    if(spectrumValueRight[i] > spectrumValueLeft[i])
      spectrumValue[i] = spectrumValueRight[i];
     else
       spectrumValue[i] = spectrumValueLeft[i];
    
 
    // Constrain any value above 1023 or below filterValue
    spectrumValue[i] = constrain(spectrumValue[i], filterValue, 1023);
 
 
    // Remap the value to a number between 0 and 255
    spectrumValue[i] = map(spectrumValue[i], filterValue, 1023, 0, 255);
 
    // Remove serial stuff after debugging
 //   Serial.print(spectrumValue[i]);
//    Serial.print(" ");
    digitalWrite(strobePin, HIGH);
   }
   //to calibrate propertly will need to compare values out of range!
   if(spectrumValue[0] > 160 || spectrumValue[1] > 160)
   {
      bloomEffect(colorArray, looper);
      looper++;
      FastLED.show();
      delay(300);
   }
   else if(spectrumValue[5] > 160 || spectrumValue[4] > 160)
   {
     sendDown(colorArray, looper);
     looper++;
     FastLED.show();
     delay(300);
   }
   else if(spectrumValue[2] > 160|| spectrumValue[3] > 160)
   { 
      looper++; 
      delay(300);
   }
  // Serial.println(); 
   //first two spectrum array values go with bass
   //next two should go with mid range.
   
   if(looper == 8)
     looper = 0;
   
   /*
   analogWrite(ledPinR, spectrumValue[1]);
   analogWrite(ledPinG, spectrumValue[4]);
   analogWrite(ledPinB, spectrumValue[6]);
   */
   
}

void bloomEffect(CRGBPalette16 color, uint8_t index)
{
    CRGB current = color[index+1];
   
    int bloom = NUM_LEDS / 2;
    int downIndex = bloom;
    int startIndex = 0;
    //don't iterate bloom for another cool effect
    for(int i = 0; i < 15; i++)
    {
        if(downIndex > 0)
        {
          leds[downIndex] = current;
          leds[downIndex - 1] = current;
          leds[downIndex - 15] = current;
          leds[downIndex - 30] = current;
          leds[downIndex - 45] = current;
          leds[downIndex - 60] = current;
          leds[downIndex - 75] = current;
          leds[downIndex - 90] = current;
          leds[downIndex - 105] = current;
          leds[downIndex - 120] = current;
          leds[downIndex - 135] = current;
          
          leds[bloom + 1] = current;
          leds[bloom + 15] = current;
          leds[bloom + 30] = current;
          leds[bloom + 45] = current;
          leds[bloom + 60] = current;
          leds[bloom + 75] = current;
          leds[bloom + 90] = current;
          leds[bloom + 105] = current;
          leds[bloom + 120] = current;
          leds[bloom + 135] = current;
        }
          
         downIndex--;
         bloom++;
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
        leds[downIndex - 1] = color[index+1];//current;
        leds[downIndex - 2] = color[index+2];//current;
        leds[downIndex - 3] = color[index+3];//current;
        leds[downIndex - 4] = color[index+4];//current;
        leds[downIndex - 5] = color[index+5];//current;
     }
     if(upIndex < NUM_LEDS - 5)
     {
        leds[upIndex + 1] = color[index+1];//current;
        leds[upIndex + 2] = color[index+2];//current;
        leds[upIndex + 3] = color[index+3];//current;
        leds[upIndex + 4] = color[index+4];//current;
        leds[upIndex + 5] = color[index+5];//current;
     }
     //if(startIndex > 0)
     //{
        leds[downIndex] = black;
        leds[upIndex] = black;
     //}
     downIndex--;
     startIndex++;
     FastLED.delay(200 / UPDATES_PER_SECOND);
  }
  
   
}

void lower()
{
  //detachInterrupt(0);
  //detachInterrupt(1);
  static unsigned long last_interrupt_time = 0; 
  unsigned long interrupt_time = millis();
  if(brightIndex > 0 && (interrupt_time - last_interrupt_time > 200))
  {
 
    brightIndex--;
    FastLED.setBrightness( brightLevel[brightIndex] );
  }
  last_interrupt_time = interrupt_time;
 // attachInterrupt(0, lower, RISING);
 // attachInterrupt(1, upper, RISING);
}

void upper()
{
//  detachInterrupt(0);
 // detachInterrupt(1); 
   static unsigned long last_interrupt_time = 0;
   unsigned long interrupt_time = millis();
  if(brightIndex < 4 && (interrupt_time - last_interrupt_time > 200))
  {
    brightIndex++;
    FastLED.setBrightness( brightLevel[brightIndex] );
  }
  last_interrupt_time = interrupt_time;
//  attachInterrupt(0, lower, RISING);
//  attachInterrupt(1, upper, RISING);


}
