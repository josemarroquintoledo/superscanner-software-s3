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

  [1] https://github.com/josemarroquintoledo/superscanner-software-s3
  [2] https://github.com/adafruit/Adafruit_NeoPixel
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

struct pixel {
  int number;
  int brightness;
};

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRBW + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

pixel led;

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
  // Waits for the pixel number
  while (Serial.available() == 0) {
  }
  led.number = Serial.parseInt();
  Serial.println(led.number);
  // Waits for the pixel's brightness
  while (Serial.available() == 0) {
  }
  led.brightness = Serial.parseInt();
  Serial.println(led.brightness);
  if (led.number <= PIN) {
    if (led.brightness >= 0 && led.brightness <= 255) {
      colorWipe(led.number, strip.Color(0, 0, 0, led.brightness));  // White color.
    }
  }
}

// Turns on a pixel with a color
void colorWipe(uint16_t pixel, uint32_t color) {
  strip.setPixelColor(pixel, color);
  strip.show();
}
