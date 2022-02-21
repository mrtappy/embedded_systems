#include <Arduino.h>
#include <MeasureClass.h>


//This example shows the most basic use case of a ultrasonic distance sensor
//In this implementation the sensor sends a ultrasonic pulse and waits for a reflection (by an obstacle)
//If a reflected signal is sensed the distance to the obstacle is calculated

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

    Serial.println(Measure.GetEnvTemperature());
    Serial.println(Measure.GetAirSpeed());
}

void loop()
{

    BreakMeasure = false;
    //Starts the ultrasonic transmitter with default transmission duration (150u)
    //Marks the measurement start and stores the current time (via micros()) in "TransmissionStart"
    //The Start method also handels all necessary resets after a measurement period elapsed
    Measure.Start();     

    while(Measure.End() == false)
    {
        //The End method checks the duration of the transmission and stops the transmission after  
        //after the specified duration exceeded
        //The Method returns true after a specified delay (defaulg 2500u) has passed
        //This delay is necessary due to transmitter interference in the receiving sensor
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
        Measure.EvalObstacle();     //Calculates the distance and the max amplitude in volts
        Serial.println(Measure.GetDistance());
    }
    delay(100); //This delay is necessary since the oscillation on the receiver needs time to decay. The delay can be reduced!
    
}
