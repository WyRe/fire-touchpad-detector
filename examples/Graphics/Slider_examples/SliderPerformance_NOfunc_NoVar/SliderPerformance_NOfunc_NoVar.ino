/*
 *  
 *  Not using any function to slider drawing.
 *
 */

#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


// Slider parameters
#define SLID_INIT_POSx 30
#define SLID_INIT_POSy 270
#define SLID_WIDTHx 180
#define SLID_HEIGHTy 15

// Store previous value
String pax = String(0);

void setup() {

  Serial.begin(9600);
  Serial.println(F("Touch Paint!"));
  
  tft.begin();
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  
  Serial.println("Touchscreen started");
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(SLID_INIT_POSx-1, SLID_INIT_POSy-1, SLID_WIDTHx+1, SLID_HEIGHTy+2, ILI9341_WHITE);
}


void loop() { 
  // Wait for a touch
  if (! ts.touched()) {
    return;
  }

  // Retrieve a point  
  TS_Point p = ts.getPoint(); 
  // Scale from ~0->4000 to tft.width using the calibration #'s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());


  if((p.y > (SLID_INIT_POSy-SLID_HEIGHTy*2)) && (p.y < (SLID_INIT_POSy + SLID_HEIGHTy*2))) { 
  if ((p.x > SLID_INIT_POSx) && (p.x < (SLID_INIT_POSx + SLID_WIDTHx))) {
    
    tft.fillRect(SLID_INIT_POSx, SLID_INIT_POSy, p.x, SLID_HEIGHTy, ILI9341_RED);
    tft.fillRect(p.x, SLID_INIT_POSy, SLID_INIT_POSx + SLID_WIDTHx - p.x -1, SLID_HEIGHTy, ILI9341_BLACK);


    tft.fillRect(SLID_INIT_POSx + SLID_WIDTHx, SLID_INIT_POSy, tft.width() - SLID_INIT_POSx - SLID_WIDTHx, SLID_HEIGHTy, ILI9341_BLACK);
    tft.drawRect(SLID_INIT_POSx-1, SLID_INIT_POSy-1, SLID_WIDTHx+1, SLID_HEIGHTy+2, ILI9341_WHITE);

      
    printText(SLID_INIT_POSx + 75, SLID_INIT_POSy - 20, pax, ILI9341_BLACK, 2);
    printText(SLID_INIT_POSx + 75, SLID_INIT_POSy - 20, String(p.x), ILI9341_WHITE, 2);
    pax = String(p.x);
    }
  }
} // void loop() end



unsigned long printText(int x, int y, String strg, int col, int sz) {
  unsigned long start = micros();
  //tft.setRotation(1);
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(sz);
  tft.println(strg);
  return micros() - start;
}
