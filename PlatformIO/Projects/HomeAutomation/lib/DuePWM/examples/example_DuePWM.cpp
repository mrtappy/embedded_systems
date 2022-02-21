//(!)(!)(!)(!)(!)(!)(!)(!)(!)ATTENTION(!)(!)(!)(!)(!)(!)(!)(!)(!)(!)
//In order to use this library properly the setup method has to be called AFTER any other arduino like 
//pin initializsations (via pinMode)
//Otherwise the setup will overwrite the settings provided by pinMode (reason unkown)

#include <Arduino.h>
#include <DuePWM.h>

DuePWM LED;
//DuePWM LED (84000000); Use this constructor to specify a custom clock value, default is 84 MHz

//This example shows the basic usage of the "DuePWM" class.
//This program lets an LED, connected to the Due's PWM channel 0 - which is pin 34 (PWML0) or 35 (PWMH0), fade.

int PWM_DUTY {0};       //variable for the duty cycle
int STATE {0};          //State variable for switch case statement

void setup()
{
    //Sets the Due's PMW channel 0 to a frequency of 1 kHz
    //The setup method also initializes the duty cycle to 0% and enables the PWM channel
    LED.setup(1000, PWM0);

    //It is also possible to setup multiple PWM Channels with the same frequency
    //LED.setup(1000, PWM0, PWM1, PWM2);

    //Enabling PWM channels
    //LED.ena0(); //enables PWM channel 0
    //LED.ena(PWM0, PWM1, PWM2); //enables PWM channel 0 to 2
    //LED.enaAll(); //enables all PWM channels
    
    //Disabling PWM channels
    //LED.dis0(); //disables PWM channel 0
    //LED.dis(PWM0, PWM1, PWM2); //enables PWM channel 0 to 2
    //LED.disAkll(PWM0, PWM1, PWM2); //disables all PWM channels

}

void loop()
{
    switch(STATE)
    {
        case 0:
        {
            //Increase PWM_DUTY until it reaches 100 then switch mode
            if(PWM_DUTY < 100)
            {
                PWM_DUTY ++;
                
                //Update the duty cycle of PWM channel 0
                LED.duty0(PWM_DUTY);

                //It is also possible to update multiple PWM channels
                //LED.duty(PWM_DUTY, PWM0, PWM1, PWM2);

            }
            else
            {
                STATE = 1;
            }
            break;
        }

        case 1:
        {
            //Decrease PWM_DUTY until it reaches 0 then switch mode
            if(PWM_DUTY > 0)
            {
                PWM_DUTY--;
                //Update the duty cycle of PWM channel 0
                LED.duty0(PWM_DUTY);

                //It is also possible to update multiple PWM channels
                //LED.duty(PWM_DUTY, PWM0, PWM1, PWM2);
            }
            else
            {
                STATE = 0;
            }
            break;
        }
    }
    
    delay(8);
}
