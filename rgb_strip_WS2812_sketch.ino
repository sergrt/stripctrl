#include "FastLED.h"

#define NUM_LEDS 114
#define DATA_PIN 13

CRGB leds[NUM_LEDS];
CRGBSet leds_ref(leds, NUM_LEDS); // ref to leds to control whole strip

unsigned long last_update_time; // Time strip was updated last time
const unsigned long fade_timeout = 3000;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalSMD5050);
  
  //FastLED.setCorrection(TypicalLEDStrip);
  //FastLED.setDither(1);
  //FastLED.setBrightness(100);

  Serial.begin(115200);
}

void waitForDataAvailable() {
  int d = 0;
  while (Serial.available() <= 0 && d < 100) {
    delay(1);
    d += 1;
  }
}

void loop() {
  while (Serial.available() > 0) {
    uint8_t _ = Serial.read();
  }

  Serial.write('R'); // Ready to receive data
  //Serial.flush();
  
  waitForDataAvailable();
  
  if (Serial.available()) {
    last_update_time = millis();
    short leds_num = 0;
    Serial.readBytes((char*)(&leds_num), 2);
    
    int dim_value = 0;
    Serial.readBytes((char*)(&dim_value), 1);

    for (int i = 0; i < leds_num; ++i) {    
      Serial.readBytes( (char*)(&leds[i]), 3);
    }
    leds_ref.fadeLightBy(dim_value);
  }

  // Turn off if there is no data for certain timeout
  if ( millis() - last_update_time > fade_timeout ) {
    leds_ref.fadeToBlackBy(255);
  }  
  FastLED.show();
}