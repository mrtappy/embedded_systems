
#include <Servo.h>

Servo ser;  
  
  int joystick_x = A0;
  int joystick_y = A1;
  int button = 3;
  int enable = 10;
  int control1 = 6;
  int control2 = 5;         //pin 6 & 5 für direction control
  float x, y;
  int knopf;
  float z = 0.00;
  int mtrspd = 0; 
 
void setup() {
  
  pinMode (joystick_x, INPUT);
  pinMode (joystick_y, INPUT);
  pinMode(button, INPUT);
  pinMode(enable, OUTPUT);
  pinMode(control1, OUTPUT);
  pinMode(control2, OUTPUT);

  digitalWrite(enable, LOW);
  digitalWrite(button, HIGH);
  
  ser.attach(9);
  ser.write(105);
  
  Serial.begin(9600);
}

void loop() {

  x = analogRead(joystick_x);
  y = analogRead(joystick_y);
  knopf = digitalRead(button);
  mtrspd = 0;   



        //Servosteuerung

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

        //Motorsteuerung

        mtrspd = 255;
        analogWrite(enable, mtrspd);
        digitalWrite(control1, HIGH);
        digitalWrite(control2, LOW);
                
       /* if (y > 530)
        {
          mtrspd = 0.25*y;
          analogWrite(enable, mtrspd);
          digitalWrite(control1, HIGH);
          digitalWrite(control2, LOW);
          
        }
        else if(y < 500)
        {
          mtrspd = map(y, 500, 0, 0, 255);
          analogWrite(enable, mtrspd);
          digitalWrite(control1, LOW);
          digitalWrite(control2, HIGH);
        }
        else{
          
          analogWrite(enable, 0);
          digitalWrite(control1, LOW);
          digitalWrite(control2, LOW);

        }    */        

}
