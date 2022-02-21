#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
#include <string.h>
#include <SPI.h>
#include <Servo.h>

  RF24 radio(7, 8); // CE,CSN
  const byte address[6] = "00001";

  Servo ser;  
  int x, y;
  float z = 0.00;
  char text[32]="0001";
 
void setup() {
  
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  
  ser.attach(9);
  ser.write(105);
  
  
}

void loop() {

  Serial.println(text);

  if(radio.available()){

    radio.read(&text, sizeof(text));

    Serial.print("Empfangen: ");
    Serial.println(text);
    
    x = atoi(text);
    
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

  }
        
}
