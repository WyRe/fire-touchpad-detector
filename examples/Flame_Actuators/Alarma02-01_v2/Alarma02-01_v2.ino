//El objetivo del código es que si el sensor analógico
// y digital de llama no detecta fuego, el led se enciede
//  y se apaga cada medio segundo para indicar que todo
//va bien. Si el detector analógico o digital detecta fuego, 
// suena el timbre y el led permanece encendido. Si el
// sensor no detecta llama analógica o digitalmente
// se apaga el timbre y el led vuelve a parpadear.
//Le doy nombres a los canales de entrada y salida.
// flame es la salida digital del sensor, entrada digital
// en el arduino.
const int flame=2;
// buzz es la señal analógica que sale del arduino hacia el
// timbre.
const int buzz=3;
// led es la señal digital que sale del arduino y enciende
// y apaga el led.
const int led=4;
// analogflame es la salida analógica del sensor, entrada analógica
// en el arduino.
const int analogflame=A5;
// fire es la variable booleana que nos dice si hay o no fuego.
// El sensor da un cero cuando detecta fuego y 1 cuando no.
bool fire;
//humidity es una constante que guarda el puerto digital en el
// que se obtiene el valor digital de la humedad.
const int dhumidity=5;
// motor es el puerto lleva la señal digital al motor.
const int motor=6;
// humidity es una variable real que guarda el valor medido de
//la humedad.
float humidity;
// analogfire es una variable real que nos dice si hay fuego
// cerca o no. Creo que está relacionado con longitud
// de onda. Considero que vale menos de 100, hay
// fuego cerca.
float analogfire;
void setup() {
  // put your setup code here, to run once:
pinMode(flame,INPUT);
pinMode(dhumidity,INPUT);
pinMode(analogflame,INPUT);
pinMode(buzz,OUTPUT);
pinMode(motor,OUTPUT);
pinMode(led,OUTPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Guardamos en humidity la lectura digital del 
  // sensor en humedad.
  humidity=digitalRead(dhumidity);
  //Escalo el valor de la humedad relativa. 0=20%
// humedad relativa y 254 es 90% de humedad relativa.
humidity=20.0+70*humidity/254.0;
  //Guardamos en fire la lectura digital del 
  // sensor en fire.
  fire=digitalRead(flame);
  //Guardamos en analogfire la lectura analógica del 
  // sensor en fire.
  analogfire=analogRead(analogflame);
  //Mostramos las medidas por pantalla.
    Serial.println(fire);
    Serial.println(analogfire);
  //Si fire=1 o analogfire > 100 no hay fuego.
  // Apagamos el timbre y hacemos parpadear el led.  
if(fire==HIGH or analogfire >= 100){
  analogWrite(buzz,0);
  digitalWrite(led,LOW);
  delay(500);
  digitalWrite(led,HIGH);
  delay(500);
}
// Si no, el led y el timbre se quedan encendidos y se
// activa la bomba de agua.
  else {
  analogWrite(motor,254);
  analogWrite(buzz,150);
  digitalWrite(led,HIGH);
  delay(1000);
}
// Si no hay fuego pero la humedad es baja activamos
// la bomba de agua a baja potencia para que aumente
// la humedad sin inundar.
if(fire==HIGH and humidity <= 25){
  analogWrite(motor,10);
  digitalWrite(led,LOW);
  delay(200);
  digitalWrite(led,HIGH);
  delay(200);
}

}
////////////////////////////////////
