#include <Arduino.h>
#include <DuePWM.h>

DuePWM Transmitter;
long start {0};
long transmitter_start {0};
long duration {0};
bool en_transmitter {false};

void setup() 
{
  Transmitter.setup(40300, PWM0);
  start = micros();
}

void loop() 
{
  duration = micros() - start;
  
  
  if(duration < 75 && en_transmitter == false)
  {
    Transmitter.duty0(50);
    start = micros();
    en_transmitter = true;
  }

  else if(duration >= 100000)
  {
    start = micros();
  }

  else if(duration >= 462)
  {
    Transmitter.duty0(0);
    en_transmitter = false;
  }

  else if(duration >= 412)
  {
    Transmitter.duty0(50);
    en_transmitter = true;
  }

  else if(duration >= 80 && en_transmitter)
  {
    Transmitter.duty0(0);
    en_transmitter = false;
  }
}