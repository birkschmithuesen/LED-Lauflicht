#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

#define NUM_LEDS_PER_STRIP 576
#define NUM_STRIPS 3
#define BRIGHTNESS 30

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

// Pin layouts on the teensy 3:
// OctoWS2811: 2,14,7,8,6,20,21,5
// pin 15 & 16 - Connect together, but do not use

float speed = 1;
int lineWidth = 1;
int pos = 0;

int num_leds = NUM_STRIPS * NUM_LEDS_PER_STRIP - 200;
int totalLengt = NUM_STRIPS * NUM_LEDS_PER_STRIP;
CRGB lineColor = CRGB(255,255,255);
int brightness = 255;
int traceR = 235;
int traceG = 200;
int traceB = 220;

int frameCount=0;
int updateEachNFrame = 7;

void setup() {
  FastLED.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness (brightness);
  FastLED.setCorrection(TypicalLEDStrip);
  //FastLED.setTemperature( OvercastSky );
  Serial.println("INITIALIZED");
  FastLED.show();
}


void loop() {
  show();
  step();
  //calculateFps(1); 
}

void show(){
  fadeUsingColor(leds, totalLengt, CRGB(traceR, traceG, traceB));
  for(int i=0; i<lineWidth; i++){
    int linePos = constrain(pos+i, 0, num_leds);
    // revers the 3rd stripe
    int lengthOfTwoStripes = 2 * NUM_LEDS_PER_STRIP;
    if(linePos >= lengthOfTwoStripes){
      int reversePos = lengthOfTwoStripes + (NUM_LEDS_PER_STRIP - (linePos - lengthOfTwoStripes));
      leds[reversePos] = lineColor;
    }
    else leds[linePos] = lineColor;
  }
  // just update the Stripe each updateEachNFrame, to have the cisuals with the fade out more often calulated then the refresh of the Stripe allows it
  frameCount++;
  boolean updateLED=false;
  if(frameCount>updateEachNFrame){
    updateLED=true;
    frameCount=0;
  }
  if(updateLED)FastLED.show();
}

void step() {
  pos=pos+speed; //do one step
  if(pos>num_leds)pos=0; //CYCLE-Mode: when the line is moved till the end - start from the beginning again
}

static inline void calculateFps(const int seconds){
  // Create static variables so that the code and variables can
  // all be declared inside a function
  static unsigned long lastMillis;
  static unsigned long frameCount;
  static unsigned int framesPerSecond;
  
  // It is best if we declare millis() only once
  unsigned long now = millis();
  frameCount ++;
  if (now - lastMillis >= seconds * 1000) {
    framesPerSecond = frameCount / seconds;
    Serial.println(framesPerSecond);
    frameCount = 0;
    lastMillis = now;
  }
}
