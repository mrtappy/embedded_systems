#include "MeasureClass.h"
#include <Arduino.h>
//#include <MeasureStates.h>
//#include <RumbleStates.h>
#include <DuePWM.h>
 

//extern int IntervalLength;
//extern int speed;

extern DuePWM USonicTransmitter;

//Constructor
USonicMeasure::USonicMeasure()
    : EnvironmentTemperature {25}, TransmissionTime {150}, MaxMeasureDuration {45000}, TriggerPin{53}, DetectPin{49}, 
    CSV_Separator {';'}, AnalogChannel {A0}, MaxFindMaxDuration {500}, distance{0}, PulseHeight{0}, TransmissionStart {0}, TimeToObstacle {0},
    FindMaxStart {0}, FindMaxDuration {0}, MeasureDuration {0}, ReadADC0 {0}, ReadADC1 {0}, NumOfObstacles {0}, AirSpeed {0},
    TransmissionMeasureDelay {2500}
{
    USonicMeasure::CalcSpeedOfSound(EnvironmentTemperature);
    //USonicTransmitter.setup(40000, PWM4);
    MeasurePWMch = PWM4;

}

USonicMeasure::USonicMeasure(double room_temp)
    : EnvironmentTemperature {room_temp}, TransmissionTime {150}, MaxMeasureDuration {45000}, TriggerPin{53}, DetectPin{49}, 
    CSV_Separator {';'}, AnalogChannel {A0}, MaxFindMaxDuration {500}, distance{0}, PulseHeight{0}, TransmissionStart {0}, TimeToObstacle {0},
    FindMaxStart {0}, FindMaxDuration {0}, MeasureDuration {0}, ReadADC0 {0}, ReadADC1 {0}, NumOfObstacles {0}, AirSpeed {0},
    TransmissionMeasureDelay {2500}
{
    USonicMeasure::CalcSpeedOfSound(EnvironmentTemperature);
    //USonicTransmitter.setup(40000, PWM4);
    MeasurePWMch = PWM4;

}

USonicMeasure::USonicMeasure(long MaxMeasureDur)
    : MaxMeasureDuration {MaxMeasureDur}, EnvironmentTemperature {25}, TransmissionTime {150}, TriggerPin{53}, DetectPin{49}, 
    CSV_Separator {';'}, AnalogChannel {A0}, MaxFindMaxDuration {500}, distance{0}, PulseHeight{0}, TransmissionStart {0}, TimeToObstacle {0},
    FindMaxStart {0}, FindMaxDuration {0}, MeasureDuration {0}, ReadADC0 {0}, ReadADC1 {0}, NumOfObstacles {0}, AirSpeed {0},
    TransmissionMeasureDelay {2500}
{
    //USonicTransmitter.setup(40000, PWM4);
    MeasurePWMch = PWM4;

}

USonicMeasure::USonicMeasure(int TrigPin, int DetPin, int AnalogCh)
    : TriggerPin {TrigPin}, DetectPin {DetPin}, AnalogChannel {AnalogCh}, EnvironmentTemperature {25}, TransmissionTime {150}, 
    MaxMeasureDuration {45000}, CSV_Separator {';'}, MaxFindMaxDuration {500}, distance{0}, PulseHeight{0}, TransmissionStart {0}, TimeToObstacle {0},
    FindMaxStart {0}, FindMaxDuration {0}, MeasureDuration {0}, ReadADC0 {0}, ReadADC1 {0}, NumOfObstacles {0}, AirSpeed {0},
    TransmissionMeasureDelay {2500}
{
    USonicMeasure::CalcSpeedOfSound(EnvironmentTemperature);
    //USonicTransmitter.setup(40000, PWM4);
    MeasurePWMch = PWM4;
}



//Methods

double USonicMeasure::CalcSpeedOfSound()
{
    AirSpeed = (331.5 + 0.6*EnvironmentTemperature);
    return AirSpeed;
}


double USonicMeasure::CalcSpeedOfSound(double room_temp)
{
    EnvironmentTemperature = room_temp;
    AirSpeed = (331.5 + 0.6*room_temp);
    return AirSpeed;
}

void USonicMeasure::Start()
{
    Serial.println(MeasureDuration);
    NumOfObstacles = 0;
    TimeToObstacle = 0;
    ReadADC0 = 0;
    ReadADC1 = 0;
    MeasureDuration = 0;
    TransmissionStart = micros();
    //digitalWrite(TriggerPin, HIGH);
    USonicTransmitter.duty4(50);
}


/*void USonicMeasure::Start(int TrigPin, long TransTime)
{
    NumOfObstacles = 0;
    ReadADC0 = 0;
    ReadADC1 = 0;
    TriggerPin = TrigPin;
    TransmissionStart = micros();
    TransmissionTime = TransTime;
    digitalWrite(TriggerPin, HIGH);
}*/

void USonicMeasure::Start(int PWMch, long TransTime)
{
    NumOfObstacles = 0;
    ReadADC0 = 0;
    ReadADC1 = 0;
    TransmissionStart = micros();
    TransmissionTime = TransTime;  
    USonicTransmitter.freq(40000, PWMch);
    USonicTransmitter.ena(PWMch);
    USonicTransmitter.duty(50, PWMch);
    MeasurePWMch = PWMch;
}

bool USonicMeasure::End()
{
    TransmissionDuration = micros() - TransmissionStart;
    if(TransmissionDuration >= (TransmissionTime + TransmissionMeasureDelay))
    {
        return true;
    }
    else if(TransmissionDuration >= TransmissionTime)
    {
        //digitalWrite(TriggerPin, LOW);
        USonicTransmitter.duty(0, MeasurePWMch);
        return false;
    }
    else
    {
        return false;
    }
}

int USonicMeasure::GetStartTime()
{
    return TransmissionStart;
}

bool USonicMeasure::ObstacleDetect()
{
    
    if(digitalRead(DetectPin))
    {
        TimeToObstacle = micros() - TransmissionStart;
        FindMaxStart = micros();
        NumOfObstacles++;
        return true;
    }
    else
    {
        return false;
    }
}


bool USonicMeasure::ObstacleDetect(int DectPin)
{
    
    if(digitalRead(DectPin))
    {
        TimeToObstacle = micros() - TransmissionStart;
        FindMaxStart = micros();
        NumOfObstacles++;
        return true;
    }
    else
    {
        return false;
    }
}

long USonicMeasure::Duration()
{
    MeasureDuration = micros() - TransmissionStart;
    return MeasureDuration;
}

bool USonicMeasure::FindMax()
{
    FindMaxDuration = micros() - FindMaxStart;
    if(FindMaxDuration >= MaxFindMaxDuration)
    {
        return true;
    }
    else
    {
        ReadADC0 = analogRead(AnalogChannel);

        if(ReadADC0 < ReadADC1)
        {
            ReadADC0 = ReadADC1;
        }
        else
        {
            ReadADC1 = ReadADC0;
        }

        return false;
    }
}

bool USonicMeasure::FindMax(int AnalogCh)
{
    FindMaxDuration = micros() - FindMaxStart;
    if(FindMaxDuration >= MaxFindMaxDuration)
    {
        return true;
    }
    else
    {
        ReadADC0 = analogRead(AnalogCh);

        if(ReadADC0 < ReadADC1)
        {
            ReadADC0 = ReadADC1;
        }
        else
        {
            ReadADC1 = ReadADC0;
        }

        return false;
    }
}

bool USonicMeasure::FindMax(long Duration, int AnalogCh)
{
    FindMaxDuration = micros() - FindMaxStart;
    if(FindMaxDuration >= Duration)
    {
        return true;
    }
    else
    {
        ReadADC0 = analogRead(AnalogCh);

        if(ReadADC0 < ReadADC1)
        {
            ReadADC0 = ReadADC1;
        }
        else
        {
            ReadADC1 = ReadADC0;
        }

        return false;
    }
}

int USonicMeasure::GetTimeToObstacle()
{
    return TimeToObstacle;
}

int USonicMeasure::GetMax()
{
    return ReadADC0;
}

void USonicMeasure::EvalObstacle()
{
    distance = (double) (0.5*AirSpeed*TimeToObstacle)/10000;
    PulseHeight = (double) ReadADC0*33/10240;
}

double USonicMeasure::GetDistance()
{
    return distance;
}

void USonicMeasure::SerialOut()
{
    //Serial.print(IntervalLength);
    Serial.print(CSV_Separator);
    Serial.print(PulseHeight);
    Serial.print(CSV_Separator);
    Serial.print(distance);
    //Serial.print(CSV_Separator);
    //Serial.print(speed);
    Serial.print(" ");
}

void USonicMeasure::SetCSV_Separator(char separator)
{
    CSV_Separator = separator;
}

char USonicMeasure::GetCSV_Separator()
{
    return CSV_Separator;
}

int USonicMeasure::GetNumOfObstacles()
{
    return NumOfObstacles;
}

void USonicMeasure::SetMaxMeasureDuration(long MaxDur)
{
    MaxMeasureDuration = MaxDur;
}

long USonicMeasure::GetMaxMeasureDuration()
{
    return MaxMeasureDuration;
}

void USonicMeasure::SetEnvTemperature(double room_temp)
{
    EnvironmentTemperature = room_temp;
}

void USonicMeasure::SetTriggerPin(int TrigPin)
{
    TriggerPin = TrigPin;
}

void USonicMeasure::SetDetectPin(int DetPin)
{
    DetectPin = DetPin;
}

void USonicMeasure::SetAnalogChannel(int AnalogCh)
{
    AnalogChannel = AnalogCh;
}

void USonicMeasure::SetMaxFindMaxDuration(long MaxFindMax)
{
    MaxFindMaxDuration = MaxFindMax;
}

void USonicMeasure::SetTransmissionTime(long TransTime)
{
    TransmissionTime = TransTime;
}

void USonicMeasure::SetTransmissionMeasureDelay(long delay)
{
    TransmissionMeasureDelay = delay;
}

void USonicMeasure::SetMinDistance(double MinDis)
{
    if(AirSpeed != 0)
    {
        TransmissionMeasureDelay = (long) 2*(MinDis/AirSpeed);    
    }
}

double USonicMeasure::GetEnvTemperature()
{
    return EnvironmentTemperature;
}

double USonicMeasure::GetAirSpeed()
{
    return AirSpeed;
}

int USonicMeasure::GetTriggerPin()
{
    return TriggerPin;
}

int USonicMeasure::GetDetectPin()
{
    return DetectPin;   
}

int USonicMeasure::GetAnalogChannel()
{
    return AnalogChannel;
}

long USonicMeasure::GetMaxFindMaxDuration()
{
    return MaxFindMaxDuration;
}

long USonicMeasure::GetTransmissionTime()
{
    return TransmissionTime;
}

long USonicMeasure::GetTransmissionMeasureDelay()
{
    return TransmissionMeasureDelay;
}

double USonicMeasure::GetMinDistance()
{
    if(AirSpeed != 0)
    {
        return (double) AirSpeed*TransmissionMeasureDelay*0.5;
    }
}

