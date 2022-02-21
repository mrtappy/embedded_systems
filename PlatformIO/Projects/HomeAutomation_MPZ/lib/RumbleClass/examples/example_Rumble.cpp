//(!)(!)(!)(!)(!)(!)(!)(!)(!)ATTENTION(!)(!)(!)(!)(!)(!)(!)(!)(!)(!)
//In order to use the DuePWM library properly the setup method has to be called AFTER any other arduino like 
//pin initializsations (via pinMode)
//Otherwise the setup will overwrite the settings provided by pinMode (reason unkown)


#include <Arduino.h>
#include <RumbleClass.h>
#include <MeasureClass.h>

//This example shows the basic usage / application of the RumbleClass.
//NOTE: The RumbleClass is derived from the DuePWM class.
//This means that every method and variable from the DuePWM class is also vailable in the RumbleClass! 
//For demonstration purposes the basic MeasureClass example is included here!

//The Intention of the RumbleClass is to control a series of rumble motors based on a distance input (e.g. distance to an obstacle)
//The default initializsation of the RumbleClass consists of 4 distance sectors:
//Sector0: min_distance (default: 50 cm) to sector_max0 (default: 150 cm)
//Sector1: sector_max0 to sector_max1 (default: 250 cm)
//Sector2: sector_max1 to sector_max2 (default: 350 cm)
//Sector3: sector_max2 to sector_max3 (default: 450 cm)
//Every Sector is attached to a PWM channel of the Arduino Due by the "SetSecPWMch" method.
//The default attachment is: Sector0 to PWM3, Sector1 to PWM2, Sector2 to PWM1, Sector3 to PWM0.

//The aim of this example is to implement a simple distance sensor with a haptic feedback (provided by the rumble motors). 
//NOTE: This example uses the most basic implementation of a distance sensor which only searches for the first(!) obstacle
//The program works as follows:
//If a obstacle is detected in sector0, the PWM channel assigned to this sector (and by that the rumble motor) is activated.
//The duty cycle is then determined by the relative position of the obstacle within the sector (100% on lower bound, 0% on upper bound).

//void PWM_Setup();

Rumble MotorControl; //create a Rumble object
int distance_obstacle {0};

//Import from MeasureClass
USonicMeasure Measure;
bool BreakMeasure {false};

bool MeasureDetect {false};


void setup()
{
    Serial.begin(115200);
    //PWM_Setup();
    //MotorControl.setup(1000, PWM0, PWM1, PWM2, PWM3); //Same setup method as in the DuePWM class

    //The SetSecMax method is used to change the max value of the sectors 
    //If the max values are changed the (linear) functions which are used to calculate the duty cycle based on the distance are also updated
    //An update of the linear function parameters (slope and offset) can be prevented by including "false" after in the max value 
    //in the function paramerter list
    MotorControl.SetSecMax(SEC0, 100);
    MotorControl.SetSecMax(SEC1, 200);
    MotorControl.SetSecMax(SEC2, 300);
    MotorControl.SetSecMax(SEC3, 400);
    //MotorControl.SetSecMax(SEC0, 100, false); //Include "false" in the parameter list to prevent an update of "sector_slope" and "sector_offset"

    //import from MeasureClass example
    pinMode(Measure.GetDetectPin(), INPUT);
    pinMode(Measure.GetTriggerPin(), OUTPUT);
    digitalWrite(Measure.GetTriggerPin(), LOW);

    Serial.println(Measure.GetEnvTemperature());
    Serial.println(Measure.GetAirSpeed());

    //PWM_Setup();
    MotorControl.setup(1000, PWM0, PWM1, PWM2, PWM3); //Same setup method as in the DuePWM class

}

void loop()
{
    MeasureDetect = false;
    BreakMeasure = false;
    Measure.Start();     
    
    while(Measure.End() == false)
    {
        //Import from MeasureClass example
    }

    while(Measure.ObstacleDetect() == false)
    {
        //Serial.println("Waiting");
        //Import from MeasureClass example
        if(Measure.Duration() >= Measure.GetMaxMeasureDuration())
        {
            BreakMeasure = true;
            break;
        }        
    }

    if(BreakMeasure != true)
    {
        Measure.EvalObstacle();     //Calculates the distance and the max amplitude in volts        
        Serial.println(Measure.GetDistance());

        //Read the distance measured by the sensor
        distance_obstacle = Measure.GetDistance();

        //Evaluate in which sector the obstacle is and set the rumble motors accordingly
        if(distance_obstacle <= MotorControl.GetMinDistance())
        {
            //Disables all the other PWM channels
            MotorControl.dis(SEC1, SEC2, SEC3);

            //Enables the PWM channel for SEC0
            MotorControl.ena(SEC0);
            
            //Calculates and updates the duty cycle of the PWM channel which is linked to the specified sector
            MotorControl.duty(100, SEC0);
        }
        else if(distance_obstacle < MotorControl.GetSecMax(SEC0))
        {
            //Disables all the other PWM channels
            MotorControl.dis(SEC1, SEC2, SEC3);
            
            //Enables the PWM channel for SEC0
            MotorControl.ena(SEC0);

            //Calculates and updates the duty cycle of the PWM channel which is linked to the specified sector
            MotorControl.CalcDuty(distance_obstacle, SEC0);
            
        }
        else if(distance_obstacle < MotorControl.GetSecMax(SEC1))
        {
            //Disables the PWM channels linked to the specified sectors
            MotorControl.dis(SEC0, SEC2, SEC3);
            
            //Enables the PWM channels for the specified sectors
            MotorControl.ena(SEC1);

            //Calculates and updates the duty cycle of the PWM channel which is linked to the specified sector
            MotorControl.CalcDuty(distance_obstacle, SEC1);
        }
        else if(distance_obstacle < MotorControl.GetSecMax(SEC2))
        {
            //Disables the PWM channels linked to the specified sectors
            MotorControl.dis(SEC0, SEC1, SEC3);
            
            MotorControl.ena(SEC2);

            //Calculates and updates the duty cycle of the PWM channel which is linked to the specified sector
            MotorControl.CalcDuty(distance_obstacle, SEC2);
        }
        else if(distance_obstacle < MotorControl.GetSecMax(SEC3))
        {
            //Disables the PWM channels for the specified sectors
            MotorControl.dis(SEC0, SEC1, SEC2);
            
            //Enables the PWM channels for the specified sectors
            MotorControl.ena(SEC3);

            //Calculates and updates the duty cycle of the PWM channel which is linked to the specified sector
            MotorControl.CalcDuty(distance_obstacle, SEC3);
        }
        else
        {
            //Nothing
            //Obstacle outside of specified range
        }
        
    }
    

    delay(100);

}