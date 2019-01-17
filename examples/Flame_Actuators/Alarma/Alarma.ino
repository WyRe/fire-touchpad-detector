const int llama=2;
const int timbre=3;
const int led=4;
bool fuego;
void setup() {
  // put your setup code here, to run once:
pinMode(llama,INPUT);
pinMode(timbre,OUTPUT);
pinMode(led,OUTPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  fuego=digitalRead(llama);
if(fuego==LOW){
  Serial.print(fuego);
  analogWrite(timbre,0);
  digitalWrite(led,LOW);
  delay(500);
  digitalWrite(led,HIGH);
  delay(500);
}
  else {
  Serial.print(fuego);
  analogWrite(timbre,150);
  digitalWrite(led,HIGH);
  delay(1000);
}
}
