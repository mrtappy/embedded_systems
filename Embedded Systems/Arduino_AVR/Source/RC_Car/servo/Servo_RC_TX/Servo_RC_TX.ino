#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
#include <string.h>
#include <stdlib.h>

#include <SPI.h>


  RF24 radio(7, 8); // CE,CSN
  int joystick_x = A0;
  int joystick_y = A1;
  int button = 3;
  int x, y;
  int knopf;
  float z = 0.00;
  const byte address[6] = "00001";
  char pos[32] ="Hello from the other side";


void setup() {
  
  pinMode (joystick_x, INPUT);
  pinMode (joystick_y, INPUT);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  Serial.begin(9600);
}

void loop() {

  x = analogRead(joystick_x);
  y = analogRead(joystick_y);

  Serial.println(pos);

  itoa(x, pos, 10);
  Serial.println(pos);


  radio.write(pos, sizeof(pos));

  delay(1);

}
