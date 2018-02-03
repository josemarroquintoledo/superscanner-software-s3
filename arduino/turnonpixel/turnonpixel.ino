/*
  Turning On a NeoPixel's Pixel

  This sketch is used to turn or or off one LED of a NeoPixel
  ring or grid at a time. It is part of the SuperScanner
  project (GPL-2.0) [1].
  
  The LED ring or grid plays the rol of illuminating the
  sample observed with a microscope in the way that is
  required by the computational imaging technique Fouerier
  Ptychography.

  Some code block was taken from the strandtest example sketch
  by Adafruit (LGPL-3.0) [2].

  Created on February 2nd, 2018
  by Jose David Marroquin Toledo

  Modified on February 3rd, 2018
  by Jose David Marroquin Toledo

  [1] https://github.com/josemarroquintoledo/superscanner-software-s3
  [2] https://github.com/adafruit/Adafruit_NeoPixel
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUM_OF_PIXELS 7

struct pixel {
  int number = 0;
  int brightness = 0;
};

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_PIXELS, PIN, NEO_GRBW + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

pixel pixelArr[NUM_OF_PIXELS];
pixel lastPixel;
int pixelArrPtr = 0;
String line;
int commaIdx;  // Index number of the comma (',').

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    line = Serial.readString();
    line.trim();
    commaIdx = line.indexOf(',');
    if (commaIdx != -1) {
      int firstNum = line.substring(0, commaIdx + 1).toInt();
      int secondNum = line.substring(commaIdx + 1).toInt();
      if (firstNum > 0 && firstNum <= NUM_OF_PIXELS + 1 && secondNum >= 0 && secondNum <= 255) {
        // firstNum (int) equals 0 is also the value for non-numeric strings.
        lastPixel.number = line.substring(0, commaIdx + 1).toInt();
        lastPixel.number--;  // To correctly reference a Pixel in the strip.
        lastPixel.brightness = line.substring(commaIdx + 1).toInt();
        pixelArr[pixelArrPtr] = lastPixel;
        colorWipe(lastPixel.number, strip.Color(0, 0, 0, lastPixel.brightness));
      }
    } else {
      line.toLowerCase();
      if (line.compareTo("-1") == 0) {
        if (lastPixel.number > 0 && lastPixel.brightness > 0 ) {
          // Turn off the last Pixel.
          colorWipe(lastPixel.number, strip.Color(0, 0, 0, 0)); 
        }
      } else if (line.compareTo("-2") == 0) {
        turnOffAll();  // Turn off all Pixels.
      } else if (line.compareTo("test") == 0) {
        testPixels();
      }
    }
  }
}

// Turns on a pixel with a color.
void colorWipe(uint16_t pixel, uint32_t color) {
  strip.setPixelColor(pixel, color);
  strip.show();
}

// Turns off all pixels (NUM_OF_PIXELS).
void turnOffAll() {
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    colorWipe(i, strip.Color(0, 0, 0, 0));
  }
}

// Launches a test routine for all Pixels (NUM_OF_PIXELS).
void testPixels() {
  turnOffAll();
  delay(250);
  // Turn on one Pixel at a time and maintains it.
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    colorWipe(i, strip.Color(0, 0, 0, 127));
    delay(250);
  }
  // Turn off one Pixel at a time.
  for (int i = NUM_OF_PIXELS - 1; i >= 0; i--) {
    colorWipe(i, strip.Color(0, 0, 0, 0));
    delay(250);
  }
  for (int k = 0; k < 3; k++) {
    if (k % 2 != 0) {
      // Decrease the brightness level from 255 to 0 of each Pixel.
      for (int i = 255; i >= 0; i--) {
        for (int j = NUM_OF_PIXELS - 1; j >=0; j--) {
          colorWipe(j, strip.Color(0, 0, 0, i));
        }
      }    
    } else {
      // Increase the brightness level from 0 to 225 of each Pixel.
      for (int i = 0; i < 256; i++) {
        for (int j = 0; j < NUM_OF_PIXELS; j++) {
          colorWipe(j, strip.Color(0, 0, 0, i));
        }
      }
    }
  }
}
