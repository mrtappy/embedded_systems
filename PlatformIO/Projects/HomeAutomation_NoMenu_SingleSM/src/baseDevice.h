// ---------------------------------------------------------------------------
// File:        baseDevice.h
// Editors:     Robert Patzke,
// Start:       11. November 2017
// Last change: 11. November2017
// URI/URL:     www.mfp-portal.de
// Licence:     Creative Commons CC-BY-SA 
// ---------------------------------------------------------------------------
//
#ifndef baseDevice_h
#define baseDevice_h


// Public (twittered) state (base state)
//
enum smPublicState
{
  smpsUnborn,
  smpsInit,
  smpsWaitTwitter,
  smpsRun
};

//State function prototypes 
void smStartWithoutSMN();
void smWaitForSMN();
void smInitTwitter();
void smInitFollower();
void smWaitForTestTwitter();
void smDisplayValues();
void smStartWithoutTTW();

void smMeasureInit();

void StartTransmission();
void EndTransmission();
void SeekObstacle();
void ReadObstacle();
void EvalObstacle();
void SerialOutObstacle();
void Test();
//-----------------------------------


//Rumble

//Rumble State Machine prototypes
void smRumbleInit();
void MotorControlSEC0();
void MotorControlSEC1();
void MotorControlSEC2();
void MotorControlSEC3();

//--------------------------------

//Other Function prototypes
void PWM_Setup();
void MotorOff();

//----

//System State Machine prototypes


//--------------------------------





// ---------------------------------------------------------------------------
#endif  // baseDevice_h
