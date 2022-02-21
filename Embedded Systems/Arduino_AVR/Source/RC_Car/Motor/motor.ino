
  int joystick_x = A0;
  int joystick_y = A1;
  int button = 3;
  int enable = 9;
  int control1 = 2;
  int control2 = 5;
  //pin 2 & 5 für direction control

void setup() {
  // put your setup code here, to run once:
  pinMode (joystick_x, INPUT);
  pinMode (joystick_y, INPUT);
  pinMode(button, INPUT);
  pinMode(enable, OUTPUT);
  pinMode(control1, OUTPUT);
  pinMode(control2, OUTPUT);

  digitalWrite(enable, LOW);
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
  int z;
  int mtrspd; 
  
  z = x-530;
  mtrspd=z/2;

  
  Serial.print ("X-Achse:"); Serial.print (x, 4);
  Serial.print ("Y-Achse:"); Serial.print (y, 4);  Serial.print ("Speed: "); Serial.println(mtrspd);

 


  if (x > 530)
  {
     
    analogWrite(enable, mtrspd);
    digitalWrite(control1, HIGH);
    digitalWrite(control2, LOW);
    
  }
  else{
    
    analogWrite(enable, 0);
    digitalWrite(control1, LOW);
    digitalWrite(control2, LOW);
   
  }
      

}
