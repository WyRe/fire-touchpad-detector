// These are the pins we are going to use
int pin_mq = 33;
const int buzzer = 34;
const int ledPIN = 35;

void setup() {
    Serial.begin(9600);
    pinMode(pin_mq, INPUT); // Set sensor - pin 9 as an input
    pinMode(buzzer, OUTPUT); // Set buzzer - pin 3 as an output
    pinMode(ledPIN , OUTPUT); // Set led - pin 4 as an output
}

void loop() {

    float sensor_volt;
    float RS_gas; // Get value of RS in a GAS
    float ratio; // Get ratio RS_GAS/RS_air
    int sensorValue = analogRead(A8);

    float lpg; // concentration of LPGs
    float dihyd; // concentration of H2
    float co; // concentration of CO

// This first part is used for detectig if there is smoke or flammable gases. 
// If they are detected te alarm will turn on. Buzzer and led.

  boolean mq_estado = digitalRead(pin_mq); // Leemos el sensor
  if(mq_estado) // if the digital output is 1 (no smoke)
  {
    Serial.println("No smoke detected");

    noTone(buzzer);     // Stop sound
    digitalWrite(ledPIN , LOW);    // turn off the led
          
  }
  
  else //if the digital output is 0 (smoke)
  {
    Serial.println("Smoke detected");

    tone(buzzer, 1000); // Send 1KHz sound signal...
    digitalWrite(ledPIN , HIGH);   // and turn on the led...
    delay(2000);        // ...for 2 sec
    noTone(buzzer);     // Stop sound
    digitalWrite(ledPIN , LOW);    // and turn off the led (to make intermitent sounds and lights)
  }
    
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = (5.0-sensor_volt)/sensor_volt; // omit * RL (1000)

    ratio = RS_gas/2.70;  // ratio = RS/R0, with R0=2.70

// We compute the different concentrations using the formulas we obtained with the regressions
    lpg = int(482.67 * pow(ratio, -2.542)); 
    dihyd = int(871.81 * pow(ratio, -2.175));
    co = int(17250 * pow(ratio, -2.668));

    Serial.print("LPGs concentration (ppm): ");
    Serial.println(lpg);
    Serial.print("H2 concentration (ppm): ");
    Serial.println(dihyd);
    Serial.print("CO concentration (ppm): ");
    Serial.println(co);

    delay(2000);

}
