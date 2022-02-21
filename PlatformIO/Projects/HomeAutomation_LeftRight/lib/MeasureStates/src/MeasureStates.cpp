#include "MeasureStates.h"
#include <Arduino.h>
#include <StateMachine.h>
#include <SystemStates.h>
#include <RumbleStates.h>

#define maxloops 20000
#define maxloops2 500
#define max_duration 45000 //unit is  us 

#define TransmissionTime 150

const char CSV_SEPARATOR = ';';

long TransmissionDuration {0};
long TransmissionStart {0};
long MeasureStart {0};
long MeasureDuration {0};
long TimeToObstacle {0};
long ReadStart {0};
long ReadTime {0};

int PulseDetect {49};
int ReadChannel {A0};
int ReadADC1 {0};
int ReadADC0 {0};
double distance {0};
double height {0};
extern int time_ms;


bool MeasureAutotmatFrist {true};
bool FirstRumble {true};

extern StateMachine SystemAutomat;
extern StateMachine MeasureAutomat;
extern StateMachine RumbleAutomat;
extern bool STATE_CONT_MEASURE;
extern bool STATE_INTERVAL_MEASURE;
extern double Air_Speed;
extern bool SystemState;
extern bool MeasureState;
extern bool RumbleState;
extern int speed;


void smMeasureInit()
{
  
  TransmissionDuration = 0;
  TransmissionStart = 0;
  MeasureStart = 0;
  MeasureDuration = 0;
  TimeToObstacle = 0;

  MeasureAutomat.enter(BeginTransmission);
}

void BeginTransmission()
{
    digitalWrite(52, LOW);
    Serial.println('\n');
    //Serial.println("Left---------------------------------------");
    ReadADC0 = 0;
    ReadADC1 = 0;
    TransmissionStart = micros();
    digitalWrite(53, HIGH);
    MeasureAutomat.enter(StopTransmission);
}


void StopTransmission()
{
    TransmissionDuration = micros() - TransmissionStart;
    if(TransmissionDuration >= TransmissionTime)
    {
        digitalWrite(53, LOW);   
        MeasureAutomat.enter(SeekObstacle, 1100);
    }
}


void SeekObstacle()
{
    if(digitalRead(PulseDetect))
    {
        TimeToObstacle = micros() - TransmissionStart;
        ReadStart = micros();
        digitalWrite(52, HIGH);
        MeasureAutomat.enter(ReadObstacle);
    }
      MeasureDuration = micros() - TransmissionStart;

    if(MeasureDuration >= max_duration)
    {
        MeasureAutomat.enter(BeginTransmission);
        RumbleAutomat.enter(MotorControlSEC0);
    }

}

void ReadObstacle()
{
    if(ReadTime >= 500)
    {
        
        MeasureAutomat.enter(EvalObstacle);
    }
    else
    {
        ReadADC1 = ReadADC0;                 //Store last conversion result for comparison
        ReadADC0 = analogRead(ReadChannel);       //Load ADC value                   

        if(ReadADC0 < ReadADC1)              //Check if max is reached
        {
            ReadADC0 = ReadADC1;
        }
    }

    ReadTime = micros()-ReadStart;

}

void EvalObstacle()
{ 
    //Calculations and output
    distance = (double) (0.5*Air_Speed*TimeToObstacle)/1000000;
    height = (double) ReadADC0*33/10240;
    SystemState = false;
    MeasureState = false;
    RumbleState = true;
    if(FirstRumble)
    {
        RumbleAutomat.enter(MotorControlSEC0);
        FirstRumble = false;
    }
    MeasureAutomat.enter(MeasureSerialOut);
}

void MeasureSerialOut() 
{
    Serial.print(time_ms);
    Serial.print(CSV_SEPARATOR);
    Serial.print(height);
    Serial.print(CSV_SEPARATOR);
    Serial.print(distance);
    Serial.print(CSV_SEPARATOR);
    Serial.print(speed);
    Serial.print("  ");

    MeasureDuration = micros() - TransmissionStart;

    if(MeasureDuration >= max_duration)
    {
        FirstRumble = true;
        if(STATE_CONT_MEASURE)
        {
            SystemAutomat.enter(ContMeasure);
            MeasureAutomat.enter(smMeasureInit);
            RumbleState = false;
            MeasureState = false;
            SystemState = true;
        }
        else if(STATE_INTERVAL_MEASURE)
        {
            SystemAutomat.enter(IntervalMeasure);
            MeasureAutomat.enter(smMeasureInit);
            MeasureState = false;
            RumbleState = false;
            SystemState = true;
        }
    }
    else
    {
        MeasureAutomat.enter(SeekObstacle);
    }
    
}