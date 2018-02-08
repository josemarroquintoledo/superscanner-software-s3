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

  Modified on February 7th, 2018
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

// Example of lighting sequence obtained with setLightingSequence() (int).
const int PIXEL_SEQ[NUM_OF_PIXELS] =  {1, 2, 3, 4, 5, 6, 7, 9, 8, 23, 22, 21, 20, 19, 18, 17, 16,
                                       15, 14, 13, 12, 11, 10, 47, 24, 25, 26, 27, 28, 29, 30, 31,
                                       32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46};
pixel lastPixel;
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
  strip.show();
  Serial.begin(9600);
  Serial.println("<ArduinoReady>");
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
        colorWipe(lastPixel.number, lastPixel.brightness);
      }
    } else {
      incomingSerialData.toLowerCase();
      if (incomingSerialData.compareTo("clear") == 0) {
        turnOffAll();  // Turn off all Pixels.
      } else if (incomingSerialData.compareTo("test") == 0) {
        testPixels();
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

// Gives a preview of the lighting sequence.
void showSequence(int n, int brightnessPixel) {
  const int BRIGHTNESS_LEVEL = 31;
  const int DELAY_TIME = 300;
  Serial.println("<SequencePreviewStart>");
  // Turn off the Pixels of the sequence.
  for (int i = 0; i < n; i++) {
    delay(DELAY_TIME);
    colorWipe(sequenceStack[i], 0);
  }
  for (int i = 0; i < n; i++) {
    delay(DELAY_TIME);
    colorWipe(sequenceStack[i], BRIGHTNESS_LEVEL);
  }
  delay(DELAY_TIME * 4);
  for (int i = 0; i < n; i++) {
    delay(DELAY_TIME);
    colorWipe(sequenceStack[i], 0);
  }
  Serial.println("<SequencePreviewEnd>");
  colorWipe(n, brightnessPixel);
}

// Prints the lighting sequence set by the user.
int setLightingSequence() {
  const int BRIGHTNESS_LEVEL = 31;
  const int JUMP_STEPS = 5;
  String incomingData;
  int prevPixel;
  Serial.println("<StripSetSequence>");
  initSequenceStack();
  int currentPixel = 0;
  int stackPtr = 0;
  turnOffAll();
  colorWipe(currentPixel, BRIGHTNESS_LEVEL);  // Turn on the first Pixel.
  while (true) {
    if (Serial.available() > 0) {
      incomingData = Serial.readString();
      incomingData.trim();
      incomingData.toLowerCase();
      if (incomingData.compareTo("f") == 0 && currentPixel < NUM_OF_PIXELS - 1) {
        Serial.println("<StripForward>");
        prevPixel = currentPixel;
        currentPixel++;  // Go to the next Pixel of the strip.
        colorWipe(prevPixel, 0);  // Turn off the previous Pixel.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL);  // Turn on the current Pixel.
      } else if (incomingData.compareTo("jf") == 0) {
        Serial.println("<StripJumpForward-" + String(JUMP_STEPS) + '>');
        prevPixel = currentPixel;
        currentPixel = currentPixel + JUMP_STEPS;
        if (currentPixel >= NUM_OF_PIXELS - 1) {
          // Go to the last Pixel of the strip.
          currentPixel = NUM_OF_PIXELS - 1;
        }
        colorWipe(prevPixel, 0);  // Turn off the previous Pixel.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL);  // Turn on the current Pixel.
      } else if (incomingData.compareTo("b") == 0 && currentPixel > 0) {
        Serial.println("<StripBack>");
        prevPixel = currentPixel;
        currentPixel--;  // Go to previous Pixel of the strip.
        colorWipe(prevPixel, 0);  // Turn off the previous Pixel.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL);  // Turn on the current Pixel.
      } else if (incomingData.compareTo("jb") == 0) {
        Serial.println("<StripJumpBack-" + String(JUMP_STEPS) + '>');
        prevPixel = currentPixel;
        currentPixel = currentPixel - JUMP_STEPS;
        if (currentPixel < 0) {
          // Go to the first Pixel of the strip.
          currentPixel = 0;
        }
        colorWipe(prevPixel, 0);  // Turn off the previous Pixel.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL);  // Turn on the current Pixel.
      } else if (incomingData.compareTo("push") == 0) {
        if (currentPixel + 1 != sequenceStack[stackPtr - 1]) {
          Serial.println("<SequencePixelPush-" + String(currentPixel + 1) + ">");
          // Give to the element referenced by stackPtr (int) in the sequenceStack array the value
          // of the position of the current Pixel.
          sequenceStack[stackPtr] = currentPixel + 1;  // The Pixels are numbered from 0 to
                                                       // NUM_OF_PIXELS.
          stackPtr++;
          printLightingSeq(); 
        }
      } else if (incomingData.compareTo("pop") == 0) {
        Serial.println("<SequencePop>");
        // Clear the value of the element referenced by stackPtr (int) in the sequenceStack array.
        stackPtr--;
        sequenceStack[stackPtr] = -1;
        printLightingSeq();
      } else if (incomingData.compareTo("show") == 0) {
        showSequence(stackPtr, BRIGHTNESS_LEVEL);
      } else if (incomingData.compareTo("q") == 0) {
        Serial.println("<SetLightingSequenceQuit>");
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
      if (i > 0) {
        Serial.print(", ");
      }
      Serial.print(sequenceStack[i]);
    }
  }
  Serial.println('}');
}

// Turns on a Pixel with the white color or turns if off.
void colorWipe(uint16_t pixel, int brightness) {
  strip.setPixelColor(PIXEL_SEQ[pixel] - 1, 0, 0, 0, brightness);
  strip.show();
  if (brightness > 0) {
    Serial.println("<Arduino-PixelOn-" + String(pixel + 1) + ">");
  } else {
    Serial.println("<Arduino-PixelOff-" + String(pixel + 1) + ">");
  }
}

// Turns off all pixels (NUM_OF_PIXELS).
void turnOffAll() {
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    colorWipe(i, strip.Color(0, 0, 0, 0));
  }
}

// Launches a test routine for all Pixels (NUM_OF_PIXELS).
void testPixels() {
  const int DELAY_TIME = 150;  // In miliseconds.
  const int BRIGHTNESS_LEVEL = 31;
  turnOffAll();
  delay(DELAY_TIME);
  Serial.println("<PixelsTest>");
  // Turn on one Pixel at a time and maintains it.
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    colorWipe(i, BRIGHTNESS_LEVEL);
    delay(DELAY_TIME);
  }
  // Turn off one Pixel at a time.
  for (int i = NUM_OF_PIXELS - 1; i >= 0; i--) {
    colorWipe(i, 0);
    delay(DELAY_TIME);
  }
  for (int k = 0; k < 3; k++) {
    if (k % 2 != 0) {
      // Decrease the brightness level from brightness (int) to 0 of each Pixel.
      for (int i = BRIGHTNESS_LEVEL; i >= 0; i = i - 3) {
        for (int j = NUM_OF_PIXELS - 1; j >= 0; j--) {
          colorWipe(j, i);
        }
      }    
    } else {
      // Increase the brightness level from 0 to brightness (int) of each Pixel.
      for (int i = 0; i < BRIGHTNESS_LEVEL + 1; i = i + 3) {
        for (int j = 0; j < NUM_OF_PIXELS; j++) {
          colorWipe(j, i);
        }
      }
    }
  }
}
