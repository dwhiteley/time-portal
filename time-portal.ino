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

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library
#include "Adafruit_TFTLCD.h"
#include <SPI.h>
#include <SD.h>

#define VSCROLL_DEF   0x33
#define VSCROLL_START 0x37

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.

//#define TFT_DC 9
//#define TFT_CS 4
#define TFT_CS A3
#define TFT_CD A2
#define TFT_WR A1
#define TFT_RD A0

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_WR);
//Adafruit_TFTLCD  tft = Adafruit_TFTLCD(TFT_CS, TFT_CD, TFT_WR, TFT_RD, 0);

#define SD_CS 10

void fillTest(void);

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

void setupScroll() {
  tft.writecommand(VSCROLL_DEF);
  tft.writedata(0);           // BFA[15:8]
  tft.writedata(0);           // BFA[7:0];
  tft.writedata(320 >> 8);    // VSA[15:8]
  tft.writedata(320 & 0xFF);  // VSA[7:0]
  tft.writedata(0);           // TFA[15:8]
  tft.writedata(0);           // TFA[7:0]
}

void setScroll(uint16_t ptr) {
  tft.writecommand(VSCROLL_START);
  tft.writedata(ptr >> 8);
  tft.writedata(ptr);
}


void flick(uint8_t level) {
  digitalWrite(A0, level);
  delay(100);
  digitalWrite(A0, 1-level);
}

void flickerTo(uint8_t level) {
  flick(level); delay(150);
  flick(level); delay(150);
  flick(level); delay(600);
  flick(level); delay(150);
  digitalWrite(A0, level);
}

#define BUFFPIXEL 30

void rawDraw(const char *filename, uint8_t x, uint16_t y, bool scrolling=false) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[2*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint16_t *convBuf = reinterpret_cast<uint16_t*>(&sdbuffer[0]); // Alias to sdbuffer to allow memory reuse.
  int      w, h, row, col;
  uint32_t pos = 0, startTime = millis();
  bool     first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  bmpFile = SD.open(filename);
  if (bmpFile == 0) {
    Serial.print(F("File not found"));
    return;
  }

  bmpWidth  = 240;
  bmpHeight = 320;
  // BMP rows are padded (if needed) to 4-byte boundary
  rowSize = (bmpWidth * 2 + 3) & ~3;

  // Crop area to be loaded
  w = bmpWidth;
  h = bmpHeight;
  if((x+w-1) >= tft.width())  w = tft.width()  - x;
  if((y+h-1) >= tft.height()) h = tft.height() - y;

  // Set TFT address window to clipped image bounds
  if(scrolling) {
    tft.setAddrWindow(x, y, x+w-1, y+1);
  }
  else {
    tft.setAddrWindow(x, y, x+w-1, y+h-1);
  }

  for (row=0; row<h; row++) { // For each scanline...

    // Seek to start of scan line.  It might seem labor-
    // intensive to be doing this on every line, but this
    // method covers a lot of gritty details like cropping
    // and scanline padding.  Also, the seek only takes
    // place if the file position actually needs to change
    // (avoids a lot of cluster math in SD library).
    pos = row * rowSize;
    if(bmpFile.position() != pos) { // Need seek?
      bmpFile.seek(pos);
    }

    uint8_t pixCnt = 0;
    for (col=w; col>0; col-=pixCnt) {
      pixCnt = (col < BUFFPIXEL)? col : BUFFPIXEL;
      bmpFile.read(sdbuffer, (pixCnt<<1));
      uint8_t bigIdx = 0;
      uint8_t smallIdx = 0;
      //tft.pushColors(convBuf,pixCnt,first);
      tft.pushColors(convBuf,pixCnt);
      first = false;
    } // end pixel
    if (scrolling) {
      setScroll(row);
      tft.setAddrWindow(x, y+row, x+w-1, y+row+1);
    }
  } // end scanline
  if (scrolling) {
    setScroll(0);
  }
  Serial.print(F("Loaded in "));
  Serial.print(millis() - startTime);
  Serial.println(" ms");

  bmpFile.close();
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

void offOn(const char* filename) {
  digitalWrite(A0, LOW);
  rawDraw(filename, 0, 0, false);
  digitalWrite(A0, HIGH);
}

// Performance test using the fillScreen function
void fillTest(void)
{
  uint32_t startTime = millis();
  for(unsigned i = 0; i < 10; i++) {
    tft.fillScreen(ILI9341_BLUE);
    tft.fillScreen(0);
    Serial.print(F("Filled in "));
    Serial.print((millis() - startTime)>>1);
    Serial.println(" ms");
    startTime = millis();
  }
}

void fillRand() {
  tft.setAddrWindow(0, 0, tft.width(), tft.height());
  for(uint16_t y = 0; y < tft.height(); y++) {
    for(uint16_t x = 0; x < tft.width(); x+=16) {
      unsigned r = random(0xFFFFU);
      for(uint8_t i=0; i<16; i++) {
        if(r&1) {
          tft.pushColor(ILI9341_BLACK);
        }
        else {
          tft.pushColor(ILI9341_WHITE);
        }
        r >>= 1;
      }
    }
  }
}

void runStatic(uint16_t frames) {
  digitalWrite(A0, LOW);
  fillRand();
  digitalWrite(A0, HIGH);
  for (uint16_t i=0; i < frames; i++) {
    setScroll(random(319));
    delay(10);
  }
  setScroll(0);
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
