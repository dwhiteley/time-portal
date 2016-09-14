Introduction
------------
Experimentations with creating a "time portal" display for a back-in-time cave diorama.
Learn more about the diorama and the start of the time portal creation at [here](http://www.eetimes.com/author.asp?section_id=216&doc_id=1328642)

Code runs on the Arduino platform (Nano), and drives a ILI9341 TFT display on an Adafruit 2.8" Color TFT Touchscreen Breakout board from Adafruit Industries

Original code created by Clive "Max" Maxfield, inspired by the example found in the [Adafruit ILI9341 library](https://github.com/adafruit/Adafruit_ILI9341)

This project uses the following Arduino libraries:
* [Adafruit\_ILI9341](https://github.com/dwhiteley/Adafruit_ILI9341) (forked)

The sketch uses 320x240 images in a raw 16-bit color format. The script utils/bmpto565raw.py will convert 24-bit color bitmaps into the needed format. The script expects the following conditons:
* The bitmaps are already pre-sized to 320x240
* The bitsmaps are enumerated XXXXXXa.bmp, where XXXXXX is a number
* The bitmaps are located in the img/originals folder in the project

The script will create XXXXXXb.raw files in the img/raw folder. These files should be loaded into the SD-Card that you insert into the display.

Clean Build of Health
---------------------
1. Make a folder for the Adafruit ILI9341 library at \<Arduino>/libraries/Adafruit\_ILI9341, where \<Arduino> is you base Arduino project directory
2. Clone the latest contents of [Adafruit\_ILI9341](https://github.com/dwhiteley/Adafruit_ILI9341) \<Arduino>/libraries/Adafruit\_ILI9341 ([link to zip](https://github.com/dwhiteley/Adafruit_ILI9341/archive/master.zip))
3. Make a project folder for this sketches code at \<Arduino>/time-portal
4. Clone the contents of the publish branch of this repo into \<Arduino>/time-portal ([link to zip](https://github.com/dwhiteley/time-portal/archive/publish.zip))
5. Open the Arduino IDE and ensure that you have installed the latest Adafruit GFX library (required for the Adafruit ILI9341 library) via the Library Manager.
6. Verify the sketch
