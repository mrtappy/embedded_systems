//This is an example for the usage of the "Bewegung" class and the "Vehicle" class.
//To show case the intended usage a simple state machine (via switch case) is used.

#include <Arduino.h>
#include <Vehicle.h>

Vehicle car;

//Define the positions of the turn points
int arr[NumOfTurnPoints][2] =  {{6250, 6250},
                                {8750, 6250},
                                {8750, 8750},
                                {6250, 8750}};

//Define the positions of the traffic lights
int arrTrafficLightts[NumOfTrafficLights][2] = {{6250, 5000},
                                                {10000, 6250},
                                                {8750, 10000},
                                                {5000, 8750}};

int STATE {0};

//For better visibility, see below
long duration {0};
long start {0};

//Function prototypes
void CalcPosition();
void PrintPosition();
void AmpelCheck();

void setup() 
{
  
  Serial.begin(115200);         // using serial interface for visualisation

  //Vehicle Object setup
  car.SetSpeed(1);
  car.SetTurnPoints(arr);                     //Copy turnpoints array to object
  car.SetTrafficLights(arrTrafficLightts);    //Copy traffic light array to object
  car.FindStartPoints();                      //Find all possible start points for the vehicle object
  car.MapStartDestinationPoints();            //Find all possible destinations for every start point and map them
  car.SetNewPath();                           //Choose a random start & destination point
  
  //Visualisation for debugging-------------------
  Serial.println("Start: ");
  Serial.println(car.GetStartPoint());
  Serial.print("X: ");
  Serial.print(car.GetStart('x'));
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.println(car.GetStart('y'));

  Serial.println();

  Serial.println("Destination: ");
  Serial.println(car.GetDestinationPoint());
  Serial.print("X: ");
  Serial.print(car.GetDestination('x'));
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.println(car.GetDestination('y'));
  //----------------------------------------------

  //Timed output for better visibility (see below)
  start = millis();
}

void loop() 
{
  
  switch(STATE)
  {
    case 0:
    {
      CalcPosition();

      //If the car has reached its destination a new random path should be set!
      if(car.CheckDestination())
      {
        car.SetNewPath();

        //Visualisation------------------------------
        Serial.println(car.GetCurrentDirection());
        Serial.println("Start: ");
        Serial.print("X: ");
        Serial.print(car.GetStart('x'));
        Serial.print(" ");
        Serial.print("Y: ");
        Serial.println(car.GetStart('y'));
        Serial.println("Destination: ");
        Serial.print("X: ");
        Serial.print(car.GetDestination('x'));
        Serial.print(" ");
        Serial.print("Y: ");
        Serial.println(car.GetDestination('y'));
        //-----------------------------------------

        break;
      }
  
      STATE = 1;
      break;
    }
    
    case 1:
    {
      AmpelCheck();
      STATE = 2;
      break;
    }

    case 2:
    {
      //Only print the position every 200 ms to the console (for better visibility)
      duration = millis() - start;
      
      if(duration >= 200)
      {
        start = millis();     //reset "start" time 
        PrintPosition();
      }
      STATE = 0;
      break;
    }
  }

}

void CalcPosition()
{
  car.CalcPosition();
}

void PrintPosition()
{
  Serial.print("Current position X: ");
  Serial.print(car.GetCurrentPos('x'));
  Serial.print(" Y: ");
  Serial.println(car.GetCurrentPos('y'));
}

void AmpelCheck()
{
  if(car.CheckPosition(6250, 6250) || car.CheckPosition(8750, 6250) 
    || car.CheckPosition(8750, 8750) || car.CheckPosition(6250,8750))
  {
    car.CalcDirection(0);
    car.CalcDirection(1);
    car.CalcDirection(2);
    car.CalcDirection(3);
    
  }
}