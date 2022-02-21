
  int joystick_x = A0;
  int joystick_y = A1;
  int button = 3;
  const int control1 = 2;
  const int control2 = 5;
  const int enable = 9;
  int motoren = 0;
  int motordr = 1;
  int motorsp = 0;
  //pin 2 & 5 für speed control

void setup() {
  // put your setup code here, to run once:
  pinMode (joystick_x, INPUT);
  pinMode (joystick_y, INPUT);
  pinMode(button, INPUT);
  pinMode(control1, OUTPUT);
  pinMode(control2, OUTPUT);
  pinMode(enable, OUTPUT);

  digitalWrite(enbable, LOW);
  digitalWrite(button, HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float x, y;
  int knopf;
  
  x = analogRead(joystick_x);
  y = analogRead(joystick_y);
  knopf = digitalRead(button);

  if(knopf=1)
  {
    Serial.println("nicht gedrueckt");
    }
   else
   {
    Serial.println("gedrueckt");
    }
    delay(500);

  if (x > 501)
    motoren = 1;