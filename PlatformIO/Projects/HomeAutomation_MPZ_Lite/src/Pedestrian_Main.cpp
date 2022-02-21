#include <Arduino.h>
#include <Pedestrian.h>

Pedestrian walker;

float old_speed {0};                  //Store old speed value if trafficlight is red. This value will be used by the "Start" method if the traffic light turns green.
bool TrafficLight {false};            //Store the state of the traffic light. For pedestrians we only use green (true) and red (false)
bool FirstCheck {true};               //Variable to store wether the direction has already been calculated by the "AmpelCheck" state.

int arr[NumOfTurnPoints][2] = {{5000, 5000}, 
                              {10000, 5000}, 
                              {10000, 10000}, 
                              {5000, 10000}};


int STATE {0};

long duration {0};
long start {0};

void CalcPosition();
void PrintPosition();
void AmpelCheck();


void setup() 
{
  
  Serial.begin(115200);         // using serial interface for visualisation

  Serial.println("Starting...");
  walker.SetSpeed(1);
  
  
  walker.SetTurnPoints(arr);
  walker.FindStartPoints();

  Serial.println("Start Points:");
  for(int i = 0; i < NumOfStartPoints; i++)
  {
    Serial.print("X: ");
    Serial.print(walker.GetStartPoints('x', i));
    Serial.print(" ");
    Serial.print("Y: ");
    Serial.println(walker.GetStartPoints('y', i));
  }
  Serial.println();

  walker.SetNewPath();

  Serial.println("Start: ");
  Serial.println(walker.GetStartArrPos());
  Serial.print("X: ");
  Serial.print(walker.GetStart('x'));
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.println(walker.GetStart('y'));

  Serial.println("Destination: ");
  Serial.println(walker.GetDestinationArrPos());
  Serial.print("X: ");
  Serial.print(walker.GetDestination('x'));
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.println(walker.GetDestination('y'));

  //walker.SetStart(15000,6250);
  //walker.SetDestination(10000, 0);
  //walker.SetDestination(15000, 6250);
  //char direction = walker.CalcStartDirection();
  Serial.print("Direction: ");
  // Serial.println(direction);
  Serial.println(walker.GetCurrentDirection());
  
  start = millis();

  TrafficLight = true;
}

void loop() 
{

  // delay(1000);
  // walker.SetNewPath();

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

  switch(STATE)
  {
    case 0:
    {
      CalcPosition();
      if(walker.CheckDestination())
      {
        walker.SetNewPath();
        Serial.println("Start: ");
        Serial.println(walker.GetStartArrPos());
        Serial.print("X: ");
        Serial.print(walker.GetStart('x'));
        Serial.print(" ");
        Serial.print("Y: ");
        Serial.println(walker.GetStart('y'));

        Serial.println("Destination: ");
        Serial.println(walker.GetDestinationArrPos());
        Serial.print("X: ");
        Serial.print(walker.GetDestination('x'));
        Serial.print(" ");
        Serial.print("Y: ");
        Serial.println(walker.GetDestination('y'));

        Serial.print("Direction: ");
        Serial.println(walker.GetCurrentDirection());
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
      duration = millis() - start;
      
      if(duration >= 200)
      {
        start = millis();
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
  //Serial.println(walker.GetDelta('x'));
  //Serial.println(walker.GetDelta('y'));
  
  FirstCheck = true;
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
 
  if(FirstCheck)
  {
  
    if(walker.CheckPosition(10000, 10000) || walker.CheckPosition(5000, 10000) 
        || walker.CheckPosition(10000, 5000) || walker.CheckPosition(5000,5000))
    // if(walker.CheckPosition(6250, 6250) || walker.CheckPosition(8750, 6250) 
    //     || walker.CheckPosition(8750, 8750) || walker.CheckPosition(6250,8750))
    {
        walker.CalcDirection(0);
        walker.CalcDirection(1);
        walker.CalcDirection(2);
        walker.CalcDirection(3);
    }
    FirstCheck = false;
  }

  if(TrafficLight)
  {
    if(walker.GetSpeed() == 0.00)
    {
      walker.Start(old_speed);  
    }
    FirstCheck = true;
  }
  else if(TrafficLight == false)
  {
    old_speed = walker.GetSpeed();
    walker.Stop();
  }
  
}