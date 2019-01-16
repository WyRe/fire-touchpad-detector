/***************************************************
  Starting touchpad/display design phase.
 ****************************************************/


#include <SPI.h>              // SCPI header-related library
#include <Wire.h>             // this is needed even tho we aren't using it
#include <DHT.h>              // Temperature and humidity library
#include <EEPROM.h>           // Built-in EEPROM in Arduino MEGA 2560 requires it
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ILI9341.h> // Display Adafruit library
#include <Adafruit_STMPE610.h>// Touchscreen Adafruit library




// Constants related with physical wiring and pins. 

// ACTUATORS
// Piranha LED
#define PH_LED_PIN 44

// SENSORS
// Setting up flame sensor
#define PIN_FLA 41
#define APIN_FLA A9
#define AVOLT_FLA 920         // Flame sensor analog output milivolts, this value is used to scale and make a precent

// Setting up MQ2 gas sensor
#define PIN_MQ2 33
#define APIN_MQ2 A8

// Setting up DHT sensor
#define PIN_DHT 22
#define DHTTYPE DHT11
DHT dht(PIN_DHT,DHTTYPE);

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


// Constants relative to graphics parts:

#define REFRESH_RATE 500               // In milliseconds

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40                      // both buttons have the same height and width, using this parameter

#define BACK_COL GLCD_CL_BLACK

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
#define MEAS_POSx 105
#define MEAS_POSy 150
#define MEAS_SIZ 2
#define TEMP_MEAS_POSx MEAS_POSx + 40
#define TEMP_MEAS_TEXTx MEAS_POSx - 50
#define GAS_MEAS_POSx MEAS_POSx + 20
#define GAS_MEAS_TEXTx MEAS_POSx - 50

#define TEMP_MEAS_COL ILI9341_RED
#define HUM_MEAS_COL GLCD_CL_DARK_CYAN
#define LPG_MEAS_COL ILI9341_GREEN
#define H2_MEAS_COL GLCD_CL_PURPLE
#define CO_MEAS_COL GLCD_CL_ORANGE

#define MEAS_PATCHx 50
#define MEAS_PATCHy 110
#define MEAS_PATCHw 160
#define MEAS_PATCHh 100
#define MEAS_PATCH_COL ILI9341_YELLOW

// Top left arros for temp and hum
#define TEMP_ARR_POSx 20
#define TEMP_ARR_POSy 110
#define HUM_ARR_POSx 20
#define HUM_ARR_POSy 130


// Slider position and colour parameters
#define SLID_INIT_POSx 30
#define SLID_INIT_POSy 270
#define SLID_WIDTHx 180
#define SLID_HEIGHTy 15
#define LEFT_SLID_COL ILI9341_WHITE
#define RIGHT_SLID_COL ILI9341_BLACK


// Slider scales
#define TEMP_SLID_SCALa 0.280
#define TEMP_SLID_SCALb -8.71
#define HUM_SLID_SCALa 0.337
#define HUM_SLID_SCALb -0.447
#define GAS_SLID_SCALa 11.7
#define GAS_SLID_SCALb -11240


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


// Measure mark width in slider
#define MARK_WIDTH 5


// Smoke indicator
#define SMOK_TXT_POSx 10
#define SMOK_TXT_POSy 60
#define SMOK_TXT_COL ILI9341_WHITE
#define SMOK_TXT_SIZ 1
#define SMOK_IND_POSx 50
#define SMOK_IND_SIDE 10
#define SMOK_IND_BORD_COL ILI9341_WHITE
#define SMOK_IND_COL GLCD_CL_GREEN

// Flame indicator
#define FLA_TXT_POSx 10
#define FLA_TXT_POSy 80
#define FLA_TXT_COL ILI9341_WHITE
#define FLA_TXT_SIZ 1
#define FLA_IND_POSx 50
#define FLA_IND_SIDE 10
#define FLA_IND_BORD_COL ILI9341_WHITE
#define FLA_IND_COL GLCD_CL_MAGENTA

// Variables:

// Slider-related
int temp_lim;
int temp_lim_px;
int oldtmp_lim_px = 0;
int hum_lim;
int hum_lim_px;
int oldhum_lim_px = 0;

// Flame sensor relative var
bool fla_state;
bool auxflast;
float fla_value;
float auxflaval;
float fla_volt;

// Gas sensor relative variables
float mq2_volt;
int mq2_value;
int auxmq2;
bool mq2_state;
bool auxmq2st;
float RS_gas; // Get value of RS in a GAS
float ratio; // Get ratio RS_GAS/RS_air
float lpg; // concentration of LPGs
float dihyd; // concentration of H2
float co; // concentration of CO
float auxlpg;
float auxdihyd;
float auxco;


// Read with raw sample data.
int temperature;
int humidity;
int auxt;
int auxh;


// Possible values -----> temperature:1 ----- humidity:2 ----- gas:3 ------
int oldmeasure, currmeasure;


// Previous selected value.
int pax;


// Retains measure reading loop with no delay for loop() cycles
long lastreadingtime=0;


void setup() {

  // Starting Serial comm, display (tft), touchscreen (ts) and sensors (DHT)
  Serial.begin(9600);
  tft.begin();
  Serial.println("Display started");
  ts.begin();
  Serial.println("Touchscreen started");
  dht.begin();
  Serial.println("DHT started");

  // LED pin setup (Actuators)
  pinMode(PH_LED_PIN, OUTPUT);

  // Flame sensor
  pinMode(PIN_FLA, INPUT);
  fla_state = digitalRead(PIN_FLA);
  fla_volt = abs(AVOLT_FLA - analogRead(APIN_FLA));
  fla_value = (float)(fla_volt*100/AVOLT_FLA);

  // Initial MQ2 sensor reading
  pinMode(PIN_MQ2, INPUT); // Set sensor - pin 33 as an input
  mq2_value = analogRead(APIN_MQ2);
  mq2_state = digitalRead(PIN_MQ2);
  mq2_volt = (float)(mq2_value/1024*5.0);
  RS_gas = (5.0-mq2_volt)/mq2_volt;     // omit * RL (1000)
  ratio = RS_gas/2.70;    // ratio = RS/R0, with R0=2.70
  lpg = (int)(482.67 * pow(ratio, -2.542));
  dihyd = (int)(871.81 * pow(ratio, -2.175));
  co = abs((int)(17250 * pow(ratio, -2.668)));          // I've introduced abs() function because co concentration was giving negative value (Sensor calibration?)

  // Initial DHT sensor reading
  Serial.println("");
  Serial.print("Initial reading      =>     ");
  Serial.print("Temp -> ");
  temperature = dht.readTemperature();
  auxt = temperature;
  Serial.print(temperature);
  Serial.print("    /   Hum -> ");
  humidity = dht.readHumidity();
  auxh = humidity;
  Serial.println(humidity);
  Serial.println("");

  // Initial setup for display. (or optionally touchscreen uncommenting)
  //tft.setRotation(2); // Check before how to rotate touchscreen
  tft.fillScreen(BACK_COL);

  // Displaying text of buttons measure selection boxes                             // Modify defined constants
  printText(TEMP_BUTT_POSx, TEMP_BUTT_POSy, "T [C]", TEMP_BUTT_COL, TEMP_BUTT_SIZ);
  printText(GAS_BUTT_POSx, GAS_BUTT_POSy, "Hum [%]", GAS_BUTT_COL, GAS_BUTT_SIZ);
  printText(HUM_BUTT_POSx, HUM_BUTT_POSy, "Gas [ppm]", HUM_BUTT_COL, HUM_BUTT_SIZ);

  // Drawing the slider border
  tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);

  /***
  Select the current measure 'Temp'; modify currmeasure value to setup a default measure to show
  in this case it draws a white rectancle around the button
  ***/
  temp_lim_px = EEPROM.read(2);
  temp_lim = EEPROM.read(1);
  hum_lim_px = EEPROM.read(4);
  hum_lim = EEPROM.read(3);
  currmeasure = EEPROM.read(0);
 
  if (currmeasure == 1) {
    tft.drawRect(0, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
    Serial.println("Default measure: TEMP");
    printText(TEMP_MEAS_TEXTx, MEAS_POSy, "Temp.", TEMP_MEAS_COL, MEAS_SIZ);
    printInteg(TEMP_MEAS_POSx, MEAS_POSy, temperature, TEMP_MEAS_COL, MEAS_SIZ);
    printText(TEMP_MEAS_TEXTx, MEAS_POSy + 20, "IR(%).", TEMP_MEAS_COL, MEAS_SIZ);
    printInteg(TEMP_MEAS_POSx, MEAS_POSy + 20, fla_value, TEMP_MEAS_COL, MEAS_SIZ);
    fillslidRender(temp_lim_px);
    printInteg(LIM_POSx, LIM_POSy, temp_lim, ILI9341_WHITE, LIM_SIZ);
  } else if (currmeasure == 2) {
      tft.drawRect(BOXSIZE*2,0,BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
      Serial.println("Default measure: HUM");
      printText(MEAS_POSx - 40, MEAS_POSy, "Hum.", HUM_MEAS_COL, MEAS_SIZ);
      printInteg(MEAS_POSx + 30, MEAS_POSy, humidity, HUM_MEAS_COL, MEAS_SIZ);
      fillslidRender(hum_lim_px);
      printInteg(LIM_POSx, LIM_POSy, hum_lim, ILI9341_WHITE, LIM_SIZ);
  } else if (currmeasure == 3) {
      tft.drawRect(BOXSIZE*4, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
      Serial.println("Default measure: GAS");
      // There is no need for initial gas measures config, because this measures have not dynamic limiter.  
  }

  // Smoke indicator
  printText(SMOK_TXT_POSx, SMOK_TXT_POSy + 2, "Smoke", SMOK_TXT_COL, SMOK_TXT_SIZ);
  tft.drawRect(SMOK_IND_POSx, SMOK_TXT_POSy, SMOK_IND_SIDE, SMOK_IND_SIDE, SMOK_IND_BORD_COL);
  // Flame indicator
  printText(FLA_TXT_POSx, FLA_TXT_POSy + 2, "Flame", FLA_TXT_COL, FLA_TXT_SIZ);
  tft.drawRect(FLA_IND_POSx, FLA_TXT_POSy, FLA_IND_SIDE, FLA_IND_SIDE, FLA_IND_BORD_COL);
}


void loop() {


  // Reading sensors info and debugging.
  if(millis()-lastreadingtime>REFRESH_RATE) {
  //Serial.println();
  //Serial.print("Sensors reading loop      =>     ");
  //Serial.print("Temp -> ");
  temperature = dht.readTemperature();
  //Serial.print(temperature);Serial.print("/");
  //Serial.print(auxt);
  //Serial.print(" /  Hum -> ");
  humidity = dht.readHumidity();
  //Serial.print(humidity);Serial.print("/");
  //Serial.println(auxh);

  fla_volt = abs(AVOLT_FLA - analogRead(APIN_FLA));
  fla_value = (float)(fla_volt*100/AVOLT_FLA);

  mq2_state = digitalRead(PIN_MQ2);
  fla_state = digitalRead(PIN_FLA);

  mq2_value = analogRead(APIN_MQ2);
  mq2_volt = (float)(mq2_value/1024*5.0);
  RS_gas = (5.0-mq2_volt)/mq2_volt;     // omit * RL (1000)
  ratio = RS_gas/2.70;    // ratio = RS/R0, with R0=2.70
  lpg = (int)(482.67 * pow(ratio, -2.542)); 
  dihyd = (int)(871.81 * pow(ratio, -2.175));
  co = abs((int)(17250 * pow(ratio, -2.668)));
  lastreadingtime=millis();
  }

  // Prioritise mq2 and flame sensor boolean states. It is not a good idea because can generate display blinking behaviour.
  //mq2_state = digitalRead(PIN_MQ2);
  //fla_state = digitalRead(PIN_FLA);

  // See if there's any  touch data for us
  if (ts.bufferEmpty() == true && temperature == auxt && humidity == auxh && mq2_value == auxmq2 && mq2_state == auxmq2st && fla_state == auxflast && fla_value == auxflaval) {
      return;
  }

  /***
  Different ways to interact with touchpad and control code execution.
  Those ways introduces delay in this design.
  ***/
  /***
  /***
  //Wait for a touch
  if (ts.touched() == false && temperature == auxt && humidity == auxh) {
     return;
  }
  ***/
  /***
  // Process data always, but with same value of p if no one touch the ts
  while(ts.touched()) {
    TS_Point p = ts.getPoint();
  }
  ***/

  
  // Retrieve a point
  TS_Point p = ts.getPoint();
  // Scale from ~0->4000 to tft.width using the calibration #'s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  /***
  //Debuggin point data or changing measire
  Serial.println("Any measure changes or someone touched the ts");
  Serial.print("(p.x, p.y)   =>   ");
  Serial.print("("); Serial.print(p.x);
  Serial.print(", "); Serial.print(p.y);
  Serial.println(")");
  ***/

  /***
  If any variable changed the first thing to do (after retrieve the point if user pressed) is check if this new value
  is greater or lower than limit chosen by user.
  ***/

  //Serial.println("Checking variables and limits:");
  checkVarArrow(temperature, temp_lim, TEMP_ARR_POSx, TEMP_ARR_POSy, TEMP_MEAS_COL);
  checkVarArrow(humidity, hum_lim, HUM_ARR_POSx, HUM_ARR_POSy, HUM_MEAS_COL);

  checkTemp(temperature, temp_lim);

  checkSmoke(mq2_state);
  checkFlame(fla_state);

  /***
  Choosing a measure. Once we receive the ts data, this condicional checks if the user pressed inside any button region,
  then, draws a white rectangle and displays the measure coming from any kind of sensor
  ***/
  temp_lim_px = EEPROM.read(2);
  temp_lim = EEPROM.read(1);
  hum_lim_px = EEPROM.read(4);
  hum_lim = EEPROM.read(3);
  
  if (p.y < BOXSIZE && p.y > 0) {
    oldmeasure = currmeasure;
    if (p.x < BOXSIZE*2) {
      currmeasure = 1;
      EEPROM.write(0, currmeasure);
      printInteg(MEAS_POSx, MEAS_POSy, auxt, BACK_COL, MEAS_SIZ);
      tft.drawRect(0, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
      //Serial.println("TEMP");
      printText(TEMP_MEAS_TEXTx, MEAS_POSy, "Temp.", TEMP_MEAS_COL, MEAS_SIZ);
      printInteg(TEMP_MEAS_POSx, MEAS_POSy, temperature, TEMP_MEAS_COL, MEAS_SIZ);
      printText(TEMP_MEAS_TEXTx, MEAS_POSy + 20, "IR(%).", TEMP_MEAS_COL, MEAS_SIZ);
      printInteg(TEMP_MEAS_POSx, MEAS_POSy + 20, fla_value, TEMP_MEAS_COL, MEAS_SIZ);
      fillslidRender(temp_lim_px);
      tft.fillRect((int)((temperature-TEMP_SLID_SCALb)/TEMP_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, TEMP_MEAS_COL);
      printInteg(LIM_POSx, LIM_POSy, temp_lim, ILI9341_WHITE, LIM_SIZ);
    } else if(p.x > BOXSIZE*2 && p.x < BOXSIZE*4) {
        currmeasure = 2;
        EEPROM.write(0,currmeasure);
        printInteg(MEAS_POSx, MEAS_POSy, auxh, BACK_COL, MEAS_SIZ);
        tft.drawRect(BOXSIZE*2, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
        //Serial.println("HUM");
        printText(MEAS_POSx - 40, MEAS_POSy, "Hum.", HUM_MEAS_COL, MEAS_SIZ);
        printInteg(MEAS_POSx + 30, MEAS_POSy, humidity, HUM_MEAS_COL, MEAS_SIZ);
        fillslidRender(hum_lim_px);
        tft.fillRect((int)((humidity-HUM_SLID_SCALb)/HUM_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, HUM_MEAS_COL);
        printInteg(LIM_POSx, LIM_POSy, hum_lim, ILI9341_WHITE, LIM_SIZ);
    } else if (p.x > BOXSIZE*4) {
        currmeasure = 3;
        EEPROM.write(0,currmeasure);
        tft.drawRect(BOXSIZE*4, 0, BOXSIZE*2, BOXSIZE, ILI9341_WHITE);
        //Serial.println("GAS");

        printInteg(GAS_MEAS_POSx, MEAS_POSy - 10, auxlpg, BACK_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 10, auxdihyd, BACK_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 30, auxco, BACK_COL, MEAS_SIZ);

        printText(GAS_MEAS_TEXTx, MEAS_POSy - 10, "LPGs:", LPG_MEAS_COL, MEAS_SIZ);
        printText(GAS_MEAS_TEXTx, MEAS_POSy + 10, "H2:", H2_MEAS_COL, MEAS_SIZ);
        printText(GAS_MEAS_TEXTx, MEAS_POSy + 30, "CO:", CO_MEAS_COL, MEAS_SIZ);

        printInteg(GAS_MEAS_POSx, MEAS_POSy - 10, lpg, LPG_MEAS_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 10, dihyd, H2_MEAS_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 30, co, CO_MEAS_COL, MEAS_SIZ);

        printText(SLID_INIT_POSx - 2, SLID_INIT_POSy + 20, "0", ILI9341_WHITE, 1);
        printText(SLID_INIT_POSx + SLID_WIDTHx - 4, SLID_INIT_POSy + 20, "50k", ILI9341_WHITE, 1);

        printgasMarks(auxlpg, auxdihyd, auxco, lpg, dihyd, co);
   }
   // This following block fixes the previous button border and shown info drawing elements with background color
   if (oldmeasure != currmeasure) {
      if (oldmeasure == 1)
        tft.drawRect(0, 0, BOXSIZE*2, BOXSIZE, BACK_COL);
        printText(TEMP_MEAS_TEXTx, MEAS_POSy, "Temp.", BACK_COL, MEAS_SIZ);
        printInteg(TEMP_MEAS_POSx, MEAS_POSy, temperature, BACK_COL, MEAS_SIZ);
        printText(TEMP_MEAS_TEXTx, MEAS_POSy + 20, "IR(%).", BACK_COL, MEAS_SIZ);
        printInteg(TEMP_MEAS_POSx, MEAS_POSy + 20, fla_value, BACK_COL, MEAS_SIZ);
      if (oldmeasure == 2)
        tft.drawRect(BOXSIZE*2, 0, BOXSIZE*2, BOXSIZE, BACK_COL);
        printText(MEAS_POSx - 40, MEAS_POSy, "Hum.", BACK_COL, MEAS_SIZ);
        printInteg(MEAS_POSx + 30, MEAS_POSy, humidity, BACK_COL, MEAS_SIZ);
      if (oldmeasure == 3) 
        tft.drawRect(BOXSIZE*4, 0, BOXSIZE*2, BOXSIZE, BACK_COL);

        printText(SLID_INIT_POSx - 2, SLID_INIT_POSy + 20, "0", BACK_COL, 1);
        printText(SLID_INIT_POSx + SLID_WIDTHx - 4, SLID_INIT_POSy + 20, "50k", BACK_COL, 1);

        printText(GAS_MEAS_TEXTx, MEAS_POSy - 10, "LPGs:", BACK_COL, MEAS_SIZ);
        printText(GAS_MEAS_TEXTx, MEAS_POSy + 10, "H2:", BACK_COL, MEAS_SIZ);
        printText(GAS_MEAS_TEXTx, MEAS_POSy + 30, "CO:", BACK_COL, MEAS_SIZ);

        printInteg(GAS_MEAS_POSx, MEAS_POSy - 10, lpg, BACK_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 10, dihyd, BACK_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 30, co, BACK_COL, MEAS_SIZ);
    }
  }
  

  
  switch (currmeasure) {
    case 1:
        printInteg(TEMP_MEAS_POSx, MEAS_POSy + 20, auxflaval, BACK_COL, MEAS_SIZ);
        printInteg(TEMP_MEAS_POSx, MEAS_POSy, auxt, BACK_COL, MEAS_SIZ);
        //Serial.println("TEMP");
        printInteg(TEMP_MEAS_POSx, MEAS_POSy, temperature, TEMP_MEAS_COL, MEAS_SIZ);
        printInteg(TEMP_MEAS_POSx, MEAS_POSy + 20, fla_value, TEMP_MEAS_COL, MEAS_SIZ);

        // This conditional fixes the mark depending on if it was over right or left slider sides
        if (auxt < temp_lim) {
          tft.fillRect((int)((auxt-TEMP_SLID_SCALb)/TEMP_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, LEFT_SLID_COL);
        } else {
            tft.fillRect((int)((auxt-TEMP_SLID_SCALb)/TEMP_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, RIGHT_SLID_COL);
        }

        // Using a specific function to render the slider when user press in proper region
        if ((p.y > (SLID_INIT_POSy - SLID_HEIGHTy * 2)) && (p.y < (SLID_INIT_POSy + SLID_HEIGHTy * 2)) && p.y > 0) {
            if ((p.x > SLID_INIT_POSx) && (p.x < (SLID_INIT_POSx + SLID_WIDTHx))) {
                fillslidRender(p.x);
                temp_lim = (int)(TEMP_SLID_SCALa*p.x + TEMP_SLID_SCALb);
                EEPROM.write(1, temp_lim);      // Writing chosen limit for temperature at addres 0
                temp_lim_px = p.x;
                EEPROM.write(2, temp_lim_px);  // Writing its corresponding p.x at addres 1
                printInteg(LIM_POSx, LIM_POSy, temp_lim, ILI9341_WHITE, LIM_SIZ);
            }
        }
        // Drawing a mark over slider, it will be placed in the measure specified value.
        tft.fillRect((int)((temperature-TEMP_SLID_SCALb)/TEMP_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, TEMP_MEAS_COL);
    break;
    case 2:
        printInteg(MEAS_POSx + 30, MEAS_POSy, auxh, BACK_COL, MEAS_SIZ);
        //Serial.println("HUM");
        printInteg(MEAS_POSx + 30, MEAS_POSy, humidity, HUM_MEAS_COL, MEAS_SIZ);

        // This conditional fixes the mark depending on if it was over right or left slider sides
        if (auxh < hum_lim) {
          tft.fillRect((int)((auxh-HUM_SLID_SCALb)/HUM_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, LEFT_SLID_COL);
        } else {
            tft.fillRect((int)((auxh-HUM_SLID_SCALb)/HUM_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, RIGHT_SLID_COL);
        }

        // Using a specific function to render the slider when user press in proper region
        if ((p.y > (SLID_INIT_POSy - SLID_HEIGHTy * 2)) && (p.y < (SLID_INIT_POSy + SLID_HEIGHTy * 2)) && p.y > 0) {
            if ((p.x > SLID_INIT_POSx) && (p.x < (SLID_INIT_POSx + SLID_WIDTHx))) {
                fillslidRender(p.x);
                hum_lim = (int)(HUM_SLID_SCALa*p.x + HUM_SLID_SCALb);
                EEPROM.write(3, hum_lim);       // Writing chosen limit for humidity at addres 2
                hum_lim_px = p.x;
                EEPROM.write(4, hum_lim_px);   // Addres 3 to save its corresponding p.x and redraw the slider
                printInteg(LIM_POSx, LIM_POSy, hum_lim, ILI9341_WHITE, LIM_SIZ);
            }
        }
        // Drawing a mark over slider, it will be placed in the measure specified value.
        tft.fillRect((int)((humidity-HUM_SLID_SCALb)/HUM_SLID_SCALa), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, HUM_MEAS_COL);
    break;
    case 3:
        //Serial.println("GAS");
        printText(SLID_INIT_POSx - 2, SLID_INIT_POSy + 20, "0", ILI9341_WHITE, 1);
        printText(SLID_INIT_POSx + SLID_WIDTHx - 4, SLID_INIT_POSy + 20, "50k", ILI9341_WHITE, 1);

        printInteg(GAS_MEAS_POSx, MEAS_POSy - 10, auxlpg, BACK_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 10, auxdihyd, BACK_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 30, auxco, BACK_COL, MEAS_SIZ);

        printText(GAS_MEAS_TEXTx, MEAS_POSy - 10, "LPGs:", LPG_MEAS_COL, MEAS_SIZ);
        printText(GAS_MEAS_TEXTx, MEAS_POSy + 10, "H2:", H2_MEAS_COL, MEAS_SIZ);
        printText(GAS_MEAS_TEXTx, MEAS_POSy + 30, "CO:", CO_MEAS_COL, MEAS_SIZ);

        printInteg(GAS_MEAS_POSx, MEAS_POSy - 10, lpg, LPG_MEAS_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 10, dihyd, H2_MEAS_COL, MEAS_SIZ);
        printInteg(GAS_MEAS_POSx, MEAS_POSy + 30, co, CO_MEAS_COL, MEAS_SIZ);

        printgasMarks(auxlpg, auxdihyd, auxco, lpg, dihyd, co);
    break;
  }

auxh = humidity;
auxt = temperature;
auxflaval = fla_value;
auxflast = fla_state;
auxlpg = lpg;
auxdihyd = dihyd;
auxco = co;
auxmq2 = mq2_value;
auxmq2st = mq2_state;

/***
 IMPORTANT: I cannot use any delay here, despite of sensor has 1Hz sampling rate, we must read the sensor permanently, because any delay
 will collide with touchscreen device, the user cannot percibe any kind of delay when he touch the touchscreen. So if the maximun refresh
 rate for the sensor is 1Hz, that would be the maximun auto-refresh frequency for the display. The loop() function returns to start if
 there is no changes in measures or no one touches the screen.
***/

} // void loop() end.

// Function to check if temperature collides with chosen limit.
unsigned long checkVarArrow(int t, int t_lim, int posx, int posy, int col) {
  // Checking temperature
  if (t == t_lim) {
      tft.fillTriangle(posx - 12, posy, posx, posy - 5, posx, posy + 5, BACK_COL);
      tft.fillTriangle(posx + 12, posy, posx, posy - 5, posx, posy + 5, BACK_COL);
      tft.setCursor(posx-4, posy-5);
      tft.setTextColor(col);tft.setTextSize(2);
      tft.println("=");
  } else if (t != t_lim) {
      if (t < t_lim) {
      tft.setCursor(posx-4, posy-5);
      tft.setTextColor(BACK_COL);tft.setTextSize(2);
      tft.println("=");
      tft.fillTriangle(posx - 12, posy, posx, posy - 5, posx, posy + 5, BACK_COL);
      tft.fillTriangle(posx + 12, posy, posx, posy - 5, posx, posy + 5, col);
  } else if (t > t_lim) {
      tft.setCursor(posx-4, posy-5);
      tft.setTextColor(BACK_COL);tft.setTextSize(2);
      tft.println("=");
      tft.fillTriangle(posx + 12, posy, posx, posy - 5, posx, posy + 5, BACK_COL);
      tft.fillTriangle(posx - 12, posy, posx, posy - 5, posx, posy + 5, col);
  }
  }
  return;
}

/***
// Function to check if any variable collides with chosen limit.
unsigned long checkVar(int i, int i_lim) {
  // Checking variable value
  if (i == i_lim) {
      // Things to do when variable is equal than chosen limit

  } else if (i < i_lim) {
      // Things to do when variable is lower than chosen limit

  } else if (i > i_lim) {
      // Things to do when variable is greater than chosen limit

  }
  return;
}
***/

// Function to check if temperature collides with chosen limit.
unsigned long checkTemp(int t, int t_lim) {
  int k;
  // Checking variable value 
  if (t == t_lim) {
      // Things to do when variable is equal than chosen limit
      digitalWrite(PH_LED_PIN, LOW);
  } else {
      // Things to do when variable is different than chosen limit
      k = int(abs(t - t_lim)*255/50);
      analogWrite(PH_LED_PIN, k);
  }
  return;
}

// Checks smoke presence
unsigned long checkSmoke(bool s) {
  if (s == false){
    tft.fillRect(SMOK_IND_POSx+2, SMOK_TXT_POSy+2, SMOK_IND_SIDE-4, SMOK_IND_SIDE-4, SMOK_IND_COL);
  } else if (s == true) {
    tft.fillRect(SMOK_IND_POSx+2, SMOK_TXT_POSy+2, SMOK_IND_SIDE-4, SMOK_IND_SIDE-4, BACK_COL);
  }
  return;
}

// Checks smoke presence
unsigned long checkFlame(bool f) {
  if (f == false){
    tft.fillRect(FLA_IND_POSx+2, FLA_TXT_POSy+2, FLA_IND_SIDE-4, FLA_IND_SIDE-4, FLA_IND_COL);
  } else if(f == true){
    tft.fillRect(FLA_IND_POSx+2, FLA_TXT_POSy+2, FLA_IND_SIDE-4, FLA_IND_SIDE-4, BACK_COL);
  }
  return;
}

// Print gas markers inside slider, which becomes into a scale from 0 to 50k ppm.
unsigned long printgasMarks (int al, int ah, int ac, int l, int h, int c) {
        tft.fillRect(SLID_INIT_POSx, SLID_INIT_POSy, SLID_WIDTHx, SLID_HEIGHTy, BACK_COL);
        tft.fillRect((int)(sqrt(al-GAS_SLID_SCALb/GAS_SLID_SCALa)), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, BACK_COL);
        tft.fillRect((int)(sqrt(ah-GAS_SLID_SCALb/GAS_SLID_SCALa)), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, BACK_COL);
        tft.fillRect((int)(sqrt(ac-GAS_SLID_SCALb/GAS_SLID_SCALa)), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, BACK_COL);
        tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);


        tft.fillRect((int)(sqrt(l-GAS_SLID_SCALb/GAS_SLID_SCALa)), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, LPG_MEAS_COL);
        tft.fillRect((int)(sqrt(h-GAS_SLID_SCALb/GAS_SLID_SCALa)), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, H2_MEAS_COL);
        tft.fillRect((int)(sqrt(c-GAS_SLID_SCALb/GAS_SLID_SCALa)), SLID_INIT_POSy, MARK_WIDTH, SLID_HEIGHTy, CO_MEAS_COL);

        // This part fixes the previous limit chosen by user.
        tft.fillRect(LIM_POSx, LIM_POSy, FIX_LIM, 15, BACK_COL);
        return;
}

// This function renders the slider filling when user press over its region
unsigned long fillslidRender (int x) {
      /***
      I don't know why I must fix the slider at right side. Theoretically this function is used inside a conditional which checks if the
      user pressed between two specific p.x values. So ... if the user is pressing outside, should not be drawn anything,
      but for some reason I need to fix, cause slider keeps drawing itself if the user drag the finger to the right.
      The following variable is intended to do this
      ***/
      int corr_wid = SLID_INIT_POSx + SLID_WIDTHx - x - 1;
      Serial.println(x);
      // Filling right and left parts of slider 
      tft.fillRect(SLID_INIT_POSx, SLID_INIT_POSy, x - SLID_INIT_POSx, SLID_HEIGHTy, LEFT_SLID_COL);
      tft.fillRect(x, SLID_INIT_POSy, corr_wid, SLID_HEIGHTy, RIGHT_SLID_COL);
      // Fixing the right side and white border
      tft.fillRect(fix_posx, fix_posy, fix_wid, fix_h, BACK_COL);
      tft.drawRect(SLID_INIT_POSx - 1, SLID_INIT_POSy - 1, SLID_WIDTHx + 1, SLID_HEIGHTy + 2, ILI9341_WHITE);

      // This part fixes the previous limit chosen by user.
      tft.fillRect(LIM_POSx, LIM_POSy, FIX_LIM, 15, BACK_COL);
      return;
}

/***
Function to display a given string of characteres. Maybe is not much efficient, cause a string requires much more
allocated memmory than a integer.
***/
unsigned long printText(int x, int y, String strg, int col, int sz) {
  //tft.setRotation(1);         // The drawn value can be rotaded
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(sz);
  tft.println(strg);
  return;
}

/***
This function is very similar to previous one, but in this case we are printing an integer, lighter than a string. Apparently
the performing is not being affected
***/
unsigned long printInteg(int x, int y, int val, int col, int sz) {
  //tft.setRotation(1);         // The drawn value can be rotaded
  tft.setCursor(x, y);
  tft.setTextColor(col); tft.setTextSize(sz);
  tft.println(val);
  return;
}
