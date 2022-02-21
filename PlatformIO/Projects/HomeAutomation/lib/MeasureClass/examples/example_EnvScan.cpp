#include <Arduino.h>
#include <MeasureClass.h>

//This example shows the basic usage of the "USonicMeasure" class 
//In this example the implemenation of an ultrasonic "environment scan" is shown
//Note: traditional ultrasonic distance measurements only search for the first obstacle 
//and start a new measurement after the first obstacle has been detected (e.g. HC SR04)

//Default initialization is handeled by the constructor(s) of the USonicMeasure class!
//USonicMeasure Measure(25.00);     //Creates Measure object with custom temperature value 
USonicMeasure Measure;   //Creates Measure object with default values (see MeasureClass.h)
bool BreakMeasure {false}; //This variable is used to check if the duration of a measurement period has been exceeded 


void setup()
{
    Serial.begin(115200);
    //Setup for the "detect pin" which is used to read the comparator output
    pinMode(Measure.GetDetectPin(), INPUT);    //Default is 49

    //Setup for the "detect pin" which is used to trigger the arduino nano, 
    //which in turn starts the 40 kHz PWM for the transmitter
    pinMode(Measure.GetTriggerPin(), OUTPUT);

    digitalWrite(Measure.GetTriggerPin(), LOW); //Make sure trigger is low at start up
}

void loop()
{
    BreakMeasure = false;
    
    //A new ultrasonic pulse is only transmitted after the "MaxMeasureDuraion" exceeded
    //During the measurement duration the comparator output is evalated during every loop
    //This implements an enviroment scan for obstacles
    if(Measure.Duration() > Measure.GetMaxMeasureDuration())
    {
        //Starts the ultrasonic transmitter with default transmission duration (150u)
        //Marks the measurement start and stores the current time (via micros()) in "TransmissionStart"
        //The Start method also handels all necessary resets after a measurement period elapsed
        //delay(100);
        Serial.println('\n');
        Measure.Start();     

        while(Measure.End() == false)
        {
            //The End method checks the duration of the transmission and stops the transmission after  
            //after the specified duration exceeded
            //The Method returns true after a specified delay (defaulg 2500u) has passed
            //This delay is necessary due to transmitter interference in the receiving sensor
        }
    }

    

    while(Measure.ObstacleDetect() == false)
    {
        //ObstacleDetect returns trun if the comparator output is high (Obstacle detected).
        //The elapsed time (micros() - TransmissionStart) is store in "TimeToObstacle".

        if(Measure.Duration() >= Measure.GetMaxMeasureDuration())
        {
            //The second statement is used to prevent a never ending loop if no obstacle is detected
            //The default value for "MaxMeasureDuration" is 45000u, 
            //which results in a max measureable distance of 7.5 m.
            BreakMeasure = true;
            break;
        }        
    }

    if(!BreakMeasure)
    {
        while(Measure.FindMax() == false)
        {
            //Searches for the max amplitude in the reflected signal for the duration 
            //specified in "MaxFindMaxDuration". The defualt "read pin" for the ADC is A0.
            //The max value read is then stored in "ReadADC0".
        }

        Measure.EvalObstacle();     //Calculates the distance and the max amplitude in volts
        Measure.SerialOut();
    }

}
