const int llama=2;
const int timbre=3;
const int led=4;
const int llamaanalogica=A5;
bool fuego;
float fuegoanalogico;
void setup() {
  // put your setup code here, to run once:
pinMode(llama,INPUT);
pinMode(llamaanalogica,INPUT);
pinMode(timbre,OUTPUT);
pinMode(led,OUTPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  fuego=digitalRead(llama);
  fuegoanalogico=analogRead(llamaanalogica);
    Serial.println(fuego);
    Serial.println(fuegoanalogico);
if(fuego==LOW){
  analogWrite(timbre,0);
  digitalWrite(led,LOW);
  delay(500);
  digitalWrite(led,HIGH);
  delay(500);
}
  else {
  analogWrite(timbre,150);
  digitalWrite(led,HIGH);
  delay(1000);
}
}
