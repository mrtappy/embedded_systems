#include <Arduino.h>
#include <DuePWM.h>

DuePWM Transmitter;
long start {0};
long duration {0};
bool en_transmitter {false};

void setup() 
{
  Transmitter.setup(40000, PWM0);
  Transmitter.duty0(50);
}

void loop() 
{
  Transmitter.duty0(50);
  delayMicroseconds(75);
  Transmitter.duty0(0);

  
  delayMicroseconds(282);
  Transmitter.duty0(50);
  delayMicroseconds(50);
  Transmitter.duty0(0);
  delayMicroseconds(25);
  Transmitter.duty0(50);
  delayMicroseconds(75);
  Transmitter.duty0(0);
  

  delayMicroseconds(100000);
}