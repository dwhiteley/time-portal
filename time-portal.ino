////////////////////////////////////////////////////////////////////////////////
// Sketch for cave diorama's time portal.
// Transitions between mirror image and a set of random images.
// Transition consists of a flicker, similiar to a glitchy screen
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>           // Standard integer set
#include <stdio.h>            // Standard I/O (for sprintf)
#include <SD.h>               // Arduino SD Card library
#include <SPI.h>              // Arduino SPI library
#include "Adafruit_ILI9341.h" // Adafruit's SPI based display driver library

////////////////////////////////////////////////////////////////////////////////
// Pin assignments
////////////////////////////////////////////////////////////////////////////////
#define TFT_CS A3             // Chip select for display
#define TFT_CD A2             // Command/Data discrete for display (not used in SPI mode)
#define TFT_WR A1             // Write discrete for display
#define TFT_RD A0             // Read discrete for display (not used)

#define SD_CS 10              // Chip select for SD Card
#define LIGHT 3               // PWM for display backlight
#define OP2   6               // Option select pin 2
#define OP1   5               // Option select pin 1
#define OP0   4               // Option select pin 0

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////
#define WAIT_HIGH 30000UL     // Max picture display time in ms
#define WAIT_LOW  10000UL     // Min picture display time in ms

#define VSCROLL_DEF   0x33    // Command code for setting up vertical scroll
#define VSCROLL_START 0x37    // Command code for setting vertical scroll
#define BUFFPIXEL 30          // Number of pixels read/written in one iteration
#define NUM_IMAGES 84         // Number of non-mirror images on SD Card

// Define possible effect states
enum {
    SCREEN_OFF,   // Display off
    MIRROR_ONLY,  // Displays only the mirror
    MIRROR_IMAGE, // Alternates between mirror and random image
    IMAGE_ONLY,   // Only random images
    LAST_OPTION   // Not an option. Used for determining option state validity
} ScreenOptions;

////////////////////////////////////////////////////////////////////////////////
// Prototypes for helper functions
////////////////////////////////////////////////////////////////////////////////
void setupScroll(void);
void setScroll(uint16_t row);
void load_mirror_image(void);
void load_random_image(void);
void rawDraw(const char *filename, bool scrolling=false);
unsigned long uniform(unsigned long low, unsigned long high);
void rand_wait(void);
uint8_t get_option(void);
void flick(uint8_t level);
void flickerTo(uint8_t level);
void fadeTo(uint8_t level);
void fadeOn(void);
void fadeOff(void);

////////////////////////////////////////////////////////////////////////////////
// Global Variables/Objects
////////////////////////////////////////////////////////////////////////////////
// Display Driver object (uses HW SPI)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_WR);
// Tracks if mirror image is in display memory
bool on_mirror = false;

////////////////////////////////////////////////////////////////////////////////
// Arduino initial setup
////////////////////////////////////////////////////////////////////////////////
void setup(void) {
  Serial.begin(9600);

  // Initialize Display
  tft.begin();

  // Initialize SD Card file system (with debug prints)
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
  } else {
    Serial.println("OK!");
  }

  // Turn off backlight
  pinMode(LIGHT, OUTPUT);
  digitalWrite(LIGHT, LOW);

  // Initialize option select pins
  pinMode(OP2, INPUT_PULLUP);
  pinMode(OP1, INPUT_PULLUP);
  pinMode(OP0, INPUT_PULLUP);

  // Setup display scrolling
  setupScroll();
  setScroll(0);
}

////////////////////////////////////////////////////////////////////////////////
// Arduino process loop
////////////////////////////////////////////////////////////////////////////////
void loop() {
    uint8_t option = get_option(); // Grab state on option select pins

    if(LAST_OPTION <= option) { // If option isn't valid
        option = SCREEN_OFF;    // Set option select to turn off screen
    }
    Serial.println(option); // DEBUG : display current option state

    if(SCREEN_OFF == option) { // If in screen off state
        // Turn of screen if needed (i.e. MIRROR_ONLY -> SCREEN_OFF)
        if(HIGH == digitalRead(LIGHT)) {
            transitionTo(LOW); 
        }
        delay(2000); // Hold screen off
    }
    else { // option = MIRROR_ONLY or MIRROR_IMAGE or IMAGE_ONLY
        // Display image(s)
        if(IMAGE_ONLY != option) { // If option = MIRROR_ONLY or MIRROR_IMAGE
            // Mirror image should be on display
            if(!on_mirror) { // If mirror image not in display memory
                load_mirror_image(); // Load the mirror image (assuming backlight is off)
            }
            // Turn on backlight if needed (not needed if last state was MIRROR_ONLY)
            if(LOW == digitalRead(LIGHT)) {
                transitionTo(HIGH);
            }
            rand_wait();     // Hold mirror image
        }

        if(MIRROR_ONLY != option) { // If option = MIRROR_IMAGE or IMAGE_ONLY
            // Display random image (next)
            if(HIGH == digitalRead(LIGHT)) {
                transitionTo(LOW); // Turn off image in prep of loading
            }
            load_random_image(); // Load a random image
            transitionTo(HIGH);  // Turn backlight on
            rand_wait();         // Hold random image
            transitionTo(LOW);   // Turn backlight off (in preparation of next loop iteration)
        }
    }
    
}

////////////////////////////////////////////////////////////////////////////////
// Helper Functions
////////////////////////////////////////////////////////////////////////////////

// Load mirror image to display memory
// Note: Assumes backlight off during this operation
void load_mirror_image(void) {
     rawDraw("000000b.raw", false); // Filename of mirror image
     on_mirror = true;              // Indicate the mirror image in display memory
}

// Picks and loads a random image. Assumptions:
// * Backlight is off
// * Random images are of the form XXXXXXb.raw, where XXXXXX is a zero padded integer
// * There are no gaps in file numbering
// * Random image indexing starts at 000001
// * NUM_IMAGES is the index of the last random image
void load_random_image(void) {
    char image_name[12];                         // String variable for random image filename
    unsigned index = rand() % NUM_IMAGES + 1;    // Pick a number between 1 and NUM_IMAGES
    sprintf(&image_name[0], "%06db.raw", index); // Generate filename from image index
    rawDraw(&image_name[0], false);              // Load image into memory
    on_mirror = false;                           // Indicate random image in display memory
}

// Loads a raw image file into display memory
// Inspired by main loop function found in Adafruit's bitmap drawing example
// Bitmap drawing example written by Limor Fried/Ladyada for Adafruit Industries. (MIT License)
// Assumptions: 
// *All images are raw data in 16-bit (5-6-5) color
// *All images are 320x240
void rawDraw(const char *filename, bool scrolling) {

    uint8_t  sdbuffer[2*BUFFPIXEL]; // pixel buffer (2 bytes per pixel)
    uint16_t *convBuf = reinterpret_cast<uint16_t*>(&sdbuffer[0]); // Alias to sdbuffer to allow memory reuse.
    uint32_t startTime = millis();  // DEBUG: for finding out image loading time

    Serial.println();
    Serial.print(F("Loading image '"));
    Serial.print(filename);
    Serial.println('\'');

    // Open requested file on SD card
    File bmpFile = SD.open(filename);
    if (bmpFile == 0) {
        Serial.print(F("File not found"));
        return;
    }

    // Crop area to be loaded
    unsigned w = 240;                // Row size in pixels
    unsigned h = 320;                // Image size in rows
    uint32_t rowSize = w * 2; // Row size in bytes

    // Set TFT address window to clipped image bounds
    if(scrolling) {
        tft.setAddrWindow(0, 0, w-1, 1);
    }
    else {
        tft.setAddrWindow(0, 0, w-1, h-1);
    }

    for (unsigned row=0; row<h; row++) { // For each scanline...
        // The seek only takes place if the file position actually needs
        // to change (avoids a lot of cluster math in SD library).
        uint32_t pos = row * rowSize;
        if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
        }

        uint8_t pixCnt;
        for (unsigned col=w; col>0; col-=pixCnt) {
            // Determine how many pixels to grab
            // Tries to grab a BUFFPIXEL size chunk if possible
            // Grabs a smaller chunk if it's at the end of a row
            pixCnt = (col < BUFFPIXEL)? col : BUFFPIXEL;
            // Grab bytes from SD card
            // Number of bytes = number of pixels * 2
            bmpFile.read(sdbuffer, (pixCnt<<1)); 
            tft.pushColors(convBuf,pixCnt); // Push bytes read into display memory
        } // end loading row
        if (scrolling) {
            // Scroll image up one row and setup next row for loading data
            setScroll(row);
            tft.setAddrWindow(0, row, w-1, row+1);
        }
    } // end loading image

    if (scrolling) {
        setScroll(0); // Reset scroll to normal
    }
    // DEBUG: Indicate the amount of time it took to write the whole image
    Serial.print(F("Loaded in "));
    Serial.print(millis() - startTime);
    Serial.println(" ms");

    bmpFile.close(); // Close raw image file
}

// Return a random integer in range [low, high)
unsigned long uniform(unsigned long low, unsigned long high) {
    return (rand() % (high-low)) + low;
}

// Wait a random amount of time.
// Lower and upper bounds defined by WAIT_LOw and WAIT_HIGH respectively
void rand_wait(void) {
    //delay(2000); //Small delay version for debugging
    delay(uniform(WAIT_LOW, WAIT_HIGH));
}

// Get option state from option state pins
// Assumes bit order 2:0
uint8_t get_option(void) {
    return (digitalRead(OP2) << 2)|(digitalRead(OP1) << 1)|digitalRead(OP0);
}


////////////////////////////////////////////////////////////////////////////////
// Display scrolling
////////////////////////////////////////////////////////////////////////////////

// Initializes display to allow for vertical scrolling
void setupScroll() {
    tft.writecommand(VSCROLL_DEF);
    tft.writedata(0);           // BFA[15:8]
    tft.writedata(0);           // BFA[7:0];
    tft.writedata(320 >> 8);    // VSA[15:8]
    tft.writedata(320 & 0xFF);  // VSA[7:0]
    tft.writedata(0);           // TFA[15:8]
    tft.writedata(0);           // TFA[7:0]
}

// Sets the row to make the top row to scroll to
void setScroll(uint16_t row) {
    tft.writecommand(VSCROLL_START);
    tft.writedata(row >> 8);
    tft.writedata(row);
}

// A shim function which allows to choose between flicker or fade transitions
void transitionTo(uint8_t level) {
    flickerTo(level);
    //fadeTo(level);
}

////////////////////////////////////////////////////////////////////////////////
// Flicker effect functions
////////////////////////////////////////////////////////////////////////////////

// Toggles backlight between off/on or on/off (depending on level)
void flick(uint8_t level) {
    digitalWrite(LIGHT, level);
    delay(75);
    digitalWrite(LIGHT, 1-level);
}

// Transition to backlight state indicated by level.
// Transition appears as flickering to that state.
void flickerTo(uint8_t level) {
    // Flicker four times, waiting in between flickers
    flick(level); delay(uniform(75 ,300));
    flick(level); delay(uniform(150,400));
    flick(level); delay(uniform(300,500));
    flick(level); delay(uniform(75 ,150));
    digitalWrite(LIGHT, level);
}

////////////////////////////////////////////////////////////////////////////////
// Fade effect functions
////////////////////////////////////////////////////////////////////////////////
void fadeTo(uint8_t level) {
    if (LOW == level) {
        fadeOff();
    }
    else {
        fadeOn();
    }
}

void fadeOff(void) {
    static const int8_t inc = 16;

    for (uint8_t val = 255; val >= inc; val-=inc) {
        analogWrite(LIGHT, val);
        delay(75);
    }
    analogWrite(LIGHT, 0);
}

void fadeOn(void) {
    static const int8_t inc = 16;

    for (uint8_t val = 0; (255-val) >= inc; val+=inc) {
        analogWrite(LIGHT, val);
        delay(75);
    }
    analogWrite(LIGHT, 255);
}
