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
#define NUM_OF_PIXELS 47

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
int lastPixelPtr = -1;
String incomingSerialData;
int commaIdx;  // Index number of the comma (',').
//
int sequenceStack[NUM_OF_PIXELS];
int pixelNumber = 0;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
  initSequenceStack();
}

void loop() {
  if (Serial.available() > 0) {
    incomingSerialData = Serial.readString();
    incomingSerialData.trim();
    commaIdx = incomingSerialData.indexOf(',');
    if (commaIdx != -1) {
      int firstNum = incomingSerialData.substring(0, commaIdx + 1).toInt();
      int secondNum = incomingSerialData.substring(commaIdx + 1).toInt();
      if (firstNum > 0 && firstNum <= NUM_OF_PIXELS + 1 && secondNum >= 0 && secondNum <= 255) {
        // firstNum (int) equals 0 is also the value for non-numeric strings.
        lastPixel.number = incomingSerialData.substring(0, commaIdx + 1).toInt();
        lastPixel.number--;  // To correctly reference a Pixel in the strip.
        lastPixel.brightness = incomingSerialData.substring(commaIdx + 1).toInt();
        lastPixelPtr++;
        pixelArr[lastPixelPtr] = lastPixel;
        colorWipe(lastPixel.number, strip.Color(0, 0, 0, lastPixel.brightness));
      }
    } else {
      incomingSerialData.toLowerCase();
      if (incomingSerialData.compareTo("pop") == 0 && lastPixelPtr >= 0) {
        if (lastPixel.number > 0 && lastPixel.brightness > 0 ) {
          // Turn off the last Pixel.
          colorWipe(pixelArr[lastPixelPtr].number, strip.Color(0, 0, 0, 0));
          lastPixelPtr--;
        }
      } else if (incomingSerialData.compareTo("clear") == 0) {
        turnOffAll();  // Turn off all Pixels.
      } else if (incomingSerialData.compareTo("test") == 0) {
        testPixels(31);
      } else if (incomingSerialData.compareTo("setseq") == 0) {
        setLightingSequence(); 
      }
    }
  }
}

void initSequenceStack() {
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    sequenceStack[i] = -1;
  }  
}

// Prints the lighting sequence set by the user.
int setLightingSequence() {
  String incomingData;
  int prevPixel;
  int currentPixel = 0;
  int stackPtr = 0;  // 
  turnOffAll();
  colorWipe(currentPixel, strip.Color(currentPixel, 0, 0, 31));  // Turn on the first Pixel.
  while (true) {
    if (Serial.available() > 0) {
      incomingData = Serial.readString();
      incomingData.trim();
      incomingData.toLowerCase();
      if (incomingData.compareTo("f") == 0 && currentPixel < NUM_OF_PIXELS - 1) {
        prevPixel = currentPixel;
        currentPixel++;  // Go to the next Pixel of the strip.
        colorWipe(prevPixel, strip.Color(0, 0, 0, 0));  // Turn off the previous Pixel.
        colorWipe(currentPixel, strip.Color(0, 0, 0, 31));  // Turn on the current Pixel.
      } else if (incomingData.compareTo("jf") == 0) {
        prevPixel = currentPixel;
        currentPixel = currentPixel + 5;
        if (currentPixel >= NUM_OF_PIXELS - 1) {
          // Go to the last Pixel of the strip.
          currentPixel = NUM_OF_PIXELS - 1;
        }
        colorWipe(prevPixel, strip.Color(0, 0, 0, 0));  // Turn off the previous Pixel.
        colorWipe(currentPixel, strip.Color(0, 0, 0, 31));  // Turn on the current Pixel.
      } else if (incomingData.compareTo("p") == 0 && currentPixel > 0) {
        prevPixel = currentPixel;
        currentPixel--;  // Go to previous Pixel of the strip.
        colorWipe(prevPixel, strip.Color(0, 0, 0, 0));  // Turn off the previous Pixel.
        colorWipe(currentPixel, strip.Color(0, 0, 0, 31));  // Turn on the current Pixel.
      } else if (incomingData.compareTo("jp") == 0) {
        prevPixel = currentPixel;
        currentPixel = currentPixel - 5;
        if (currentPixel < 0) {
          // Go to the first Pixel of the strip.
          currentPixel = 0;
        }
        colorWipe(prevPixel, strip.Color(0, 0, 0, 0));  // Turn off the previous Pixel.
        colorWipe(currentPixel, strip.Color(0, 0, 0, 31));  // Turn on the current Pixel.
      } else if (incomingData.compareTo("push") == 0) {
        // Give to the element referenced by stackPtr (int) in the sequenceStack array the value
        // of the position of the current Pixel.
        sequenceStack[stackPtr] = currentPixel + 1;  // The Pixels are numbered from 0 to
                                                     // NUM_OF_PIXELS.
        stackPtr++;
        printLightingSeq();
      } else if (incomingData.compareTo("pop") == 0) {
        // Clear the value of the element referenced by stackPtr (int) in the sequenceStack array.
        stackPtr--;
        sequenceStack[stackPtr] = -1;
        printLightingSeq();
      } else if (incomingData.compareTo("q") == 0) {
        printLightingSeq();
        turnOffAll();
        return 0;
      }
    }
  }
}

// Prints the lighting sequence as array to copy and paste in an Arduino code.
void printLightingSeq() {
  Serial.print('{');
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    if (sequenceStack[i] != -1) {
      Serial.print(sequenceStack[i]);
      Serial.print(", "); 
    }
  }
  Serial.println('}');
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
void testPixels(int brightness) {
  turnOffAll();
  delay(200);
  // Turn on one Pixel at a time and maintains it.
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    colorWipe(i, strip.Color(0, 0, 0, brightness));
    delay(200);
  }
  // Turn off one Pixel at a time.
  for (int i = NUM_OF_PIXELS - 1; i >= 0; i--) {
    colorWipe(i, strip.Color(0, 0, 0, 0));
    delay(200);
  }
  for (int k = 0; k < 3; k++) {
    if (k % 2 != 0) {
      // Decrease the brightness level from brightness (int) to 0 of each Pixel.
      for (int i = brightness; i >= 0; i = i - 3) {
        for (int j = NUM_OF_PIXELS - 1; j >= 0; j--) {
          colorWipe(j, strip.Color(0, 0, 0, i));
        }
      }    
    } else {
      // Increase the brightness level from 0 to brightness (int) of each Pixel.
      for (int i = 0; i < brightness + 1; i = i + 3) {
        for (int j = 0; j < NUM_OF_PIXELS; j++) {
          colorWipe(j, strip.Color(0, 0, 0, i));
        }
      }
    }
  }
}
