/***************************************************
  Starting touchpad/display design phase.
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <DHT.h> // Temperature and humidity library




// Constants related with physical wiring and pins. 

// Setting up DHT sensor
#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

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





// Constants relative to graphics parts: 

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40            // both buttons have the same height and width, using this parameter

// Button's Texts Positions
#define TEMP_BUTT_POSx 25
#define TEMP_BUTT_POSy 18
#define TEMP_BUTT_COL ILI9341_WHITE
#define TEMP_BUTT_SIZ 0.5
#define GAS_BUTT_POSx 97.5
#define GAS_BUTT_POSy 18
#define GAS_BUTT_COL ILI9341_WHITE
#define GAS_BUTT_SIZ 0.5
#define HUM_BUTT_POSx 175
#define HUM_BUTT_POSy 18
#define HUM_BUTT_COL ILI9341_WHITE
#define HUM_BUTT_SIZ 0.5

// Measurement position, size and colours
#define MEAS_POSx 115
#define MEAS_POSy 150
#define MEAS_SIZ 2
#define MEAS_PATCHw 25
#define MEAS_PATCHh 15
#define TEMP_MEAS_COL ILI9341_RED
#define HUM_MEAS_COL ILI9341_BLUE
#define GAS_MEAS_COL ILI9341_GREEN


// Slider position parameters
#define SLID_INIT_POSx 30
#define SLID_INIT_POSy 270
#define SLID_WIDTHx 180
#define SLID_HEIGHTy 15


// Right fix for slider.
#define fix_posx SLID_INIT_POSx + SLID_WIDTHx
#define fix_posy SLID_INIT_POSy
#define fix_wid tft.width() - SLID_INIT_POSx - SLID_WIDTHx
#define fix_h SLID_HEIGHTy


// Chosen limit by user position
#define LIM_POSx SLID_INIT_POSx + 75
#define LIM_POSy SLID_INIT_POSy - 20
#define LIM_SIZ 2
#define FIX_LIM 40
int temp_lim,temp_lim_aux,hum_lim,hum_lim_aux;




// Variables:

// Read with raw sample data.
int temperature = 0;
int humidity = 0;
int auxt = 0;
int auxh = 0;


// Possible values -----> temperature:1 ----- humidity:2 ----- gas:3 ------
int oldmeasure, currmeasure;


// Previous selected value.
int pax;





void setup() {

  // Starting Serial comm, display (tft), touchscreen (ts) and sensors (DHT)
  Serial.begin(9600);
  tft.begin();
  Serial.println("Display started");
  ts.begin();
  Serial.println("Touchscreen started");
  dht.begin();
  Serial.println("DHT started");

  // Initial sensor reading
  Serial.println("");
  Serial.print("Initial reading      =>     ");
  Serial.print("Temp -> ");
  temperature = dht.readTemperature();
  Serial.print(temperature);
  Serial.print("    /   Hum -> ");
  humidity = dht.readHumidity();
  Serial.println(humidity);
  Serial.println("");
  
  // Initial setup for display. (or optionally touchscreen uncommenting)
  // tft.setRotation(2); // Check before how to rotate touchscreen
  tft.fillScreen(ILI9341_BLACK);

  // Displaying text of buttons measure selection boxes                             // Modify defined constants
  printText(TEMP_BUTT_POSx, TEMP_BUTT_POSy, "T [C]", TEMP_BUTT_COL, TEMP_BUTT_SIZ);
  printText(GAS_BUTT_POSx, GAS_BUTT_POSy, "Hum [%]", GAS_BUTT_COL, GAS_BUTT_SIZ);
  printText(HUM_BUTT_POSx, HUM_BUTT_POSy, "Gas [ppm]", HUM_BUTT_COL, HUM_BUTT_SIZ);

  // Select the current measure 'Temp'; modify currmeasure value to setup a default measure to show
  // in this case it draws a white rectancle around the button
  currmeasure = 1;
  if (currmeasure == 1) {
    tft.drawRect(0, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
    Serial.println("TEMP");
    printInteg(MEAS_POSx, MEAS_POSy, temperature, TEMP_MEAS_COL, MEAS_SIZ);
  } else if (currmeasure == 2) {
      tft.drawRect(BOXSIZE*2,0,BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
      Serial.println("HUM");
      printInteg(MEAS_POSx, MEAS_POSy, humidity, HUM_MEAS_COL, MEAS_SIZ);
  } else if (currmeasure == 3) {
      tft.drawRect(BOXSIZE*4, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
      Serial.println("GAS");
      //printInteg(MEAS_POSx, MEAS_POSy, currmeasure, GAS_MEAS_COL, MEAS_SIZ);
  }

  // Drawing the slider border
  tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);

}


void loop() {

  // Reading sensors info and debugging.
  //Serial.println();
  //Serial.print("Sensors reading loop      =>     ");
  //Serial.print("Temp -> ");
  temperature = dht.readTemperature();
  //Serial.print(temperature);
  //Serial.println(auxt);
  //Serial.print("    /   Hum -> ");
  humidity = dht.readHumidity();
  //Serial.println(humidity);
  //Serial.println(auxh);
  
  // Wait for a touch
  if (ts.touched() == false && temperature == auxt && humidity == auxh) {
    return;
  }
      
  // Retrieve a point
  TS_Point p = ts.getPoint();
  // Scale from ~0->4000 to tft.width using the calibration #'s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  // Debuggin point data or changing measire
  //Serial.println("Any measure changes or someone touched the ts");
  Serial.print("(p.x, p.y)   =>   ");
  Serial.print("("); Serial.print(p.x);
  Serial.print(", "); Serial.print(p.y);
  Serial.println(")");
  
  // Choosing a measure. Once we receive the ts data, this condicional check if the user pressed inside button region, 
  // then, draws a white rectangle and displays the measure coming from any kind of sensor
  if (p.y < BOXSIZE && p.y > 0) {
    oldmeasure = currmeasure;
    if (p.x < BOXSIZE*2) {
      currmeasure = 1;
      tft.fillRect(MEAS_POSx, MEAS_POSy, MEAS_PATCHw, MEAS_PATCHh, ILI9341_BLACK);
      tft.drawRect(0, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
      Serial.println("TEMP");
      printInteg(MEAS_POSx, MEAS_POSy, temperature, TEMP_MEAS_COL, MEAS_SIZ);
      fillslidRender(temp_lim_aux);
      printInteg(LIM_POSx, LIM_POSy, temp_lim, ILI9341_WHITE, LIM_SIZ);
    } else if(p.x > BOXSIZE*2 && p.x < BOXSIZE*4) {
        currmeasure = 2;
        tft.fillRect(MEAS_POSx, MEAS_POSy, MEAS_PATCHw, MEAS_PATCHh, ILI9341_BLACK);
        tft.drawRect(BOXSIZE*2, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
        Serial.println("HUM");
        printInteg(MEAS_POSx, MEAS_POSy, humidity, HUM_MEAS_COL, MEAS_SIZ);
        fillslidRender(hum_lim_aux);
        tft.fillRect(LIM_POSx, LIM_POSy, FIX_LIM, MEAS_PATCHh, ILI9341_BLACK);
        printInteg(LIM_POSx, LIM_POSy, hum_lim, ILI9341_WHITE, LIM_SIZ);
    } else if (p.x > BOXSIZE*4) {
        currmeasure = 3;
        //printInteg(MEAS_POSx, MEAS_POSy, oldmeasure, ILI9341_BLACK, MEAS_SIZ);
        tft.drawRect(BOXSIZE*4, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
        Serial.println("GAS");
        //printInteg(MEAS_POSx, MEAS_POSy, currmeasure, GAS_MEAS_COL, MEAS_SIZ);
    }
   // This following block fixes the previous button border by drawing a black rectangle 
    if (oldmeasure != currmeasure) {
      if (oldmeasure == 1)
        tft.drawRect(0, 0, BOXSIZE*2, BOXSIZE, ILI9341_BLACK);
      if (oldmeasure == 2)
        tft.drawRect(BOXSIZE*2, 0, BOXSIZE*2, BOXSIZE, ILI9341_BLACK);
      if (oldmeasure == 3) 
        tft.drawRect(BOXSIZE*4, 0, BOXSIZE*2, BOXSIZE, ILI9341_BLACK);  
    }
  }
  

  
  switch (currmeasure) {
    case 1:
        tft.fillRect(MEAS_POSx, MEAS_POSy, MEAS_PATCHw, MEAS_PATCHh, ILI9341_BLACK);
        Serial.println("TEMP");
        printInteg(MEAS_POSx, MEAS_POSy, temperature, TEMP_MEAS_COL, MEAS_SIZ);
        auxt = temperature;
        auxh = humidity;
        
        // Using a specific function to render the slider when user press in proper region
        if ((p.y > (SLID_INIT_POSy - SLID_HEIGHTy * 2)) && (p.y < (SLID_INIT_POSy + SLID_HEIGHTy * 2)) && p.y > 0) {
            if ((p.x > SLID_INIT_POSx) && (p.x < (SLID_INIT_POSx + SLID_WIDTHx))) {
                fillslidRender(p.x);
                temp_lim = (int)(0.280*p.x - 8.71);
                temp_lim_aux = p.x;
                printInteg(LIM_POSx, LIM_POSy, temp_lim, ILI9341_WHITE, LIM_SIZ);
            }
        }
    break;
    case 2:
        tft.fillRect(MEAS_POSx, MEAS_POSy, MEAS_PATCHw, MEAS_PATCHh, ILI9341_BLACK);
        Serial.println("HUM");
        printInteg(MEAS_POSx, MEAS_POSy, humidity, HUM_MEAS_COL, MEAS_SIZ);
        auxh = humidity;
        auxt = temperature;

        // Using a specific function to render the slider when user press in proper region
        if ((p.y > (SLID_INIT_POSy - SLID_HEIGHTy * 2)) && (p.y < (SLID_INIT_POSy + SLID_HEIGHTy * 2)) && p.y > 0) {
            if ((p.x > SLID_INIT_POSx) && (p.x < (SLID_INIT_POSx + SLID_WIDTHx))) {
                fillslidRender(p.x);
                hum_lim = (int)(0.337*p.x + 9.57);
                hum_lim_aux = p.x;
                printInteg(LIM_POSx, LIM_POSy, hum_lim, ILI9341_WHITE, LIM_SIZ);
            }
        }
    break;
    //case 3:
    //break;  
  }


//delayMicroseconds(100);
} // void loop() end.


// This function renders the slider filling when user press over its region
unsigned long fillslidRender (int x) {
  unsigned long start = micros();
      // I don't know why I must fix the slider at right side. Theoretically this function is used inside a conditional which checks if the
      // user pressed between two specific p.x values. So ... if the user is pressing outside, should not be drawn anything, 
      // but for some reason I need to fix, cause slider keeps drawing itself if the user drag the finger to the right. 
      // The following variable is intended to do this
      int corr_wid = SLID_INIT_POSx + SLID_WIDTHx - x - 1;

      // Filling right and left parts of slider 
      tft.fillRect(SLID_INIT_POSx, SLID_INIT_POSy, x, SLID_HEIGHTy, ILI9341_GREEN);
      tft.fillRect(x, SLID_INIT_POSy, corr_wid, SLID_HEIGHTy, ILI9341_BLACK);
      // Fixing the right side and white border
      tft.fillRect(fix_posx, fix_posy, fix_wid, fix_h, ILI9341_BLACK);
      tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);

      // This part draws the limit chosen by the user by dragging the slider. If the board read a value higher than this limit 
      // from any sensor It will trigger an alarm. However, by the moment is just to draw the chosen limit in the display.
      // Here we must compute with p.x to translate the value to different scales, anda maybe make a case, to display the 
      // right scale (depending on the measure). By the moment it is just showing the p.x value like a text string
      tft.fillRect(LIM_POSx, LIM_POSy, FIX_LIM, MEAS_PATCHh, ILI9341_BLACK);
      return micros() - start;
}

// Function to display a given string of characteres. Maybe is not much efficient, cause a string requires much more 
// allocated memmory than a integer.
unsigned long printText(int x, int y, String strg, int col, int sz) {
  unsigned long start = micros();
  //tft.setRotation(1);         // The drawn value can be rotaded
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(sz);
  tft.println(strg);
  return micros() - start;
}

// This function is very similar to previous one, but in this case we are printing an integer, lighter than a string. Apparently 
// the performing is not being affected
unsigned long printInteg(int x, int y, int val, int col, int sz) {
  unsigned long start = micros();
  //tft.setRotation(1);         // The drawn value can be rotaded
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(sz);
  tft.println(val);
  return micros() - start;
}
