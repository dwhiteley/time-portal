#include <stdint.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library
#include "Adafruit_TFTLCD.h"

#define VSCROLL_DEF   0x33
#define VSCROLL_START 0x37
#define BUFFPIXEL 30

#define TFT_CS A3
#define TFT_CD A2
#define TFT_WR A1
#define TFT_RD A0

#define SD_CS 10
#define LIGHT 3

#define WAIT_HIGH 30000UL
#define WAIT_LOW  10000UL


void fillTest(void);
void setupScroll(void);
void setScroll(uint16_t ptr);
void flick(uint8_t level);
void flickerTo(uint8_t level);
void rawDraw(const char *filename, uint8_t x, uint16_t y, bool scrolling=false);
void rand_wait(void);
uint8_t get_option(void);
void offOn(const char* filename);
void fillTest(void);
void fillRand();
void runStatic(uint16_t frames);
uint16_t read16(File &f);
uint32_t read32(File &f);

