#include "SystemStates.h"
#include <Arduino.h>
#include <SerialProcessing.h>
#include <StateMachine.h>
#include <MeasureStates.h>
#include <RumbleStates.h>

extern double Air_Speed;
extern char str[len];
extern double EnviornmentTemperature;
extern int RX_STRING_READY; 
extern StateMachine SystemAutomat;
extern StateMachine MeasureAutomat;
extern bool STATE_CONT_MEASURE;
extern bool STATE_INTERVAL_MEASURE;
extern void smMeasureInit();
extern bool SystemState;
extern bool MeasureState;
extern bool RumbleState;

//global variables 
int STATE_VARIABLE {0};
long time_ms {0};
long IntervalMeasureStart {0};
long IntervalMeasureDuration {0};
bool HomeMenuFirst {true};
bool MeasureStopFrist {true};
bool GetTimeMS {false};
bool SelectTimeFirst {true};
bool IntervalMeasureFirst {true};
bool IntervalMeasureEndFirst {true};
char IntervalMeasureRestart {'0'};

void CalcSpeed()
{
  
  if(SystemAutomat.firstEnter())
  {
    Serial.println("Fetching temperature...");
  }

  else
  {
    if(RX_STRING_READY == 0)
    {
      EnviornmentTemperature = READ_double(str);
    }
    
    else
    {
      Serial.print("\nTemperature: ");
      Serial.print(EnviornmentTemperature);
      Serial.println(" °C");
      Air_Speed = 331.5 +0.6*EnviornmentTemperature;
      Air_Speed *= 100; //m/s to cm/s conversion
      Serial.print("AirSpeed: ");
      Serial.print(Air_Speed);
      Serial.println(" cm/s");
      Serial.println();
      RESET_STRING(str);
      SystemAutomat.enter(HomeMenu);
    }
  }
}

void HomeMenu()
{
  if(HomeMenuFirst)
  {
    Serial.println("Hello!");
    Serial.println("Please choose your next action...");
    Serial.println("1: continuos measurement");
    Serial.println("2: measurement interval");
    Serial.println("3: Test rumble motors");
    Serial.println("4: End program");
    Serial.println();
    Serial.println("Enter mode of operation: ");
    HomeMenuFirst = false;
    STATE_VARIABLE = 0;
  }

  else
  {
    if(STATE_VARIABLE == 0)
    {
      STATE_VARIABLE = READ_Digit();
    }
    else if(STATE_VARIABLE == 1)
    {
      HomeMenuFirst = true;
      SystemAutomat.enter(ContMeasure);
    }
    else if(STATE_VARIABLE == 2)
    {
      HomeMenuFirst = true;
      SystemAutomat.enter(SelectTime);
    }
    else if(STATE_VARIABLE == 3)
    {
      HomeMenuFirst = true;
      SystemAutomat.enter(TestRumble);
    }
    else if(STATE_VARIABLE == 4)
    {
      SystemAutomat.enter(SystemStop);
    }
    SERIAL_FLUSH();
  }
}

void ContMeasure()
{
  Serial.println("ContMeasure");
  STATE_CONT_MEASURE = true;
  STATE_INTERVAL_MEASURE = false;
  MeasureState = true;
  SystemState = false;
  if(Serial.available() > 0)
  {
    SystemAutomat.enter(MeasureStop);
    MeasureState = false;
    SystemState = true;
  }
  else
  { 
    MeasureAutomat.run();
    MeasureState = true;
    SystemState = false;
  }
}

void SelectTime()
{
  if(SelectTimeFirst)
  {
    Serial.println("Enter measurement duration: ");
    SelectTimeFirst = false;
    RESET_STRING(str);
  }

  if(RX_STRING_READY == 0)
  {
    time_ms = READ_int(str);
  }
  else
  {
    Serial.print("You entered: ");
    Serial.println(time_ms);
    SystemAutomat.enter(IntervalMeasure);
  }
}

void IntervalMeasure()
{
  if(IntervalMeasureFirst)
  {
    IntervalMeasureFirst = false;
    Serial.println("IntervalMeasure");
    IntervalMeasureStart = millis();
  }
  STATE_CONT_MEASURE = false;
  STATE_INTERVAL_MEASURE = true;
  
  IntervalMeasureDuration = millis()-IntervalMeasureStart;

  if(IntervalMeasureDuration >= time_ms)
  {
    SystemState = true;
    MeasureState = false;
    Serial.print("Duration: ");
    Serial.println(IntervalMeasureDuration);
    SystemAutomat.enter(IntervalMeasureEnd);

  }
  else
  {
    MeasureState = true;
    SystemState = false;
    MeasureAutomat.run();
  }
 
}

void IntervalMeasureEnd()
{
  
  if(IntervalMeasureEndFirst)
  {
    SERIAL_FLUSH();
    MotorOff();
    Serial.println("Measurement interval done");
    Serial.println("Restart measurement (Y/N)?");
    IntervalMeasureRestart = 0;
    IntervalMeasureEndFirst = false;
  }

  if(IntervalMeasureRestart == 0)
  {
    IntervalMeasureRestart = READ_Char();
    
  }
  else if(IntervalMeasureRestart == 'y')
  {
    IntervalMeasureFirst = true;
    IntervalMeasureEndFirst = true;
    SystemAutomat.enter(IntervalMeasure);
  }
  else if(IntervalMeasureRestart == 'n')
  {
    IntervalMeasureFirst = true;
    IntervalMeasureEndFirst = true;
    SelectTimeFirst = true;
    SystemAutomat.enter(MeasureStop);
  }
  else if(IntervalMeasureRestart != 0)
  {
    Serial.println("Invalid input, try again");
    Serial.println("Restart measurement (Y/N)?");
    IntervalMeasureRestart = 0;
  }
  else
  {
    //Wait for input
  }
}


void TestRumble()
{

}

void MeasureStop()
{
  if(MeasureStopFrist)
  {
    STATE_VARIABLE = 0;
    MotorOff();
    Serial.println("Measurement ended");
    Serial.println("What now?");
    Serial.println("1. Go to main menu   2. Exit");
    MeasureStopFrist = false;
    SERIAL_FLUSH();
  }
  else
  {
    if(STATE_VARIABLE == 0)
    {
      STATE_VARIABLE = READ_Digit();
    }
    else if(STATE_VARIABLE == 1)
    {
      MeasureStopFrist = true;
      SystemAutomat.enter(HomeMenu);
    }
    else if(STATE_VARIABLE == 2)
    {
      SystemAutomat.enter(SystemStop);
    }
    else if(STATE_VARIABLE != 0)
    {
      Serial.println("Invalid Input, try again");
      Serial.println("What now?");
      Serial.println("1. Go to main menu   2. Exit");
      STATE_VARIABLE = 0;
    }
    SERIAL_FLUSH();
  }
}


void SystemStop()
{
  if(SystemAutomat.firstEnter())
  {
    Serial.println("Exit");
  }
}