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

const int buttonPin = 17; 
const int lightTriggerPin =  13;
const int soundTriggerPin =  18;

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

boolean lauflichtStatus = false, lightStatus = false, soundStatus = false;

int frameCount=0, lightCount = 0, soundCount = 0;
int updateEachNFrame = 7; //since the LED refresh rate is quite slow, the visuals should be calculated more often then the LEDs are refreshed

void setup() {
  FastLED.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness (brightness);
  FastLED.setCorrection(TypicalLEDStrip);
  //FastLED.setTemperature( OvercastSky );
  pinMode(lightTriggerPin, OUTPUT);
  pinMode(soundTriggerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("INITIALIZED");
  FastLED.show();
}


void loop() {
  checkButtonState();
  showLED();
  if(lauflichtStatus){
    doOneTimeStep();
  }
  if(lightStatus)showLight();
  if(soundStatus)playSound();
  calculateFps(1); 
}

void checkButtonState() {
  if(lauflichtStatus == false){
    int buttonState = digitalRead(buttonPin);
    if (buttonState == LOW) {
      lauflichtStatus = true;
      soundStatus = true;
    }
  }
}

void showLED(){
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
  
  // just update the Stripe each updateEachNFrame, to have the visuals with the fade out more often calulated then the refresh of the Stripe allows it
  frameCount++;
  boolean updateLED=false;
  if(frameCount>updateEachNFrame){
    updateLED=true;
    frameCount=0;
  }
  if(updateLED)FastLED.show();
}


void doCycleStep() {
  pos=pos+speed; //do one step
  if(pos>num_leds)pos=0; //CYCLE-Mode: when the line is moved till the end - start from the beginning again
}

void doOneTimeStep() {
  pos=pos+speed; //do one step
  lightStatus = false; //somehow if not setting the variable to false all the time it sometimes is set to 255
  if(pos>num_leds){ //when the line is moved till the end: set it back to start and stop the execution
    pos=0;
    lauflichtStatus = false;
    lightStatus = true;
  }
}

void showLight(){
  if(lightCount<30){
    digitalWrite(lightTriggerPin, HIGH); 
    lightCount++;
  }
  else{
    digitalWrite(lightTriggerPin, LOW); 
    lightCount = 0;
    lightStatus = false;
  }
}

void playSound(){
  if(soundCount<30){
    digitalWrite(soundTriggerPin, HIGH); 
    soundCount++;
  }
  else{
    digitalWrite(soundTriggerPin, LOW); 
    soundCount = 0;
    soundStatus = false;
  } 
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
