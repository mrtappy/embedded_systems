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


//State function prototypes 
void smStartWithoutSMN();
void smWaitForSMN();
void smInitTwitter();
void smInitFollower();
void smWaitForTestTwitter();
void smDisplayValues();
void smStartWithoutTTW();

void smMeasureInit();

void FetchStart();
void FetchDestination();
void CalcPosition();
void PrintPosition();
void AmpelCheck();


// Public (twittered) state (base state)
//
enum smPublicState
{
  smpsUnborn,
  smpsInit,
  smpsWaitTwitter,
  smpsRun
};

// ---------------------------------------------------------------------------
#endif  // baseDevice_h
