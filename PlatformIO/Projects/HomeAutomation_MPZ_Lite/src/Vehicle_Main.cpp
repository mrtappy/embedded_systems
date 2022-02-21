#include <Arduino.h>
#include <Vehicle.h>

Vehicle walker;

float old_speed {0};                  //Store old speed value if trafficlight is red. This value will be used by the "Start" method if the traffic light turns green.
bool TrafficLight {false};            //Store the state of the traffic light. For pedestrians we only use green (true) and red (false)
bool FirstCheck {true};               //Variable to store wether the direction has already been calculated by the "AmpelCheck" state.

//int arr[NumOfTurnPoints][2] = {{5000, 5000}, {10000, 5000}, {10000, 10000}, {5000, 10000}};

int arr[NumOfTurnPoints][2] =  {{6250, 6250},
                                {8750, 6250},
                                {8750, 8750},
                                {6250, 8750}};

int arrTrafficLightts[NumOfTrafficLights][2] = {{6250, 5000},
                                                {10000, 6250},
                                                {8750, 10000},
                                                {5000, 8750}};

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
  
  //walker.SetStart(5000, 15000);
  //walker.SetStart(15000,6250);
  walker.SetTurnPoints(arr);
  walker.SetTrafficLights(arrTrafficLightts);
  
  // walker.SetDestination(10000, 0);
  // walker.SetDestination(15000, 6250);
  // char direction = walker.CalcStartDirection();
  // Serial.print("Direction: ");
  // Serial.println(direction);
  
  walker.FindStartPoints();
  walker.MapStartDestinationPoints();
  walker.SetNewPath();
  //walker.SetStart(0);
  //walker.CalcDestinationPoints(0);

  //walker.SetDestination(15000, 6250);
  // char direction = walker.CalcStartDirection();
  // Serial.print("Direction: ");
  // Serial.println(direction);

  Serial.println("Start: ");
  Serial.println(walker.GetStartPoint());
  Serial.print("X: ");
  Serial.print(walker.GetStart('x'));
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.println(walker.GetStart('y'));

  Serial.println();

  Serial.println("Destination: ");
  Serial.println(walker.GetDestinationPoint());
  Serial.print("X: ");
  Serial.print(walker.GetDestination('x'));
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.println(walker.GetDestination('y'));

  start = millis();

  //walker.CalcCoords();
  // Serial.println();
  // Serial.print("Coord1: ");
  // Serial.println(walker.GetCoords(1));
  // Serial.print("Coord2: ");
  // Serial.println(walker.GetCoords(2));

  // Serial.println();
  // Serial.print("Start0: ");
  // Serial.print("X: ");
  // Serial.print(walker.GetStartPoints('x', 0));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetStartPoints('y', 0));
  // Serial.print("Start1: ");
  // Serial.print("X: ");
  // Serial.print(walker.GetStartPoints('x', 1));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetStartPoints('y', 1));
  // Serial.print("Start2: ");
  // Serial.print("X: ");
  // Serial.print(walker.GetStartPoints('x', 2));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetStartPoints('y', 2));
  // Serial.print("Start3: ");
  // Serial.print("X: ");
  // Serial.print(walker.GetStartPoints('x', 3));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetStartPoints('y', 3));

  // Serial.println();
  // Serial.println(walker.GetTrafficLightMin('x'));
  // Serial.println(walker.GetTrafficLightMax('x'));
  // Serial.println(walker.GetTrafficLightMin('y'));
  // Serial.println(walker.GetTrafficLightMax('y'));
  

  //walker.FindNextTrafficLight();

  // Serial.println();
  // Serial.print("First traffic light: ");
  // Serial.println(walker.GetNextTrafficLights(0));
  // Serial.print("X: ");
  // Serial.print(walker.GetTrafficLights('x', walker.GetNextTrafficLights(0)));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetTrafficLights('y', walker.GetNextTrafficLights(0)));

  // Serial.print("Second traffic light: ");
  // Serial.println(walker.GetNextTrafficLights(1));
  // Serial.print("X: ");
  // Serial.print(walker.GetTrafficLights('x', walker.GetNextTrafficLights(1)));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetTrafficLights('y', walker.GetNextTrafficLights(1)));
  // Serial.print("Third traffic light: ");
  // Serial.println(walker.GetNextTrafficLights(2));
  // Serial.print("X: ");
  // Serial.print(walker.GetTrafficLights('x', walker.GetNextTrafficLights(2)));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetTrafficLights('y', walker.GetNextTrafficLights(2)));

  // Serial.println();
  // Serial.println("Destination: ");
  // for(int i = 0; i < NumOfDestinationPoints; i++)
  // {
  //   Serial.print("X: ");
  //   Serial.print(walker.GetDestinationPoints('x', i));
  //   Serial.print(" Y: ");
  //   Serial.println(walker.GetDestinationPoints('y', i));
  // }

  // Serial.println();
  // Serial.println("Mapping: ");
  // for(int i = 0; i < NumOfStartPoints; i++)
  // {
  //   for(int j = 0; j < NumOfMappingsPerPoint+1; j++)
  //   {
  //     Serial.print(walker.GetMapping(i,j));
  //     Serial.print(" ");
  //   }
  //   Serial.println();
    
  // }
  
  // Serial.print("X: ");
  // Serial.print(walker.GetDestinationPoints('x', 0));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetDestinationPoints('y', 0));
  // Serial.print("Destination1: ");
  // Serial.print("X: ");
  // Serial.print(walker.GetDestinationPoints('x', 1));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetDestinationPoints('y', 1));
  // Serial.print("Destination2: ");
  // Serial.print("X: ");
  // Serial.print(walker.GetDestinationPoints('x', 2));
  // Serial.print(" Y: ");
  // Serial.println(walker.GetDestinationPoints('y', 2));


  // Serial.println();
  // Serial.println(walker.GetDeltaTrafficLights(walker.GetNextTrafficLights(0)));
  // Serial.println(walker.GetDeltaTrafficLights(walker.GetNextTrafficLights(1)));
  // Serial.println(walker.GetDeltaTrafficLights(walker.GetNextTrafficLights(2)));

  TrafficLight = true;
}

void loop() 
{
  
  switch(STATE)
  {
    case 0:
    {
      CalcPosition();
      if(walker.CheckDestination())
      {
        walker.SetNewPath();
        Serial.println(walker.GetCurrentDirection());
        Serial.println("Start: ");
        Serial.print("X: ");
        Serial.print(walker.GetStart('x'));
        Serial.print(" ");
        Serial.print("Y: ");
        Serial.println(walker.GetStart('y'));
        Serial.println("Destination: ");
        Serial.print("X: ");
        Serial.print(walker.GetDestination('x'));
        Serial.print(" ");
        Serial.print("Y: ");
        Serial.println(walker.GetDestination('y'));
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
     if(walker.CheckPosition(6250, 6250) || walker.CheckPosition(8750, 6250) 
        || walker.CheckPosition(8750, 8750) || walker.CheckPosition(6250,8750))
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