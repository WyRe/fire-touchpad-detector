/***************************************************
  Starting touchpad/display design phase.
 ****************************************************/


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

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40            // both buttons have the same height and width, using this parameter

// Buttons and Text Positions
#define TEMP_BUTT_POSx 50
#define TEMP_BUTT_POSy 18
#define TEMP_BUTT_COL ILI9341_WHITE
#define TEMP_BUTT_SIZ 0.5
#define GAS_BUTT_POSx 170     // 20*8.5 is also compiled
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

  // Starting display (tft), touchscreen (ts) and serial comm. 
  Serial.begin(9600);
  Serial.println(F("Touch Paint!"));
  tft.begin();
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  
  
  // Initial setup for display. (or optionally touchscreen uncommenting)
  // tft.setRotation(2); // Check before how to rotate touchscreen
  tft.fillScreen(ILI9341_BLACK);

  // Displaying text of buttons measure selection boxes                             // Modify defined constants
  printText(TEMP_BUTT_POSx, TEMP_BUTT_POSy, "Temp", TEMP_BUTT_COL, TEMP_BUTT_SIZ);
  printText(GAS_BUTT_POSx, GAS_BUTT_POSy, "Gas", GAS_BUTT_COL, GAS_BUTT_SIZ);

  // Select the current measure 'Gas'; modify currmeasure value to setup a default measure to show
  // in this case it draws a white rectancle around the button
  currmeasure = 2;
  if (currmeasure == 1) {
    tft.drawRect(0, 0, BOXSIZE*3, BOXSIZE, ILI9341_WHITE);
    printText(TEMP_MEAS_POSx, TEMP_MEAS_POSy, "Temperature", TEMP_MEAS_COL, TEMP_MEAS_SIZ);
  } else if (currmeasure == 2) {
      tft.drawRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE, ILI9341_WHITE);
      printText(GAS_MEAS_POSx, GAS_MEAS_POSy, "GAS", GAS_MEAS_COL, GAS_MEAS_SIZ);
  }
  // Drawing the slider border
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

  
  // Choosing a measure. Once we receive the ts data, this condicional check if the user pressed inside button region, 
  // then, draws a white rectangle and displays the measure coming from any kind of sensor
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
   // This following block fixes the previous button border by drawing a black rectangle 
    if (oldmeasure != currmeasure) {
      if (oldmeasure == 1)
        tft.drawRect(0, 0, BOXSIZE*3, BOXSIZE, ILI9341_BLACK);
      if (oldmeasure == 2)
        tft.drawRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE, ILI9341_BLACK);
    }
  }


  // Here is the most complicated part, The Slider. Again a conditional function check if the user pressed in the 
  // Slider region. 
  if ((p.y > (SLID_INIT_POSy - SLID_HEIGHTy * 2)) && (p.y < (SLID_INIT_POSy + SLID_HEIGHTy * 2))) {
    if ((p.x > SLID_INIT_POSx) && (p.x < (SLID_INIT_POSx + SLID_WIDTHx))) {
      // I don't know why I must fix the slider at right side. Theoretically this conditional check if user pressed between two specific
      // p.x values. So ... if the user is pressing outside, should not be drawn anything, but for some reason I need to fix, cause slider 
      // keeps drawing itself if the user drag the finger to the right. The following variable is intended to do this
      int corr_wid = SLID_INIT_POSx + SLID_WIDTHx - p.x - 1;

      // Filling right and left parts of slider 
      tft.fillRect(SLID_INIT_POSx, SLID_INIT_POSy, p.x, SLID_HEIGHTy, ILI9341_GREEN);
      tft.fillRect(p.x, SLID_INIT_POSy, corr_wid, SLID_HEIGHTy, ILI9341_RED);
      // Fixing the right side
      tft.fillRect(lim_posx, lim_posy, lim_wid, lim_h, ILI9341_BLACK);
      tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);

      // This part draws the limit chosen by the user by dragging the slider. If the board read a value higher than this limit 
      // from any sensor It will trigger an alarm. However, by the moment is just to draw the chosen limit in the display 
      int LIM_POSx = SLID_INIT_POSx + 75;
      int LIM_POSy = SLID_INIT_POSy - 20;
      int LIM_SIZ = 2;
      
      // Here we must compute with p.x to translate the value to different scales, anda maybe make a case, to display the 
      // right scale (depending on the measure). By the moment it is just showing the p.x value like a text string
      printText(LIM_POSx, LIM_POSy, pax, ILI9341_BLACK, LIM_SIZ);
      printText(LIM_POSx, LIM_POSy, String(p.x), ILI9341_WHITE, LIM_SIZ);
      pax = String(p.x);
    }
  }
} // void loop() end.

// In this declarative version this is the unique one function that We've used. It display a given string of characteres. 
// Maybe is not much efficient, cause a string requires much more allocated memmory than a integer. This MUST BE MODIFIED when finally 
// the display begins to show measures and limits as integers. 
unsigned long printText(int x, int y, String strg, int col, int sz) {
  unsigned long start = micros();
  //tft.setRotation(1);         // The drawn value can be rotaded
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(sz);
  tft.println(strg);
  return micros() - start;
}
