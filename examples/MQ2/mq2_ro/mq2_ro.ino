//This code will compute the values of Rs and R0
//Wait for about four hours to let the reading stabilize
//Run it once in clear air to measure Rs
//Run it again in H2 to measure R0

void setup() {
    Serial.begin(9600);
}

void loop() {
    float sensor_volt;
    float RS_air; // Get the value of RS via in a clear air
    float R0;  // Get the value of R0 via in H2. 
    float sensorValue; // This is the read analog value

    sensorValue = analogRead(A8);

    
  // Get a average data by testing 100 times
    for(int x = 0 ; x < 100 ; x++)
    {
        sensorValue = sensorValue + analogRead(A0);
    }

    sensorValue = sensorValue/100.0;
    sensor_volt = sensorValue/1024*5.0;

    RS_air = (5.0-sensor_volt)/sensor_volt; // omit * RL (1k)
    R0 = RS_air/9.8; // The ratio of RS/R0 is 9.8 in a clear air from Graph (Found using WebPlotDigitizer)


    Serial.print("sensor_volt = ");
    Serial.print(sensor_volt);
    Serial.println("V");

    Serial.print("Rs = ");
    Serial.print(RS_air);
    
    Serial.print("R0 = ");
    Serial.println(R0);
    delay(1000);
}
