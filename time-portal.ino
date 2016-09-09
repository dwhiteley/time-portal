#include <stdint.h>
#include <gfxfont.h>
#include <stdio.h>

/***************************************************
  Time portal sketch.
  Flickers between mirror image and a random image.
  Based on the bitmap drawing example written by Limor Fried/Ladyada for
  Adafruit Industries.
 ****************************************************/

#include "time-portal-effects.h"
#include "fade_effect.h"
#define NUM_IMAGES 84
#define FRAME_TIME 75
#define BACKLIGHT 3

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_WR);
//Adafruit_TFTLCD  tft = Adafruit_TFTLCD(TFT_CS, TFT_CD, TFT_WR, TFT_RD, 0);
Fade fade(BACKLIGHT);
bool on_image;

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
  pinMode(BACKLIGHT, OUTPUT);
  setupScroll();
  setScroll(0);
  on_image = false;
  fade.start();
  rawDraw("000000b.raw", 0, 0);
  //offOn("bubble.raw");
}

void loop() {
    if (fade.is_done()) {
        fade.start();
        if (on_image) {
            on_image = false;
            rawDraw("000000b.raw", 0, 0);
        } else {
            char image_name[12]; 
            on_image = true;
            sprintf(&image_name[0], "%06db.raw", rand() % NUM_IMAGES + 1);
            rawDraw(&image_name[0], 0, 0);
        }
    } else {
        fade.step();
        delay(FRAME_TIME);
    }
  //flickerTo(HIGH); delay(2000); flickerTo(LOW);
  //rawDraw("000002b.raw", 0, 0, true);
  //delay(2000);
  //flickerTo(LOW);
  //runStatic(1000);
  //static int frame_row = 0;
  //setScroll(frame_row);
  //frame_row +=320/8;
  //if(frame_row >= 320)
  //  frame_row-=320;
  //delay(75);
}

