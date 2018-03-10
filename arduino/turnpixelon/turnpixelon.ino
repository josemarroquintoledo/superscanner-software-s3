/*
  Turning a NeoPixel's Pixel On.

  This sketch is used to turn Pixels on or off, test them or set a custom lighting sequence of
  NeoPixel [1] strips, rings or jewel.

  SuperScanner project (GPL-2.0) [2] uses this sketch to light up a sample under a microscope
  (microscopx) with one NeoPixel at a time according to the computational imaging technique
  Fourier Ptychography.

  Created on February 2nd, 2018
  by Jose David Marroquin Toledo

  Modified on March 10th, 2018
  by Jose David Marroquin Toledo
  
  [1] https://github.com/adafruit/Adafruit_NeoPixel

  [2] https://github.com/josemarroquintoledo/superscanner-software-s3
  
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define MAX_NUM_OF_PIXELS 47

struct pixel {
  int number = -1;
  int brightness = 0;
  String color = "w";
};

// From the strandtest example sketch (LGPL-3.0) by Adafruit
//
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_NUM_OF_PIXELS, PIN, NEO_RGBW + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
//

// Custom lighting sequence obtained with setLightingSequence() (int).
const int PIXEL_SEQ[MAX_NUM_OF_PIXELS] =  {1, 2, 3, 4, 5, 6, 7, 18, 17, 16, 15, 14, 13, 12, 11, 10,
                                           9, 8, 23, 22, 21, 20, 19, 46, 47, 24, 25, 26, 27, 28,
                                           29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
                                           43, 44, 45};

pixel lastPixel;
String incomingSerialData;
int firstCommaIdx;  // Index number of the comma (',') in the main loop.
int secondCommaIdx;
bool useCustomSeq = false; 
//
int sequenceStack[MAX_NUM_OF_PIXELS];

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
    incomingSerialData.toLowerCase();
    firstCommaIdx = incomingSerialData.indexOf(',');
    // The second comma is at least two characters later.
    secondCommaIdx = incomingSerialData.indexOf(',', firstCommaIdx + 2);
    if (firstCommaIdx != -1 && secondCommaIdx != -1) {
      int pixel = incomingSerialData.substring(0, firstCommaIdx + 1).toInt();
      int brt = incomingSerialData.substring(firstCommaIdx + 1, secondCommaIdx).toInt();
      String color = incomingSerialData.substring(secondCommaIdx + 1);
      if (pixel > 0 && pixel <= MAX_NUM_OF_PIXELS && brt >= 0 && brt <= 255
          && (color.compareTo("r") == 0 || color.compareTo("g") == 0
              || color.compareTo("b") == 0 || color.compareTo("w") == 0)) {
        // The first Pixel is 1 not 0.
        lastPixel.number = pixel;
        lastPixel.color = color;
        if (useCustomSeq) {
          // Turn a Pixel on or off using the custom lighting sequence (const int PIXEL_SEQ[]).
          colorWipe(PIXEL_SEQ[lastPixel.number - 1], brt, lastPixel.color);
        } else {
          // Turn a Pixel on or off using the real lighting sequence.
          colorWipe(lastPixel.number, brt, lastPixel.color);
        }
      }
    } else {
      if (incomingSerialData.compareTo("useseq") == 0 && !useCustomSeq) {
        Serial.println("<CustomSeqUse>");
        useCustomSeq = true;
      } if (incomingSerialData.compareTo("noseq") == 0 && useCustomSeq) {
        Serial.println("<CustomSeqNo>");
        useCustomSeq = false;
      } else if (incomingSerialData.compareTo("clear") == 0) {
        turnAllOff();  // Turn all Pixels off.
      } else if (incomingSerialData.compareTo("test") == 0) {
        // It uses the custom lighting sequence PIXEL_SEQ[] (const int).
        if (!useCustomSeq) {
          Serial.println("<CustomSeqUse>");
          useCustomSeq = true;
        }
        testPixels();
      } else if (incomingSerialData.compareTo("setseq") == 0) {
        if (useCustomSeq) {
          Serial.println("<CustomSeqNo>");
          useCustomSeq = false;
        }
        setLightingSequence(); 
      }
    }
  }
}

void initSequenceStack() {
  for (int i = 0; i < MAX_NUM_OF_PIXELS; i++) {
    sequenceStack[i] = -1;
  }  
}

// Gives a preview of the custome lighting sequence.
void showSequence(int currentPixelIdx, String strColor) {
  const int BRIGHTNESS_LEVEL = 31;
  const int DELAY_TIME = 300;
  Serial.println("<SequencePreviewStart>");
  for (int i = 0; i <= currentPixelIdx; i++) {
    delay(DELAY_TIME);
    colorWipe(sequenceStack[i], BRIGHTNESS_LEVEL, strColor);
  }
  delay(DELAY_TIME * 4);
  // Turn the Pixel off 
  for (int i = 0; i <= currentPixelIdx; i++) {
    colorWipe(sequenceStack[i], 0, strColor);
  }
  Serial.println("<SequencePreviewEnd>");
  printLightingSeq();
}

// Creates and print a custom lighting sequence that can be used to replace the value of PIXEL_SEQ
// (const int[]).
//
// After the calling of the function, can be written in the serial port: f ("forward") to turn the
// next Pixel on and off the current one, b ("backward") to turn the current one off and turn the
// previous one on, jf ("jump forward") to go JUMP_STEPS (int) steps forward, jb ("jump backward")
// to go JUMP_STEPS (int) steps backward, push to add a item (Pixel's index in the real sequence)
// to end of the list (int sequenceStack[]), pop to remove it and q ("quit") to finish the
// function's execution.
int setLightingSequence() {
  const int BRIGHTNESS_LEVEL = 31;
  const int JUMP_STEPS = 5;
  const String COLOR = "r";
  
  int currentPixel = 1;
  int stackPtr = -1;
  String incomingData;
  int prevPixel;
  bool inSeq;
  Serial.println("<StripSetSequence>");
  initSequenceStack();
  turnAllOff();
  colorWipe(currentPixel, BRIGHTNESS_LEVEL, COLOR);  // Turn the first Pixel on.
  while (true) {
    if (Serial.available() > 0) {
      incomingData = Serial.readString();
      incomingData.trim();
      incomingData.toLowerCase();
      if (incomingData.compareTo("f") == 0 && currentPixel < MAX_NUM_OF_PIXELS) {
        Serial.println("<StripForward>");
        prevPixel = currentPixel;
        currentPixel++;  // Go to the next Pixel in the strip.
        colorWipe(prevPixel, 0, COLOR);  // Turn the previous Pixel off.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL, COLOR);  // Turn the current Pixel on.
      } else if (incomingData.compareTo("jf") == 0) {
        Serial.println("<StripJumpForward-" + String(JUMP_STEPS) + '>');
        prevPixel = currentPixel;
        currentPixel = currentPixel + JUMP_STEPS;
        if (currentPixel >= MAX_NUM_OF_PIXELS - 1) {
          // Go to the last Pixel in the strip.
          currentPixel = MAX_NUM_OF_PIXELS - 1;
        }
        colorWipe(prevPixel, 0, COLOR);  // Turn the previous Pixel off.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL, COLOR);  // Turn the current Pixel on.
      } else if (incomingData.compareTo("b") == 0 && currentPixel > 1) {
        Serial.println("<StripBack>");
        prevPixel = currentPixel;
        currentPixel--;  // Go to previous Pixel in the strip.
        colorWipe(prevPixel, 0, COLOR);  // Turn the previous Pixel off.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL, COLOR);  // Turn the current Pixel on.
      } else if (incomingData.compareTo("jb") == 0) {
        Serial.println("<StripJumpBack-" + String(JUMP_STEPS) + '>');
        prevPixel = currentPixel;
        currentPixel = currentPixel - JUMP_STEPS;
        if (currentPixel < 1) {
          // Go to the first Pixel in the strip.
          currentPixel = 1;
        }
        colorWipe(prevPixel, 0, COLOR);  // Turn the previous Pixel off.
        colorWipe(currentPixel, BRIGHTNESS_LEVEL, COLOR);  // Turn the current Pixel on.
      } else if (incomingData.compareTo("push") == 0) {
        inSeq = false;
        // Look for the current Pixel in the sequence to add it only if it does not exist.
        for (int i = 0; i <= stackPtr; i++) {
          if (sequenceStack[i] == currentPixel) {
            inSeq = true;
            break;
          }
        }
        if (!inSeq) {
          Serial.println("<SequencePixelPush-" + String(currentPixel) + ">");
          stackPtr++;
          sequenceStack[stackPtr] = currentPixel;
          printLightingSeq(); 
        }          
      } else if (incomingData.compareTo("pop") == 0 && stackPtr > -1) {
        Serial.println("<SequencePop>");
        // Clear the value of the element referenced by stackPtr (int) in the sequenceStack array.
        sequenceStack[stackPtr] = -1;
        stackPtr--;
        printLightingSeq();
      } else if (incomingData.compareTo("show") == 0) {
        colorWipe(currentPixel, 0, COLOR);  // Turn the current Pixel off.
        showSequence(stackPtr, COLOR);
        colorWipe(currentPixel, BRIGHTNESS_LEVEL, COLOR);  // Turn the current Pixel on again.
      } else if (incomingData.compareTo("print") == 0) {
        printLightingSeq();
      } else if (incomingData.compareTo("q") == 0) {
        Serial.println("<StripSetSequenceQuit>");
        printLightingSeq();
        turnAllOff();
        return 0;
      }
    }
  }
}

// Prints the custom lighting sequence.
void printLightingSeq() {
  Serial.print('{');
  for (int i = 0; i < MAX_NUM_OF_PIXELS; i++) {
    if (sequenceStack[i] != -1) {
      if (i > 0) {
        Serial.print(", ");
      }
      Serial.print(sequenceStack[i]);
    }
  }
  Serial.println('}');
}

// Sets a Pixel to a red, green, blue or white color. If brightness (int) is 0, the Pixel will be
// turned off.
//
// pixel (uint16_t) is the Pixel's index in the strip from 1 to MAX_NUM_OF_PIXELS.
void colorWipe(uint16_t pixel, int brightness, String strColor) {
  uint32_t uintColor;
  Serial.print("<Arduino-Pixel");
  if (brightness > 0) {
    if (strColor.compareTo("r") == 0) {
      Serial.print("RedOn-");
      uintColor = strip.Color(0, brightness, 0, 0);
    } else if (strColor.compareTo("g") == 0) {
      Serial.print("GreenOn-");
      uintColor = strip.Color(brightness, 0, 0, 0);
    } else if (strColor.compareTo("b") == 0) {
      Serial.print("BlueOn-");
      uintColor = strip.Color(0, 0, brightness, 0);
    } else if (strColor.compareTo("w") == 0) {
      Serial.print("WhiteOn-");
      uintColor = strip.Color(0, 0, 0, brightness);
    }
    strip.setPixelColor(pixel - 1, uintColor);
  } else {
    Serial.print("Off-");
    strip.setPixelColor(pixel - 1, 0);
  }
  Serial.println(String(pixel) + ">");
  strip.show();
}

// Turns all pixels (MAX_NUM_OF_PIXELS) off.
void turnAllOff() {
  Serial.println("<StripTurnOff>");
  for (int i = 1; i <= MAX_NUM_OF_PIXELS; i++) {
    colorWipe(i, 0, "w");  // It does not matter the color ("r", "g", "b" or "w").
  }
  Serial.println("<StripTurnedOff>");
}

// Launches a test routine for all Pixels (MAX_NUM_OF_PIXELS).
void testPixels() {
  const int DELAY_TIME = 150;  // In miliseconds.
  const int BRIGHTNESS_LEVEL = 31;
  const String COLOR = "r";
  
  Serial.println("<StripTestStart>");
  turnAllOff();
  delay(DELAY_TIME);
  // Turn one Pixel on at a time and maintains it.
  for (int i = 0; i < MAX_NUM_OF_PIXELS; i++) {
    colorWipe(PIXEL_SEQ[i], BRIGHTNESS_LEVEL, COLOR);
    delay(DELAY_TIME);
  }
  // Turn one Pixel off at a time.
  for (int i = MAX_NUM_OF_PIXELS - 1; i >= 0; i--) {
    colorWipe(PIXEL_SEQ[i], 0, COLOR);
    delay(DELAY_TIME);
  }
  for (int k = 0; k < 3; k++) {
    if (k % 2 != 0) {
      // Decrease the brightness level from brightness (int) to 0 of each Pixel.
      for (int i = BRIGHTNESS_LEVEL; i >= 0; i = i - 3) {
        for (int j = MAX_NUM_OF_PIXELS - 1; j >= 0; j--) {
          colorWipe(PIXEL_SEQ[j], i, COLOR);
        }
      }    
    } else {
      // Increase the brightness level from 0 to brightness (int) of each Pixel.
      for (int i = 0; i < BRIGHTNESS_LEVEL + 1; i = i + 3) {
        for (int j = 0; j < MAX_NUM_OF_PIXELS; j++) {
          colorWipe(PIXEL_SEQ[j], i, COLOR);
        }
      }
    }
  }
  Serial.println("<StripTestEnd>");
}
