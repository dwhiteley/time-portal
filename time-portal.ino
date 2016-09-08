#include <stdint.h>
#include <gfxfont.h>

/***************************************************
  This is our Bitmap drawing example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "time-portal-effects.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_WR);
//Adafruit_TFTLCD  tft = Adafruit_TFTLCD(TFT_CS, TFT_CD, TFT_WR, TFT_RD, 0);

void setup(void) {
  Serial.begin(9600);

  // Initialize Display
  //tft.begin(0x9341);
  tft.begin();
  //fillTest();
  

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
  } else {
    Serial.println("OK!");
  }
  pinMode(A0, OUTPUT);
  setupScroll();
  setScroll(0);
  //offOn("000000b.raw");
  offOn("bubble.raw");
}

void loop() {
//  offOn("000000b.raw"); delay(2000);
//  flickerTo(LOW);
//  rawDraw("000001b.raw", 0, 0, false);
//  flickerTo(HIGH); 
//  delay(2000);
//  rawDraw("000002b.raw", 0, 0, true);
//  delay(2000);
//  flickerTo(LOW);
//  runStatic(1000);
  static int frame_row = 0;
  setScroll(frame_row);
  frame_row +=320/8;
  if(frame_row >= 320)
    frame_row-=320;
    delay(75);
}
