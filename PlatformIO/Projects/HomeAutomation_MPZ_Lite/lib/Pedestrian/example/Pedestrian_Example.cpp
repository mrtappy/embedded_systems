//This is an example for the usage of the "Bewegung" class and the "Pedestrian" class.
//To show case the intended usage a simple state machine (via switch case) is used.

#include <Arduino.h>
#include <Pedestrian.h>

Pedestrian walker;

//Defines the turn points 
int arr[NumOfTurnPoints][2] = {{5000, 5000},          
                              {10000, 5000}, 
                              {10000, 10000}, 
                              {5000, 10000}};


int STATE {0};          

//Used for improved visibility, see below!
long duration {0};
long start {0};

//Function prototypes
void CalcPosition();
void PrintPosition();
void AmpelCheck();

void setup() 
{
  
  Serial.begin(115200);         // using serial interface for visualisation

  //Setup of the pedestrian object
  walker.SetSpeed(1);
  walker.SetTurnPoints(arr);            //Copy the turn points to the object
  walker.FindStartPoints();             //Find and store all possible start points (= all possible destination points)

  //Visualistaion----------------------------------------
  // Serial.println("Start Points:");
  // for(int i = 0; i < NumOfStartPoints; i++)
  // {
  //   Serial.print("X: ");
  //   Serial.print(walker.GetStartPoints('x', i));
  //   Serial.print(" ");
  //   Serial.print("Y: ");
  //   Serial.println(walker.GetStartPoints('y', i));
  // }
  // Serial.println();
  //------------------------------------------------------

  walker.SetNewPath();

  //Visualistaion----------------------------------------
  // Serial.println("Start: ");
  // Serial.println(walker.GetStartArrPos());
  // Serial.print("X: ");
  // Serial.print(walker.GetStart('x'));
  // Serial.print(" ");
  // Serial.print("Y: ");
  // Serial.println(walker.GetStart('y'));

  // Serial.println("Destination: ");
  // Serial.println(walker.GetDestinationArrPos());
  // Serial.print("X: ");
  // Serial.print(walker.GetDestination('x'));
  // Serial.print(" ");
  // Serial.print("Y: ");
  // Serial.println(walker.GetDestination('y'));
  
  // Serial.print("Direction: ");
  // Serial.println(walker.GetCurrentDirection());
  //-----------------------------------------------------
  
  //For improved visibility in the console, see below!
  start = millis();

}

void loop() 
{
  switch(STATE)
  {
    case 0:
    {
      CalcPosition();     //Calculate the current position of the object
      if(walker.CheckDestination())     
      {
        //if the object has reached its destination choose a new random path
        walker.SetNewPath();

        //Visualisation---------------------------------
        // Serial.println("Start: ");
        // Serial.println(walker.GetStartArrPos());
        // Serial.print("X: ");
        // Serial.print(walker.GetStart('x'));
        // Serial.print(" ");
        // Serial.print("Y: ");
        // Serial.println(walker.GetStart('y'));

        // Serial.println("Destination: ");
        // Serial.println(walker.GetDestinationArrPos());
        // Serial.print("X: ");
        // Serial.print(walker.GetDestination('x'));
        // Serial.print(" ");
        // Serial.print("Y: ");
        // Serial.println(walker.GetDestination('y'));

        // Serial.print("Direction: ");
        // Serial.println(walker.GetCurrentDirection());
        //-------------------------------------------------
        break;          //If the object has reached its destination recalculate the position (stay in state 0)
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
      duration = millis() - start;
      
      //Only print the position to the console every 200 ms (for improved readability)
      if(duration >= 200)
      {
        start = millis();       //Reset the start time
        PrintPosition();
      }
      
      //PrintPosition();
      STATE = 0;
      break;
    }
  }

}

void CalcPosition()
{
  walker.CalcPosition();
}

void PrintPosition()
{
  Serial.print("Current position X: ");
  Serial.print(walker.GetCurrentPos('x'));
  Serial.print(" Y: ");
  Serial.println(walker.GetCurrentPos('y'));
}

void AmpelCheck()
{
  if(walker.CheckPosition(10000, 10000) || walker.CheckPosition(5000, 10000) 
    || walker.CheckPosition(10000, 5000) || walker.CheckPosition(5000,5000))  
  {
        walker.CalcDirection(0);
        walker.CalcDirection(1);
        walker.CalcDirection(2);
        walker.CalcDirection(3);
  } 
}