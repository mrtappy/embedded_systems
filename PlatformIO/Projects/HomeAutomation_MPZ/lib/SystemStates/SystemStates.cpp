#include "SystemStates.h"
#include <Arduino.h>
#include <StateMachine.h>
#include <MeasureStates.h>
#include <RumbleStates.h>
#include <SerialProcessing.h>
#include <RumbleClass.h>
#include <DuePWM.h>


//GlobalVariables
char HomeMenuChoice {'0'};
char EndMeasureChoice {'0'};
char EndIntervalMeasureChoice {'0'};
char TEMP_FETCH {'0'};
char str[len] {0};

int IntervalLength {0};

long IntervalMeasureStart {0};
long IntervalMeasureDuration {0};

bool EndMeasureFirst {true};
bool EndIntervalMeasureFirst {true};
bool IntervalMeasureFirst {true};
bool ContMeasureFirst {true};


//external
extern Rumble MotorControl;
extern StateMachine SystemAutomat;      //Defined and initialized in baseDevice.cpp
extern StateMachine MeasureAutomat;     //Defined and initialized in baseDevice.cpp 
extern StateMachine RumbleAutomat;      //Defined and initialized in baseDevice.cpp
extern bool SystemState;                //Defined and initialized in baseDevice.cpp
extern bool MeasureState;               //Defined and initialized in baseDevice.cpp
extern bool RumbleState;                //Defined and initialized in baseDevice.cpp
extern int RX_STRING_READY;             //Defined and initialized in SerialProcessing.cpp
extern int STRING_POSITION;
extern bool FLUSH_READY;                //Defined and initialized in SerialProcessing.cpp

void smSystemInit()
{
    Serial.println("Starting system");
    SystemAutomat.enter(HomeMenu);
}

void SystemCycle()
{

}


void HomeMenu()
{ 
    ContMeasureFirst = true;
    EndMeasureFirst = true;
    EndIntervalMeasureFirst = true;
    IntervalMeasureFirst = true;
    RumbleState = false;
    MeasureState = false;
    SystemState = true;
    HomeMenuChoice = '0';
    EndMeasureChoice = '0';
    EndIntervalMeasureChoice = '0';
    TEMP_FETCH = '0';

    Serial.println("Hello!");
    Serial.println("Please choose your next action...");
    Serial.println("1: continuos measurement");
    Serial.println("2: measurement interval");
    Serial.println("3: Test rumble motors");
    Serial.println("4: End program");
    Serial.println();
    Serial.println("Enter mode of operation: ");
    SystemAutomat.enter(FetchHomeMenu);
}

void FetchHomeMenu()
{
    if(RX_STRING_READY != 1)
    {
        READ_String(str);
        HomeMenuChoice = str[0];
    }
    
    //HomeMenuChoice = READ_Char();

    else if(HomeMenuChoice == '0')
    {
        //Nothing, wait for input
    }
    else if(HomeMenuChoice == '1')
    {
        SystemAutomat.enterVia(SerialReset, ContMeasure);
    }
    else if(HomeMenuChoice == '2')
    {
        SystemAutomat.enterVia(SerialReset, InitIntervalMeasure);
    }
    else if(HomeMenuChoice == '3')
    {
        SystemAutomat.enterVia(SerialReset, RumbleTest);
    }
    else if(HomeMenuChoice == '4')
    {
        SystemAutomat.enterVia(SerialReset, EndProgram);
    }
    else
    {
        Serial.println("Invalid input, try again");
        HomeMenuChoice = '0';
        SystemAutomat.enterVia(SerialReset, FetchHomeMenu);
    }
}

void ContMeasure()
{
    if(ContMeasureFirst)
    {
        MotorControl.ena(PWM0, PWM1, PWM2, PWM3);
        ContMeasureFirst = false;
        Serial.println("Starting measurement");
    }
    
    if(Serial.available() > 0)
    {
        SystemState = true;
        MeasureState = false;
        RumbleState = false;
        TEMP_FETCH = Serial.read();
        SystemAutomat.enterVia(SerialReset, EndMeasure);
    }
    else
    {
        MeasureState = true;
        SystemState = false;
        RumbleState = false;
    }
}

void EndMeasure()
{
    RumbleState = false;
    MeasureState = false;
    SystemState = true;
    
    if(EndMeasureFirst)
    {
        Serial.println("\nMeasurement ended");
        Serial.println("What now?");
        Serial.println("1. Home menu    2. End program");
        Serial.println("Choose next step: ");
        MotorControl.dis(PWM0, PWM1, PWM2, PWM3);
        SystemAutomat.enterVia(SerialReset, FetchEndMeasure);
    }

    EndMeasureFirst = false;
}

void FetchEndMeasure()
{
    //Serial.println(RX_STRING_READY);

    if(RX_STRING_READY != 1)
    {
        READ_String(str);
        EndMeasureChoice = str[0];
    }
    
    //EndMeasureChoice = READ_Char();

    else if(EndMeasureChoice == '0')
    {
        //Nothing, wait for input
    }
    else if(EndMeasureChoice == '1')
    {
        SystemAutomat.enterVia(SerialReset, HomeMenu);
    }
    else if(EndMeasureChoice == '2')
    {
        SystemAutomat.enterVia(SerialReset, EndProgram);
    }
    else
    {
        EndMeasureChoice = '0';
        SystemAutomat.enterVia(SerialReset, FetchEndMeasure);
    }
}

void InitIntervalMeasure()
{
    Serial.println("Enter measurement duration (ms): ");
    SystemAutomat.enter(FetchIntervalMeasure);
}

void FetchIntervalMeasure()
{
    if(RX_STRING_READY != 1)
    {
        IntervalLength = READ_int(str);
    }
    else
    {
        Serial.print("You entered: ");
        Serial.println(IntervalLength);
        IntervalLength *= 1000;             //Conversion to micros
        SystemAutomat.enterVia(SerialReset, IntervalMeasure);
    }
    
}

void IntervalMeasure()
{
    if(IntervalMeasureFirst)
    {
        MotorControl.ena(PWM0, PWM1, PWM2, PWM3);
        IntervalMeasureStart = micros();
        IntervalMeasureFirst = false;
    }

    IntervalMeasureDuration = micros() - IntervalMeasureStart;

    if(IntervalMeasureDuration >= IntervalLength)
    {
        SystemState = true;
        MeasureState = false;
        RumbleState = false;
        MotorControl.disAll();
        SystemAutomat.enterVia(SerialReset, EndIntervalMeasure);
    }
    else
    {
        MeasureState = true;
        SystemState = false;
        RumbleState = false;
    }
}

void EndIntervalMeasure()
{
    if(EndIntervalMeasureFirst)
    {
        Serial.println("Measurement done");
        Serial.println("What now?");
        Serial.println("1. Restart Measurement");
        Serial.println("2. Home menu");
        Serial.println("3. End program");
        Serial.println("Choose your next action: ");

        EndIntervalMeasureFirst = false;
        SystemAutomat.enterVia(SerialReset, FetchEndIntervalMeasure);
    }

}

void FetchEndIntervalMeasure()
{
    //EndIntervalMeasureChoice = READ_Char();
    if(RX_STRING_READY != 1)
    {
        READ_String(str);
        EndIntervalMeasureChoice = str[0];
    }

    else if(EndIntervalMeasureChoice == '0')
    {
        //Nothing, wait for input
    }
    else if(EndIntervalMeasureChoice == '1')
    {
        Serial.println("Restarting measurement");
        IntervalMeasureFirst = true;
        EndIntervalMeasureFirst = true;
        SystemAutomat.enterVia(SerialReset, IntervalMeasure);
    }
    else if(EndIntervalMeasureChoice == '2')
    {
        SystemAutomat.enterVia(SerialReset, HomeMenu);
    }
    else if(EndIntervalMeasureChoice == '3')
    {
        SystemAutomat.enterVia(SerialReset, EndProgram);
    }
    else
    {
        Serial.println("Invalid input, try again");
        EndIntervalMeasureChoice = '0';
        SystemAutomat.enterVia(SerialReset, FetchEndIntervalMeasure);
    }
}


void RumbleTest()
{
    RumbleState = true;
    SystemState = false;
    MeasureState = false;
    RumbleAutomat.enter(RumbleTestSEC0);
    SystemAutomat.enter(EndMeasure);
}

void EndProgram()
{
    if(SystemAutomat.firstEnter())
    {
        Serial.println("Program ended");
    }
}

void SerialFlush()
{
    if(!FLUSH_READY)
    {
        SERIAL_FLUSH();
    }
    else
    {
        FLUSH_READY = false;
        SystemAutomat.enter();
    }
}

void ResetString()
{
    if(RX_STRING_READY != 0)
    {
        RESET_STRING(str);
    }
    else
    {
        SystemAutomat.enter();
    }
}

void SerialReset()
{
    if(!FLUSH_READY && RX_STRING_READY != 0)
    {
        SERIAL_FLUSH();
        RESET_STRING(str);
        //Serial.println("Resetting string and flushing serial port");
        //Serial.print(STRING_POSITION);
        //Serial.print(" ");
        //Serial.print(RX_STRING_READY);
        //Serial.print(" ");
        //Serial.println(str);
    }
    else if(!FLUSH_READY)
    {
        SERIAL_FLUSH();
        //Serial.println("Flushing serial port");
    }
    else if(RX_STRING_READY != 0)
    {
        RESET_STRING(str);
        //Serial.println("resetting string...");
        //Serial.print(STRING_POSITION);
        //Serial.print(" ");
        //Serial.print(RX_STRING_READY);
        //Serial.print(" ");
        //Serial.println(str);
    }
    else
    {
        //Serial.println("done.");
        SystemAutomat.enter();
    }
    
}