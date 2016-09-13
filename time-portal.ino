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
#define NUM_IMAGES 84

enum {
    SCREEN_OFF,
    MIRROR_ONLY,
    MIRROR_IMAGE,
    IMAGE_ONLY,
    LAST_OPTION
} ScreenOptions;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_WR);
//Adafruit_TFTLCD  tft = Adafruit_TFTLCD(TFT_CS, TFT_CD, TFT_WR, TFT_RD, 0);
bool on_mirror = false;

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
  pinMode(LIGHT, OUTPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  setupScroll();
  setScroll(0);
  digitalWrite(LIGHT, LOW);
  //offOn("bubble.raw");
}

void loop() {
    char image_name[12]; 
    uint8_t option = get_option();
    if(LAST_OPTION <= option) {
        option = SCREEN_OFF;
    }
    Serial.println(option);

    if(SCREEN_OFF == option) {
        if(HIGH == digitalRead(LIGHT)) {
            flickerTo(LOW); 
        }
    }
    else {
        if(IMAGE_ONLY != option){
            if(!on_mirror) {
                rawDraw("000000b.raw", 0, 0, false);
                on_mirror = true;
            }
            if(LOW == digitalRead(LIGHT)) {
                flickerTo(HIGH); 
            }
            rand_wait(); 
        }
        if(MIRROR_ONLY != option) {
            if(HIGH == digitalRead(LIGHT)) {
                flickerTo(LOW); 
            }
            sprintf(&image_name[0], "%06db.raw", rand() % NUM_IMAGES + 1);
            rawDraw(&image_name[0], 0, 0, false);
            flickerTo(HIGH); rand_wait(); flickerTo(LOW);
            on_mirror = false;
        }
    }
  //rawDraw("000002b.raw", 0, 0, true);
  //delay(2000);
  //flickerTo(LOW);
  //runStatic(1000);
  //static int frame_row = 0;
  //setScroll(frame_row);
  //frame_row +=320/8;
  //if(frame_row >= 320)
  //  frame_row-=320;
  //  delay(75);
}
