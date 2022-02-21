#include <Arduino.h>
#include <DuePWM.h>

DuePWM Transmitter;
long start {0};
long duration {0};
bool en_transmitter {false};

void setup() 
{
  Transmitter.setup(40300, PWM0);
  Transmitter.duty0(50);
}

void loop() 
{

}