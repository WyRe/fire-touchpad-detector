/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
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
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>



// This is calibration data for the raw touch data to the screen coordinates
//#define TS_MINX 480
//#define TS_MINY 380
//#define TS_MAXX 3650
//#define TS_MAXY 3700
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// Color definitions for 64k color mode
// Bits 0..4 -> Blue 0..4
// Bits 5..10 -> Green 0..5
// Bits 11..15 -> Red 0..4
#define GLCD_CL_BLACK 0x0000
#define GLCD_CL_WHITE 0xFFFF
#define GLCD_CL_GRAY 0x7BEF
#define GLCD_CL_LIGHT_GRAY 0xC618
#define GLCD_CL_GREEN 0x07E0
#define GLCD_CL_LIME 0x87E0
#define GLCD_CL_BLUE 0x001F
#define GLCD_CL_RED 0xF800
#define GLCD_CL_AQUA 0x5D1C
#define GLCD_CL_YELLOW 0xFFE0
#define GLCD_CL_MAGENTA 0xF81F
#define GLCD_CL_CYAN 0x07FF
#define GLCD_CL_DARK_CYAN 0x03EF
#define GLCD_CL_ORANGE 0xFCA0
#define GLCD_CL_PINK 0xF97F
#define GLCD_CL_BROWN 0x8200
#define GLCD_CL_VIOLET 0x9199
#define GLCD_CL_SILVER 0xA510
#define GLCD_CL_GOLD 0xA508
#define GLCD_CL_NAVY 0x000F
#define GLCD_CL_MAROON 0x7800
#define GLCD_CL_PURPLE 0x780F
#define GLCD_CL_OLIVE 0x7BE0

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

int pax=0,
    pay=0;

void setup() {
  Serial.begin(9600);
  Serial.println("ILI9341 and STMPE610 Test!"); 
 
  tft.begin();
  ts.begin();
  
  tft.fillScreen(ILI9341_BLACK);

  
  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(10);
  Serial.print(F("Drag                     \n"));
//  Serial.println(testText(int x, int y));
//  delay(3000);
}


void loop() {
    
    // See if there's any touch data for us
    if (! ts.touched()) {
      return;
    }
    
    // Retrieve a point  
    TS_Point p = ts.getPoint();
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    //Serial.print("\tPressure = "); Serial.println(p.z); 
    Serial.print("\n");
    
    // Scale from ~0->4000 to tft.width using the calibration #'s
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");    
    
    //tft.fillCircle(pax, pay, 5, ILI9341_BLACK);
    //tft.fillCircle(p.x, p.y, 5, ILI9341_WHITE);
    testText(pax,pay,ILI9341_BLACK);
    testText(p.x,p.y,GLCD_CL_OLIVE);
    pax=p.x;
    pay=p.y;

}


unsigned long testText(int x, int y, char col) {
  unsigned long start = micros();
  //tft.setRotation(1);
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(0.5);
  tft.println("Me vale verga");
  return micros() - start;
}
