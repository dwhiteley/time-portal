Experimentations with creating a "time portal" display for a back-in-time cave diorama.
Learn more about the diorama and the start of the time portal creation at [here](http://www.eetimes.com/author.asp?section_id=216&doc_id=1328642)

Code runs on the Arduino platform (Nano), and drives a ILI9341 TFT display on an Adafruit 2.8" Color TFT Touchscreen Breakout board from Adafruit Industries

Original code created by Clive "Max" Maxfield, inspired by the example found in the [Adafruit ILI9341 library](https://github.com/adafruit/Adafruit_ILI9341)

This project uses the following Arduino libraries:
* [Adafruit_ILI9341](https://github.com/dwhiteley/Adafruit_ILI9341) (forked)
* [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)

The sketch uses 320x240 images in a raw 16-bit color format. The script utils/bmpto565raw.py will convert 24-bit color bitmaps into the needed format. The script expects the following conditons:
* The bitmaps are already pre-sized to 320x240
* The bitsmaps are enumerated XXXXXXa.bmp, where XXXXXX is a number
* The bitmaps are located in the img/originals folder in the project

The script will create XXXXXXb.raw files in the img/raw folder. These files should be loaded into the SD-Card that you insert into the display.
