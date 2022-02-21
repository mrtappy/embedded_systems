#include <Arduino.h>
#include <MeasureClass.h>

//This example shows the basic usage of the "USonicMeasure" class 
//In this example the implemenation of an ultrasonic "environment scan" is shown
//In this case the implementation is based on a simple state machine using the switch case statement

//Default initialization is handeled by the constructor(s) of the USonicMeasure class!
//USonicMeasure Measure(25.00);     //Creates Measure object with custom temperature value 
USonicMeasure Measure;   //Creates Measure object with default values (see MeasureClass.h)
int STATE {0}; //stores the state for the state machine

long MeasureDelay {2500};
long MeasureDelayDuration {0};
long TransmissionEnd {0};



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
    
    switch(STATE)
    {
        case 0:
        {
            //The first state of our simple state machine is the start of a measurement
            //The measurement is started by the transmission of a ultrasonic pulse
            Serial.println('\n');
            Measure.Start();
            STATE = 1;
            MeasureDelayDuration = 0;
            break;
        }

        case 1:
        {
            //The second state is active until the specified transmission duration and delay passed
            //The default transmission duration 150 us
            //The default delay between transmission and actual measurement is 2500 us
            
            if(Measure.End())
            {
                STATE = 2;
                TransmissionEnd = micros();
            }
            break;
        }

        case 2:
        {
            //In this state the mcu waits for the detection of an obstacle
            //If the max measure duration passed a new measurement is started
            if(Measure.Duration() >= Measure.GetMaxMeasureDuration())
            {
                STATE = 0;
            }
            else
            {
                if(Measure.ObstacleDetect())
                {
                    STATE = 3;
                    //break;
                }
            }
            break;
        }

        case 3:
        {   
            //In this state the max amplitude of the reflected signal is evaluated
            //If the max measure duration passed a new measurement is started
            if(Measure.Duration() >= Measure.GetMaxMeasureDuration())
            {
                STATE = 0;
            }
            else
            {
                if(Measure.FindMax())
                {
                    STATE = 4;
                }
            }
            break;
        }

        case 4:
        {
            Measure.EvalObstacle(); //Distance (in cm) and amplitude (in volts) calculation
            Measure.SerialOut();    //Prints the distance and amplitude to the console


            //If the max measure duration passed a new measurement is started
            //Otherwise the scan continues
            if(Measure.Duration() >= Measure.GetMaxMeasureDuration())
            {
                STATE = 0;
            }
            else 
            {
                STATE = 2;
            }
            break;
        }

        default:
        {
            break;
        }

    }
    
    


}
