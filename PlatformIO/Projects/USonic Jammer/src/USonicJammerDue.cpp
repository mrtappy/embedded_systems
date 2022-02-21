
#include <stdio.h>
#include <stdlib.h>
#include <arduino.h>														//Arduino library for testing purposes.
#include <DuePWM.h>

bool duration_end {true};
long duration {0};
long start {0};

DuePWM Jammer;

void setup()
{
  Jammer.setup(40000, PWM0);
  Jammer.duty(50, PWM0);
} 
  

void loop() 
{  
    if(duration_end)
    {
      Jammer.duty(50, PWM0);
      duration_end = false;
      start = micros();
      duration = 0;
    }
    else if(duration >= 45000)
    {
      duration_end = true;
    }
    else if(duration >= 200)
    {
      Jammer.duty(0, PWM0);
    }
    
    duration = micros() - start;

}
