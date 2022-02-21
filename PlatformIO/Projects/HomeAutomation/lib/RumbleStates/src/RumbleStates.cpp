#include "RumbleStates.h"
#include <Arduino.h>
#include <StateMachine.h>
#include <SerialProcessing.h>
#include <MeasureClass.h>
#include <DuePWM.h>
#include <RumbleClass.h>

Rumble MotorControl;

extern StateMachine RumbleAutomat;      //Defined and initialized in baseDevice.cpp
extern USonicMeasure Measure;

extern bool SystemState;                //Defined and initialized in baseDevice.cpp
extern bool MeasureState;               //Defined and initialized in baseDevice.cpp
extern bool RumbleState;                //Defined and initialized in baseDevice.cpp

double distance {0};
int speed {250};


bool RumbleTestFirst {true};
char NextEngine {'0'};


//State Functions
void RumbleCycle()
{
  //Observation State, always called! 
}


void RumbleInit()
{
  //Serial.println("Rumble Automat Init");
  RumbleAutomat.enter(MotorControlSEC0);
}

void MotorControlSEC0()
{
    distance = Measure.GetDistance();
    
    //Serial.print("SEC0: ");
    if(distance < MotorControl.GetSecMax(SEC0))
    {  
        speed = MotorControl.CalcDuty(distance, SEC0);

        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC1);
    }
      
    else if(distance < MotorControl.GetSecMax(SEC1))
    {
        MotorControl.duty(0, SEC0);
        speed = MotorControl.CalcDuty(distance, SEC1);

        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC2);
    }
      else if(distance <= MotorControl.GetSecMax(SEC2))
      {      
        
        MotorControl.duty(0, SEC0, SEC1);      
        speed = MotorControl.CalcDuty(distance, SEC2);

        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC3);
      }
      else if(distance > MotorControl.GetSecMax(SEC2))
      { 
        MotorControl.duty(0, SEC0, SEC1, SEC2);      
        speed = MotorControl.CalcDuty(distance, SEC3);

        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC4);
      }
      else
      {
        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC4);
      }
}

void MotorControlSEC1()
{
   distance = Measure.GetDistance();
   
    //Serial.print("SEC1: ");
    if(distance < MotorControl.GetSecMax(SEC0))
    {
      speed = 0;
      MeasureState = true;
      RumbleState = false;
      //no action section already used
    }
      
    else if(distance < MotorControl.GetSecMax(SEC1))
    {
      speed = MotorControl.CalcDuty(distance, SEC1);

      MeasureState = true;
      RumbleState = false;
      RumbleAutomat.enter(MotorControlSEC2);
    }
    else if(distance < MotorControl.GetSecMax(SEC2))
    {
        MotorControl.duty(0, SEC1);
        speed = MotorControl.CalcDuty(distance, SEC2);

        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC3);
    }
    else if(distance > MotorControl.GetSecMax(SEC2))
    { 
        MotorControl.duty(0, SEC1, SEC2);
        speed = MotorControl.CalcDuty(distance, SEC3);

        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC4);
    }

    else
    {
      MeasureState = true;
      RumbleState = false;
      RumbleAutomat.enter(MotorControlSEC4);
    }
}

void MotorControlSEC2()
{
  distance = Measure.GetDistance();
  
  //Serial.print("SEC2: ");
  if(distance < MotorControl.GetSecMax(SEC1))
  {
    speed = 0;
    MeasureState = true;
    RumbleState = false;
    //no action section already used
  }

  else if(distance < MotorControl.GetSecMax(SEC2))
  {
    speed = MotorControl.CalcDuty(distance, SEC2);

    MeasureState = true;
    RumbleState = false;
    RumbleAutomat.enter(MotorControlSEC3);
  }
  else if(distance > MotorControl.GetSecMax(SEC2))
  { 
    MotorControl.duty(0, SEC2);
    speed = MotorControl.CalcDuty(distance, SEC3);
    
    MeasureState = true;
    RumbleState = false;
    RumbleAutomat.enter(MotorControlSEC4);
  }

  else
  {
    MeasureState = true;
    RumbleState = false;
    RumbleAutomat.enter(MotorControlSEC4);
  }

}

void MotorControlSEC3()
{
  distance = Measure.GetDistance();
  
  //Serial.print("SEC3: ");
  if(distance < MotorControl.GetSecMax(SEC2))
  {
    speed = 0;
    MeasureState = true;
    RumbleState = false;
    //no action section already used
  }
  
  else if(distance > MotorControl.GetSecMax(SEC2))
  { 
    speed = MotorControl.CalcDuty(distance, SEC3);

    MeasureState = true;
    RumbleState = false;
    RumbleAutomat.enter(MotorControlSEC4);
  }
  else
  {
    MotorControl.duty(0, SEC3);
    
    MeasureState = true;
    RumbleState = false;
    RumbleAutomat.enter(MotorControlSEC4);
  }
}

void MotorControlSEC4()
{
  //No action since all sections have already been set
  //Waiting for new measurement to start
  MeasureState = true;
  RumbleState = false;
}

void RumbleTestSEC0()
{
  if(RumbleTestFirst)
  {
    Serial.println(); //extra new line for better readability
    
    Serial.println("Testing rumble engine @ index finger");
    Serial.println("Press space to test next rumble engine");

    MotorControl.ena(SEC0);
    MotorControl.duty(100, SEC0);
    
    RumbleTestFirst = false;
  }

  NextEngine = READ_Char();

  if(NextEngine == ' ')
  {
    MotorControl.dis(SEC0);
    RumbleAutomat.enter(RumbleTestSEC1);
    RumbleTestFirst = true;
  }
  
}

void RumbleTestSEC1()
{
  if(RumbleTestFirst)
  {
    Serial.println(); //extra new line for better readability
  
    Serial.println("Testing rumble engine @ middle finger");
    Serial.println("Press space to test next rumble engine");
    
    MotorControl.ena(SEC1);
    MotorControl.duty(100, SEC1);
    
    RumbleTestFirst = false;
  }

  NextEngine = READ_Char();

  if(NextEngine == ' ')
  {
    MotorControl.dis(SEC1);
    RumbleAutomat.enter(RumbleTestSEC2);
    RumbleTestFirst = true;
  }
}

void RumbleTestSEC2()
{
  if(RumbleTestFirst)
  {
    Serial.println(); //extra new line for better readability
  
    Serial.println("Testing rumble engine @ ring finger");
    Serial.println("Press space to test next rumble engine");

    MotorControl.ena(SEC2);
    MotorControl.duty(100, SEC2);

    RumbleTestFirst = false;
  }

  NextEngine = READ_Char();

  if(NextEngine == ' ')
  {
    MotorControl.dis(SEC2);
    RumbleAutomat.enter(RumbleTestSEC3);
    RumbleTestFirst = true;
  }
}

void RumbleTestSEC3()
{
  if(RumbleTestFirst)
  {
    Serial.println(); //extra new line for better readability
  
    Serial.println("Testing rumble engine @ pinky");
    Serial.println("Press space to test next rumble engine");

    MotorControl.ena(SEC3);
    MotorControl.duty(100, SEC3);
    
    RumbleTestFirst = false;
  }

  NextEngine = READ_Char();

  if(NextEngine == ' ')
  {
    MotorControl.dis(SEC3);
    SystemState = true;
    MeasureState = false;
    RumbleState = false;
    RumbleTestFirst = true;
    RumbleAutomat.enter(RumbleInit);
  }
}


