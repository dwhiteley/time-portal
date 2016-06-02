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
  
  //yield();

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
  } else {
    Serial.println("OK!");
  }
  pinMode(A0, OUTPUT);

}

void loop() {

  digitalWrite(A0, LOW);
  bmpDraw("000000b.raw", 0, 0);
  digitalWrite(A0, HIGH);
  delay(2000);
  
  digitalWrite(A0, LOW);
  bmpDraw("000001b.raw", 0, 0);
  digitalWrite(A0, HIGH);
  delay(2000);
  
  digitalWrite(A0, LOW);
  bmpDraw("000002b.raw", 0, 0);
  digitalWrite(A0, HIGH);
  delay(2000);  
}


// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  uint16_t *convBuf = reinterpret_cast<uint16_t*>(&sdbuffer[0]); // Alias to sdbuffer to allow memory reuse.
  uint8_t  buf[2];
  uint16_t color;
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  bool     first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
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
  tft.setAddrWindow(x, y, x+w-1, y+h-1);

  for (row=0; row<h; row++) { // For each scanline...

    // Seek to start of scan line.  It might seem labor-
    // intensive to be doing this on every line, but this
    // method covers a lot of gritty details like cropping
    // and scanline padding.  Also, the seek only takes
    // place if the file position actually needs to change
    // (avoids a lot of cluster math in SD library).
    //pos = (bmpHeight - 1 - row) * rowSize;
    pos = row * rowSize;
    if(bmpFile.position() != pos) { // Need seek?
      bmpFile.seek(pos);
      buffidx = sizeof(sdbuffer); // Force buffer reload
    }

    uint8_t pixCnt = 0;
    for (col=w; col>0; col-=pixCnt) {
      pixCnt = (col < BUFFPIXEL)? col : BUFFPIXEL;
      bmpFile.read(sdbuffer, (pixCnt<<1));
      uint8_t bigIdx = 0;
      uint8_t smallIdx = 0;
//      for(unsigned i = 0; i < (pixCnt<<1); i+=2) {
//        uint8_t temp = sdbuffer[i];
//        sdbuffer[i] = sdbuffer[i+1];
//        sdbuffer[i+1] = temp;
//      }
      //tft.pushColors(convBuf,pixCnt,first);
      tft.pushColors(convBuf,pixCnt);
      first = false;
    } // end pixel
  } // end scanline
  Serial.print(F("Loaded in "));
  Serial.print(millis() - startTime);
  Serial.println(" ms");

  bmpFile.close();
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
