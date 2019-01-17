/*
 *  
 *  Using a specific function sliderDraw() to slider drawing.
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
  // Debuggin point data
  Serial.print("("); Serial.print(p.x);
  Serial.print(", "); Serial.print(p.y);
  Serial.println(")");

  sliderDraw(SLID_INIT_POSx, SLID_WIDTHx, SLID_INIT_POSy, SLID_HEIGHTy, p.x, p.y);
  Serial.println("Outside function");

} // void loop() end

unsigned long sliderDraw(int slid_posx, int slid_w, int slid_posy, int slid_h, int ptx, int pty) {

    //Serial.println("Inside function, before if");
    if ((pty > (slid_posy-slid_h*2)) && (pty < (slid_posy + slid_h*2))) { 
    if ((ptx > slid_posx) && (ptx < (slid_posx + slid_w))) {
      // Fill left side with RED 
      tft.fillRect(slid_posx, slid_posy, ptx, slid_h, ILI9341_RED);
      // Fill right side with BLACK
      tft.fillRect(ptx, slid_posy, slid_posx + slid_w - ptx -1, slid_h, ILI9341_BLACK);
      // Fixing extra right space after slider
      tft.fillRect(slid_posx + slid_w, slid_posy, tft.width() - slid_posx - slid_w, slid_h, ILI9341_BLACK);
      // Fixing border
      tft.drawRect(slid_posx-1, slid_posy-1, slid_w+1, slid_h+2, ILI9341_WHITE);
      }
    }
    //Serial.println("Inside function, after if");
}
