#define Eingang A0


void setup() 
{
  pinMode(Eingang, INPUT);
  Serial.begin (9600);

}

void loop() {
  int x = 0;

  x = analogRead(A0);

  Serial.println(x);

  delay(10);

}
