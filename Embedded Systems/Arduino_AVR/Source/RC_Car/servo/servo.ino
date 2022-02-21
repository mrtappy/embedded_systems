
#include <Servo.h>

Servo ser;  
  
  int joystick_x = A0;
  int joystick_y = A1;
  int button = 3;
  

void setup() {
  // put your setup code here, to run once:
  pinMode (joystick_x, INPUT);
  pinMode (joystick_y, INPUT);
  pinMode(button, INPUT);
  
  ser.attach(9);
  
  digitalWrite(button, HIGH);
  ser.write(105);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float x, y;
  int knopf;
  float z = 0.00;
  x = analogRead(joystick_x);
  y = analogRead(joystick_y);
  knopf = digitalRead(button);

 
  if (x > 533)
  {
      //z=((x-523)*0.36);
      z=154-(9*x/98);
      ser.write(z);
  }
  else if(x<490){

    z=140-(x/14);
    ser.write(z);
  }
  else
  {
    ser.write(105);
  }
  

   
  Serial.println(x);
  


}

