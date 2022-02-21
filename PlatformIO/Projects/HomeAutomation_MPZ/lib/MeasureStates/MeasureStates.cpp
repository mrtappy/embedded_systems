#include "MeasureStates.h"
#include <Arduino.h>
#include <StateMachine.h>
#include <RumbleStates.h>
#include <MeasureClass.h>

//Global variables
double Air_Speed {0};

//external 
extern StateMachine MeasureAutomat;                   //Defined and initialized in baseDevice.cpp
extern StateMachine RumbleAutomat;                    //Defined and initialized in baseDevice.cpp
extern bool SystemState;                              //Defined and initialized in baseDevice.cpp
extern bool RumbleState;                              //Defined and initialized in baseDevice.cpp
extern bool MeasureState;                             //Defined and initialized in baseDevice.cpp
extern int speed;                                     //Defined and initialized in RumbleStates.cpp

long measure_delay {0};
long measure_end {0};

USonicMeasure Measure;

void MeasureCycle()
{
    //Function to observe process if necessary
}


void MeasureInit()
{
  Air_Speed = Measure.CalcSpeedOfSound();
  Serial.print(Air_Speed);
  Serial.println(" m/s");
  MeasureAutomat.enter(StartTransmission);                   //First step after startup is to calculate the speed of sound based on the first order approximation which uses the environment temperature as variable
}

void StartTransmission()
{
  Serial.println('\n');
  Measure.Start();
  MeasureAutomat.enter(EndTransmission);
}

void EndTransmission()
{
  
  if(Measure.End())
  {
    MeasureAutomat.enter(SeekObstacle);
  }
}

void SeekObstacle()
{
  
  if(Measure.ObstacleDetect())
  {
    MeasureAutomat.enter(ReadObstacle);
  }

  if(Measure.Duration() >= (Measure.GetMaxMeasureDuration()))
  {
    SystemState = true;
    MeasureState = false;
    RumbleState = false;
    MeasureAutomat.enter(StartTransmission);
    RumbleAutomat.enter(MotorControlSEC0);
  }
}

void ReadObstacle()
{
  if(Measure.FindMax())
  {
    MeasureAutomat.enter(EvalObstacle);
  }
}

void EvalObstacle()
{
  Measure.EvalObstacle();
  MeasureState = false;
  RumbleState = true;
  MeasureAutomat.enter(SerialOutObstacle);
}

void SerialOutObstacle()
{
  Measure.SerialOut();
  
  if(Measure.Duration() >= (Measure.GetMaxMeasureDuration()))
  {
    SystemState = true;
    MeasureState = false;
    RumbleState = false;
    //MeasureAutomat.enter(StartTransmission, 2000000);
    MeasureAutomat.enter(StartTransmission);
    RumbleAutomat.enter(MotorControlSEC0);
  }
  else
  {
    MeasureAutomat.enter(SeekObstacle);
  }
}

