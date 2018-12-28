/***************************************************
  Comienzo fase diseño display/touchpad.
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
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

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40            // both buttons have the same height and width, using this parameter

// Buttons and Text Positions
#define TEMP_BUTT_POSx 50
#define TEMP_BUTT_POSy 18
#define TEMP_BUTT_COL ILI9341_WHITE
#define TEMP_BUTT_SIZ 0.5
#define GAS_BUTT_POSx 170     // 20*8.5 also is compiled
#define GAS_BUTT_POSy 18
#define GAS_BUTT_COL ILI9341_WHITE
#define GAS_BUTT_SIZ 0.5

#define TEMP_MEAS_POSx 60
#define TEMP_MEAS_POSy 150
#define TEMP_MEAS_COL ILI9341_RED
#define TEMP_MEAS_SIZ 2
#define GAS_MEAS_POSx 100
#define GAS_MEAS_POSy 150
#define GAS_MEAS_COL ILI9341_GREEN
#define GAS_MEAS_SIZ 2

// Slider parameters
#define SLID_INIT_POSx 30
#define SLID_INIT_POSy 270
#define SLID_WIDTHx 180
#define SLID_HEIGHTy 15


int oldmeasure, currmeasure;            // ----- temperature:1 ----- gas:2 -----

// Right limiter for slider.
int lim_posx = SLID_INIT_POSx + SLID_WIDTHx;
int lim_posy = SLID_INIT_POSy;
int lim_wid = tft.width() - SLID_INIT_POSx - SLID_WIDTHx;
int lim_h = SLID_HEIGHTy;

// Selected value.
String pax = String(0);


void setup(void) {

  Serial.begin(9600);
  Serial.println(F("Touch Paint!"));

  tft.begin();
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");

  // tft.setRotation(2); // Check before how to rotate touchscreen

  tft.fillScreen(ILI9341_BLACK);

  // make the measure selection boxes                             // RETOCAR
  printText(TEMP_BUTT_POSx, TEMP_BUTT_POSy, "Temp", TEMP_BUTT_COL, TEMP_BUTT_SIZ);
  printText(GAS_BUTT_POSx, GAS_BUTT_POSy, "Gas", GAS_BUTT_COL, GAS_BUTT_SIZ);

  // select the current measure 'Gas'; modify currmeasure value to setup a default measure to show.
  currmeasure = 2;
  if (currmeasure == 1) {
    tft.drawRect(0, 0, BOXSIZE*3, BOXSIZE, ILI9341_WHITE);
    printText(TEMP_MEAS_POSx, TEMP_MEAS_POSy, "Temperature", TEMP_MEAS_COL, TEMP_MEAS_SIZ);
  } else if (currmeasure == 2) {
      tft.drawRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE, ILI9341_WHITE);
      printText(GAS_MEAS_POSx, GAS_MEAS_POSy, "GAS", GAS_MEAS_COL, GAS_MEAS_SIZ);
  }

  tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);
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

  // Choosing colour
  if (p.y < BOXSIZE && p.y > 0) {
    oldmeasure = currmeasure;
    if (p.x < BOXSIZE*3) {
      currmeasure = 1;
      printText(GAS_MEAS_POSx, GAS_MEAS_POSy, "GAS", ILI9341_BLACK, GAS_MEAS_SIZ);
      tft.drawRect(0, 0, BOXSIZE*3, BOXSIZE, ILI9341_WHITE);
      Serial.println("TEMP");
      printText(TEMP_MEAS_POSx, TEMP_MEAS_POSy, "Temperature", TEMP_MEAS_COL, TEMP_MEAS_SIZ);
    } else if (p.x < BOXSIZE*6) {
      currmeasure = 2;
      printText(TEMP_MEAS_POSx, TEMP_MEAS_POSy, "Temperature", ILI9341_BLACK, TEMP_MEAS_SIZ);
      tft.drawRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE, ILI9341_WHITE);
      Serial.println("GAS");
      printText(GAS_MEAS_POSx, GAS_MEAS_POSy, "GAS", GAS_MEAS_COL, GAS_MEAS_SIZ);
    }

    if (oldmeasure != currmeasure) {
      if (oldmeasure == 1)
        tft.drawRect(0, 0, BOXSIZE*3, BOXSIZE, ILI9341_BLACK);
      if (oldmeasure == 2)
        tft.drawRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE, ILI9341_BLACK);
    }
  }

  if ((p.y > (SLID_INIT_POSy - SLID_HEIGHTy * 2)) && (p.y < (SLID_INIT_POSy + SLID_HEIGHTy * 2))) {
    if ((p.x > SLID_INIT_POSx) && (p.x < (SLID_INIT_POSx + SLID_WIDTHx))) {
      int corr_wid = SLID_INIT_POSx + SLID_WIDTHx - p.x - 1;


      tft.fillRect(SLID_INIT_POSx, SLID_INIT_POSy, p.x, SLID_HEIGHTy, ILI9341_RED);
      tft.fillRect(p.x, SLID_INIT_POSy, corr_wid, SLID_HEIGHTy, ILI9341_BLACK);


      tft.fillRect(lim_posx, lim_posy, lim_wid, lim_h, ILI9341_BLACK);
      tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);


      int LIM_POSx = SLID_INIT_POSx + 75;
      int LIM_POSy = SLID_INIT_POSy - 20;
      int LIM_SIZ = 2;


      printText(LIM_POSx, LIM_POSy, pax, ILI9341_BLACK, LIM_SIZ);
      printText(LIM_POSx, LIM_POSy, String(p.x), ILI9341_WHITE, LIM_SIZ);
      pax = String(p.x);
      //      sliderLimiter();
    }
  }
} // void loop() end.

//unsigned long sliderLimiter() {
//  int posx = SLID_INIT_POSx+SLID_WIDTHx;
//  int posy = SLID_INIT_POSy;
//  int wid = 240 - SLID_INIT_POSx - SLID_WIDTHx;
//  int h = SLID_HEIGHTy;
//  tft.fillRect(posx, posy, wid, h, ILI9341_BLACK);
//}

unsigned long printText(int x, int y, String strg, int col, int sz) {
  unsigned long start = micros();
  //tft.setRotation(1);
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(sz);
  tft.println(strg);
  return micros() - start;
}
