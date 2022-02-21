#include <Arduino.h>
#include <DuePWM.h>

DuePWM sync;


void setup() 
{
  sync.setup(40000, PWM0); 
  sync.duty(50, PWM0);
}

void loop() 
{
  

}