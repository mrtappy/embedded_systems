#include <Arduino.h>
#define Eingang 8
#define Ausgang 9

int maximumRange = 300;
int minimumRange = 2;
long Abstand;
float s;
long Dauer;
int x = 0;

void setup() {
  pinMode(Ausgang, OUTPUT);
  pinMode(Eingang, INPUT);
  Serial.begin (9600);
}

void loop() {

  digitalWrite(Ausgang, HIGH);
  delayMicroseconds(10);
  digitalWrite(Ausgang, LOW);

  x = pulseIn(Eingang, HIGH);
  s = x/56.2;
   
  Serial.print(s);
  Serial.println("cm");    
  
  delay(10);

}