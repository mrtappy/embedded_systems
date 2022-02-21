// ---------------------------------------------------------------------------
// File:        baseDevice.ino
// Editors:     Robert Patzke,
// Start:       11. November 2017
// Last change: 14. April 2018
// URI/URL:     www.mfp-portal.de / homeautomation.x-api.de/wikidha
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//
// Note!
// This is an example, how devices for Decentral Home Automation (DHA) may be
// programmed.
// It is important, that such devices run a basic application task, if there
// is no communication via Social Manufacturing Network (SMN).
// DHA devices are software agents (at least partly).
//

#define MaxMeasureDuration  45000

#include <Arduino.h>
#include <baseDevice.h> 

//#define	smnDebug

#include <LoopCheck.h>
// We will use LoopCheck to control the timing of the device independent from
// the resources (timers) of the microcontroller.

#include <StateMachine.h>
// The auxiliary class StateMachine provides methods for a comfortable
// handling of state sequences (timing, etc.)

#include <SocManNet.h>
// SocManNet is the interface to the broadcast communication and is needed
// by Twitter and Follower

#include <Twitter.h>
// Twitter sends cyclic broadcast messages

#include <Follower.h>
// Follower filters broadcast messages on receiving

#include <SerialProcessing.h>
//SerialProcessing Library by DT

//Rumble
double temp_speed = 250;
int speed = 250;
int sec0_max = 150;
int sec1_max = 250;
int sec2_max = 350;
bool RumbleState = {false};
bool MeasureState = {true};
extern int speed;



// ---------------------------------------------------------------------------
// DEFINITIONS
// Definitions are used here to adjust the device (e.g. time behaviour) to
// our needs.
// ---------------------------------------------------------------------------
//
#define SMN_DEVICE_NAME     "BaseDeviceDHA"
#define SMN_TWITTER_NAME    "MyFirstCommObj"
#define SMN_FOLLOWER_NAME   "TestTwitter"


// ---------------------------------------------------------------------------
// INSTANCES and VARIABLES
// Class instances and variables defined outside functions to use them in
// any function
// ---------------------------------------------------------------------------
//
LoopCheck   loopCheck;      // Instance of LoopCheck
SocManNet   socManNet;      // Instance of SocManNet
Twitter     devTwitter;     // Instance of Twitter
Follower    devFollower;    // Instance of Follower

// basic definitions for the system state machine
//
#define MeasureCycleTime    10
void smSystemInit();      // forward reference to state smInit
void smCycle();     // forward reference to state smCycle
StateMachine MeasureAutomat(smSystemInit, smCycle, MeasureCycleTime);

lcDateTime  dt;             // Time-Structure of LoopCheck

//#define RumbleCycleTime 10
//void smRumbleInit();
//StateMachine RumbleAutomat(smRumbleInit, 0, RumbleCycleTime);

// Variables to be twittered
//
int     appIntVar1, appIntVar2, appIntVar3;
double  appFloatVar1, appFloatVar2;
char    *appTextPtr1;

// Variables (management structures) expected by Follower
//
IntegerValue    intMan1, intMan2, intMan3;
FloatValue      floatMan1, floatMan2;
TextValue       textMan1;

// Variables for debugging and error handling
//
SocManNetError	smnError;
SmnIfInfo       smnInfo;

//Variables and defines for UltrasonicSystem by DT--------------------

//Global variables

double Air_Speed {0};
double EnviornmentTemperature {25};
long TransmissionStart {0};
long TransmissionDuration {0};
long TimeToObstacle {0};
long ReadStart {0};
long ReadTime {0};
long TransmissionTime {150};
int ReadADC0 {0};
int ReadADC1 {0};
double distance {0};
double PulseHeight {0};
long time_ms {0};
const char CSV_SEPARATOR {';'};
long MeasureDuration {0};

bool SEC0 {false};
bool SEC1 {false};
bool SEC2 {false};
bool SEC3 {false};

//---------------------------------------------------------------------

// ---------------------------------------------------------------------------
// SETUP
// The setup function is called once at startup of the sketch
// ---------------------------------------------------------------------------
//
void setup()
{
  Serial.begin(115200);         // using serial interface for visualisation

  //smnError =
    //socManNet.init(false);      // Start connecting to the network with the
                                // IP-Address from socManNetUser.h

  pinMode(49, INPUT);  //Pulse detection
  pinMode(53, OUTPUT); //Trigger Pin for Arduino Nano (Transmitter PWM)
  pinMode(52, OUTPUT);
  digitalWrite(53, LOW);
  digitalWrite(52, LOW);

  PWM_Setup();
  
}

// ---------------------------------------------------------------------------
// LOOP
// The loop function is called in an endless loop
// ---------------------------------------------------------------------------
//
void loop()
{
  loopCheck.begin();    // mandatory call at loop beginning to use LoopCheck
  // -------------------------------------------------------------------------

  if(loopCheck.timerMicro(lcTimer0, 10, 0)) // clock of state machine
  {
    //the boolean variables SystemState, MeasureState and RumbleState determine which state machine is currently active
    //the next/current states are determined by the state machines themself
    //For this use case the SystemAutomat runs first and determines whether the next step is to measure (MeasureAutomat) or to check the rumble engines (RumbleAutomat)
    //If the next step is to measure the RumbleAutomat is started after each measurement to create a rough feeling for the environment

    MeasureAutomat.run();  

  }


  /*
  if(loopCheck.timerMilli(lcTimer1, 2, 0))
    devTwitter.run(500);   // giving the CPU to devTwitter for its tasks
  //
  // This happens every 2 milliseconds, which is a frequency of 500 Hz.
  // Twitter.run needs the frequency to calculate the twitter cycle time
  */

  /*
  if(loopCheck.timerMicro(lcTimer2, 10, 0))
    socManNet.run();        // giving the CPU to socManNet for its tasks
  //
  // This may happen every 10 microseconds. But we should expect,
  // that the loop cycle time (Arduino background loop() calling distance)
  // is longer than 10 microseconds.
  // So run() will be called with every loop(), if no other LoopCheck timer
  // finishes before in loop().
  // Thus, such a usage of timerMicro can only work, if it is the last
  // timer in loop, as timers behind this too short timer will not
  // come to be finished, because we have allowed only one timer finishing
  // in the same loop cycle (to avoid CPU load peaks).
  */

  // -------------------------------------------------------------------------
  loopCheck.end();      // mandatory call at loop ending to use LoopCheck
}

// ---------------------------------------------------------------------------
// Hilfsfunktionen
// ---------------------------------------------------------------------------
//
void updateTwitter()
{
  devTwitter.setIntValue(0, appIntVar1);  // First integer in broadcast message
  devTwitter.setIntValue(1, appIntVar2);  // Second integer
  devTwitter.setIntValue(2, appIntVar3);  // Third integer
  // That are 3 values for the 3 integer variables we declared with
  // devTwitter.init

  devTwitter.setFloatValue(0, appFloatVar1); // First float in broadcast message
  devTwitter.setFloatValue(1, appFloatVar2); // Second float
  // That are 2 values for the 2 float variables we declared with
  // devTwitter.init

  devTwitter.setTextValue(0, appTextPtr1);
  // One value for the one string variable we declared with devTwitter.init
}

// ***************************************************************************
//                      S T A T E   M A C H I N E
// ***************************************************************************
//

// ---------------------------------------------------------------------------
// Special state called with each clock of state machine (without delay)
// ---------------------------------------------------------------------------
//
void smCycle()
{
  // if necessary, observe your processes here
}


//------------------SystemStates--------------------------

//Initialization state for the "SystemAutomat" state machine
void smSystemInit()
{
  Air_Speed = 331.5 +0.6*EnviornmentTemperature;
  MeasureAutomat.enter(StartTransmission);                   //First step after startup is to calculate the speed of sound based on the first order approximation which uses the environment temperature as variable
  
}


void StartTransmission()
{
  Serial.println('\n');
  //Serial.println("Left---------------------------------------");
  ReadADC0 = 0;
  ReadADC1 = 0;
  SEC0 = false;
  SEC1 = false;
  SEC2 = false;
  SEC3 = false;
  TransmissionStart = micros();
  digitalWrite(53, HIGH);
  digitalWrite(52, HIGH);
  MeasureAutomat.enter(EndTransmission);
}

void EndTransmission()
{
  TransmissionDuration = micros() - TransmissionStart;
  if(TransmissionDuration >= TransmissionTime)
  {
    digitalWrite(53, LOW);
    
    MeasureAutomat.enter(SeekObstacle, 1000);
  }
}

void SeekObstacle()
{
  
  digitalWrite(52, LOW);
  
  if(digitalRead(49))
  {
    
    TimeToObstacle = micros()-TransmissionStart;
    ReadStart = micros();
    MeasureAutomat.enter(ReadObstacle);
  }
  MeasureDuration = micros() - TransmissionStart;

  if(MeasureDuration >= MaxMeasureDuration)
  {
    MeasureAutomat.enter(StartTransmission);
  }
}

void ReadObstacle()
{
  if(ReadTime > 500)
  {
    MeasureAutomat.enter(EvalObstacle);
  }
  else
  {
    
    ReadADC0 = analogRead(A0);

    if(ReadADC0 < ReadADC1)
    {
      ReadADC0 = ReadADC1;
    }
    else
    {
      ReadADC1 = ReadADC0;
    }

  }

  ReadTime = micros()-ReadStart;
}

void EvalObstacle()
{
  distance = (double) (0.5*Air_Speed*TimeToObstacle)/10000;
  PulseHeight = (double) ReadADC0*33/10240;
  
  //MeasureState = false;
  //RumbleState = true;
  //MeasureAutomat.enter(SerialOutObstacle);
  
  if(SEC0 == false)
  {
    MeasureAutomat.enter(MotorControlSEC0);
  }
  else if(SEC1 == false)
  {
    MeasureAutomat.enter(MotorControlSEC1);
  }
  else if(SEC2 == false)
  {
    MeasureAutomat.enter(MotorControlSEC2);
  }
  else if(SEC3 == false)
  {
    MeasureAutomat.enter(MotorControlSEC3);
  }
  else
  {
    MeasureAutomat.enter(SerialOutObstacle);
  }
  
}

void SerialOutObstacle()
{
  MeasureDuration = micros()-TransmissionStart;

  Serial.print(time_ms);
  Serial.print(CSV_SEPARATOR);
  Serial.print(PulseHeight);
  Serial.print(CSV_SEPARATOR);
  Serial.print(distance);
  Serial.print(CSV_SEPARATOR);
  Serial.print(speed);
  Serial.print(" ");
  
  if(MeasureDuration >= MaxMeasureDuration)
  {
    MeasureAutomat.enter(StartTransmission);
  }
  else
  {
    MeasureAutomat.enter(SeekObstacle);
  }
}


//####################SocManNet disabled ##########################################
// ---------------------------------------------------------------------------
// Special (first) Initialisation
// ---------------------------------------------------------------------------
//

/*
bool setTestTwitterTime;

void smInit()
{
  // Inform user about init parameters
  //
  if(smnError != smnError_none)
  {
    Serial.print(socManNet.getErrorMsg(smnError));
    automat.enter(smStartWithoutSMN);   // next state is smStartWithoutSMN
    return;
  }

  // Set parameters for the next state
  //
  automat.setTimeOut(10000);            // 10 seconds time-out for next state
  automat.enter(smWaitForSMN);          // next state is smWaitForSMN
}

// ---------------------------------------------------------------------------
// Waiting for being connected to the network
// ---------------------------------------------------------------------------
//
void smWaitForSMN()
{
  if(socManNet.connected)
    automat.enter(smInitTwitter);       // continue there when connected

  if(automat.timeOut())
  {
    Serial.println("Time-Out with network connection");
    automat.enter(smStartWithoutSMN);   // continue there when time out
  }
}

// ---------------------------------------------------------------------------
// Initialise Twitter
// ---------------------------------------------------------------------------
//
void smInitTwitter()
{
  // Tell user network parameters (for check)
  //
  socManNet.getIfInfo(&smnInfo);
  Serial.print(smnInfo.macAdrCStr);
  Serial.print(" / ");
  Serial.println(smnInfo.ipAdrCStr);

  // There are some parameters, which have to be set befor the basic
  // initialisation of twitter is done. Because these values are used there.
  //
  devTwitter.setDeviceKey();    // The device key is used by FollowMultDev
                                // to distinguish different twitter with
                                // the same object name
  // Calling setDeviceKey without parameter (= device key) or omitting the call
  // creates a device key from the lower 2 bytes of MAC address.

  devTwitter.setApplicationKey(0);  // The application key defines a relation
                                    // (or connection) between different devices
  // with respect to their tasks in a common application.
  // The value 0 stands for "do not care".

  devTwitter.init
  (
    &socManNet,                 // Twitter needs a reference to the network
    loopCheck.refDateTime(),    // and a reference to a time string (RTC)
    (char *)SMN_TWITTER_NAME,   // and the name of the communication object
    3,                          // the number of Integer to send (<= 4)
    2,                          // the number of Floats to send (<= 4)
    1,                          // the number of Text strings to send (<= 4)
    normalSpeed                 // and the cyclic speed
  );                            // highSpeed=10Hz, normalSpeed=1Hz, lowSpeed=0.1Hz

  // There are more parameters with Twitter, which will be send
  //

  devTwitter.setDeviceName((char *) SMN_DEVICE_NAME);
  // Tell the world, who you are

  devTwitter.setDeviceState(12);    // status of your device (handbook)

  devTwitter.posX = 3667;           // X-position of your device
                                    // (local coordinates in cm)

  devTwitter.posY = 1088;           // Y-position of your device
                                    // (local coordinates in cm)

  devTwitter.posZ = 80;             // Z-position of your device
                                    // (local coordinates in cm)

  devTwitter.baseState = smpsInit;  // The state of your finite state machine
                                    // which you present to the world

  devTwitter.baseMode = 0;          // A number describing your plan/wish
                                    // (explain it in handbook)

  // Giving some default values for your twitter variables
  //
  appIntVar1 = 12;
  appIntVar2 = 345;
  appIntVar3 = -7;

  appFloatVar1 = 88.88;
  appFloatVar2 = 0.0758;

  appTextPtr1 = (char *) "Hi, it's me!";

  updateTwitter();

  // HINT:
  // The communication object name (here "MyFirstCommObj") and the data structure
  // (i.e. number of integer, float and text declared in Twitter.init) and
  // the meaning of their content are a matter of standardisation
  // (e.g. for the decentral homeautomation DHA)
  // The other parameters (e.g. device key) are mandatory for all devices.

  // There is no internal test for twitter running
  // so simply start Twitter and switch to next state
  //
  devTwitter.enabled = true;
  automat.enter(smInitFollower);
}

// ---------------------------------------------------------------------------
// Initialise Follower
// ---------------------------------------------------------------------------
//
void smInitFollower()
{
  devFollower.init(&socManNet, (char *) SMN_FOLLOWER_NAME);
  // with this example we follow "TestTwitter", which is the simple example
  // for Android smartphones on Github

  intMan1.recDsc.idx = 0;   // expect intMan1 as first int of TestTwitter
  intMan2.recDsc.idx = 1;   // expect intMan2 as second int of TestTwitter
  intMan3.recDsc.idx = 2;   // expect intMan3 as third int of TestTwitter

  floatMan1.recDsc.idx = 0; // expect floatMan1 as first float
  floatMan2.recDsc.idx = 1; // expect floatMan1 as second float

  textMan1.recDsc.idx = 0;  // only one text expected

  devFollower.enabled = true;   // start Follower

  automat.enter(smWaitForTestTwitter);
  automat.setTimeOut(5000);     // Time-out for next state 5 seconds
}

// ---------------------------------------------------------------------------
// Waiting for a telegram from TestTwitter
// ---------------------------------------------------------------------------
//
unsigned int recParseLastCount = 0;

void smWaitForTestTwitter()
{                                   // recParseCounter is incremented with
  if(                               // each telegram recognised by Follower
      devFollower.recParseCounter >     // So we wait here, until Follower
      (2 + recParseLastCount)           // received 3 telegrams more than
    )                                   // the latest marked value
  {
    devTwitter.baseState = smpsRun;     // Running with TestTwitter
    automat.enter(smDisplayValues,100); // Enter next state with delay 100 ms
    return;
  }

  if(automat.timeOut())
  {
    Serial.println("TestTwitter not detected");
    automat.enter(smStartWithoutTTW,10000); // Enter with delay 10 seconds
  }
}

// ---------------------------------------------------------------------------
// Show received values
// ---------------------------------------------------------------------------
//
unsigned int watchTestTwitter;

void smDisplayValues()
{
  if(automat.firstEnter())      // smDisplayValues will be repeated
  {                             // StateMachine.firstEnter() returns true
    watchTestTwitter = 0;       // only the first time a state is entered
    setTestTwitterTime = true;  // after another state was used
  }

  // For this example we will show only the value of the first integer value
  // received from Testwitter, when the content changed.
  //
  devFollower.getValue(&intMan1);
  if(intMan1.newValue)
  {
    Serial.println(intMan1.value);
  }

  // And we will display the text from TestTwitter, whenever a new telegram
  // comes in without respect to the change of content.
  //
  devFollower.getValue(&textMan1);
  if(textMan1.newPdu)
  {
    Serial.println(textMan1.value);
  }

  // Update the time in LoopCheck from TestTwitter on the minute
  //
  if(setTestTwitterTime)
  {
    loopCheck.getDateTime(&dt);
    if(dt.Second == 0)
    {
      loopCheck.setDateTime(devFollower.timeString);
      Serial.println(devFollower.timeString);
      setTestTwitterTime = false;
    }
  }

  // We know, that TestTwitter is sending once a second.
  // So it makes no sence, to ask every AutCycleTime for a new
  // value. It is quick enough, if we ask only ten times a second.
  //
  automat.setDelay(100);

  // Let us check the activity of TestTwitter again via recParseCounter
  // based on the status cycle time of 100 ms as defined by setDelay()
  //
  if(recParseLastCount != devFollower.recParseCounter)
  {
    // TestTwitter has sent a telegram
    recParseLastCount = devFollower.recParseCounter;
    watchTestTwitter = 0;
  }
  else
    watchTestTwitter++;

  if(watchTestTwitter > 30)
  {
    // no telegram from TestTwitter since 3 seconds
    //
    automat.enter(smStartWithoutTTW);
  }
}


// ---------------------------------------------------------------------------
// Start without TestTwitter
// ---------------------------------------------------------------------------
//
void smStartWithoutTTW()
{
  // With this simple example, the device has nothing to do if TestTwitter
  // is not present in the network. So here is another loop implemented
  // to wait until TestTwitter is online.
  //
  automat.setTimeOut(5000);
  automat.enter(smWaitForTestTwitter);
}

// ---------------------------------------------------------------------------
// Start without communication
// ---------------------------------------------------------------------------
//
void smStartWithoutSMN()
{
  automat.enter(smTAP1);
}


// ---------------------------------------------------------------------------
// Display SocManNet statistics
// ---------------------------------------------------------------------------
//
char statData[48];

void smDisplayStatisticsSMN()
{
  socManNet.getStatistic(statData);
  Serial.print(statData);
  automat.setDelay(1000);
}
*/

//Rumble States

//State Functions

void MotorControlSEC0()
{ 
    Serial.print("SEC0: ");
    if(distance < sec0_max)
      {
        
        if(distance < 50)
        {
          speed = 0;
        }
        else
        {
          temp_speed = (double) 2.5*distance - 125;
          speed = (int) temp_speed;
        }
        
        PWM -> PWM_ENA = PWM_ENA_CHID3;
        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(speed);
        
        SEC0 = true;
        MeasureAutomat.enter(SerialOutObstacle);
      }
      
      else if(distance < sec1_max)
      {
        temp_speed = (double) 2.5*distance - 375;
        speed = (int) temp_speed;

        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID2;
        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(speed);
        
        SEC0 = true;
        SEC1 = true;
        MeasureAutomat.enter(SerialOutObstacle);
      }
      else if(distance <= sec2_max)
      {
        
        temp_speed = (double) 2.5*distance - 625;
        speed = (int) temp_speed;
        
        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID1;
        REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);            
   
        SEC0 = true;
        SEC1 = true;
        SEC2 = true;
        MeasureAutomat.enter(SerialOutObstacle);
      }
      else if(distance > sec2_max)
      { 
        temp_speed = (double) 2.5*distance - 875;
        speed = (int) temp_speed;
       
        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
        REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID0;
        REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
        
        SEC0 = true;
        SEC1 = true;
        SEC2 = true;
        SEC3 = true;
        MeasureAutomat.enter(SerialOutObstacle);
      }
      else
      {
        SEC0 = true;
        SEC1 = true;
        SEC2 = true;
        SEC3 = true;
        MeasureAutomat.enter(SerialOutObstacle);
      }
  
}

void MotorControlSEC1()
{
    Serial.print("SEC1: ");
    if(distance < sec0_max)
    {
      MeasureAutomat.enter(SerialOutObstacle);
      //no action section already used
    }
      
    else if(distance < sec1_max)
    {
      temp_speed = (double) 2.5*distance - 375;
      speed = (int) temp_speed;
      
      PWM -> PWM_ENA = PWM_ENA_CHID2;
      REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(speed);              
  
      SEC1 = true;
      MeasureAutomat.enter(SerialOutObstacle);
    }
    else if(distance < sec2_max)
    {
      temp_speed = (double) 2.5*distance - 625;
      speed = (int) temp_speed;

      REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
      PWM -> PWM_ENA = PWM_ENA_CHID1;
      REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);              

      SEC1 = true;
      SEC2 = true;
      MeasureAutomat.enter(SerialOutObstacle);
    }
    else if(distance > sec2_max)
    { 
      temp_speed = (double) 2.5*distance - 875;
      speed = (int) temp_speed;

      REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
      REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
      PWM -> PWM_ENA = PWM_ENA_CHID0;
      REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
      
      SEC1 = true;
      SEC2 = true;
      SEC3 = true;
      MeasureAutomat.enter(SerialOutObstacle);
    }

    else
    {
      SEC1 = true;
      SEC2 = true;
      SEC3 = true;
      MeasureAutomat.enter(SerialOutObstacle);
    }
}

void MotorControlSEC2()
{
  Serial.print("SEC2: ");
  if(distance < sec1_max)
  {
    MeasureAutomat.enter(SerialOutObstacle);
    //no action section already used
  }

  else if(distance < sec2_max)
  {
    temp_speed = (double) 2.5*distance - 625;
    speed = (int) temp_speed;
  
    PWM -> PWM_ENA = PWM_ENA_CHID1;
    REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);              
    
    SEC2 = true;
    MeasureAutomat.enter(SerialOutObstacle);
  }
  else if(distance > sec2_max)
  { 
    temp_speed = (double) 2.5*distance - 875;
    speed = (int) temp_speed;
  
    REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
    PWM -> PWM_ENA = PWM_ENA_CHID0;
    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
    
    SEC2 = true;
    SEC3 = true;
    MeasureAutomat.enter(SerialOutObstacle);
  }

  else
  {
    SEC2 = true;
    SEC3 = true;
    MeasureAutomat.enter(SerialOutObstacle);
  }

}

void MotorControlSEC3()
{
  Serial.print("SEC3: ");
  if(distance < sec2_max)
  {
    MeasureAutomat.enter(SerialOutObstacle);
    //no action section already used
  }
  
  else if(distance > sec2_max)
  { 
    temp_speed = (double) 2.5*distance - 875;
    speed = (int) temp_speed;
  
    PWM -> PWM_ENA = PWM_ENA_CHID0;
    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
    
    SEC3 = true;
    MeasureAutomat.enter(SerialOutObstacle);
  }
  else
  {

    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(250);
    
    SEC3 = true;
    MeasureAutomat.enter(SerialOutObstacle);
  }
}


//Other Functions
void PWM_Setup()
{
    int default_speed = 250;
    //Ports
    PIOC -> PIO_PDR = PIO_PDR_P21 | PIO_PDR_P2 | PIO_PDR_P4 | PIO_PDR_P6 | PIO_PDR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Enable peripheral control of PC21

    //PWM Setup Use PWM 9 on Arduino pin 9
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | ~PMC_WPMR_WPEN; //Enable PMC write access
    PMC -> PMC_PCER1 = PMC_PCER1_PID36; //Enable PWM peripheral clock
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | PMC_WPMR_WPEN; //Disable PMC write access
    
    PIOC -> PIO_ABSR = PIO_ABSR_P21 | PIO_ABSR_P2 | PIO_ABSR_P4 | PIO_ABSR_P6 | PIO_ABSR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Select peripheral type B which is PWM Output

    PWM -> PWM_WPCR = PWM_WPCR_WPKEY(0x50574D) | 0x00; //Enable write access on all groups
    PWM -> PWM_CLK = PWM_CLK_PREA(1); //PREA_DIV  = 2
    REG_PWM_CMR4 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 

    //PWM Channel 0 - Pin 34 = PC2 (PWML0)
    REG_PWM_CMR0 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD0 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY0 = PWM_CDTY_CDTY(default_speed);   //default value

    //PWM Channel 1 - Pin 36 = PC4 (PWML1)
    REG_PWM_CMR1 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD1 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY1 = PWM_CDTY_CDTY(default_speed); //default value
  
    //PWM Channel 2 - Pin 38 = PC6 (PWML2)
    REG_PWM_CMR2 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD2 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY2 = PWM_CDTY_CDTY(default_speed); //default value

    // PWM Channel 3 - Pin 40 = PC8 (PWML3)
    REG_PWM_CMR3 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD3 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY3 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 4 - Pin 9 = PC21 (PWM4L)
    REG_PWM_CPRD4 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY4 = PWM_CDTY_CDTY(default_speed);

    //PWM Channel 5 - Pin 8 = PC22 (PWML5)
    REG_PWM_CMR5 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD5 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY5 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 6 - Pin 7 = PC23 (PWML6)
    REG_PWM_CMR6 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD6 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY6 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 7 - Pin 6 = PC24 (PWML7)
    REG_PWM_CMR7 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD7 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY7 = PWM_CDTY_CDTY(default_speed); //default value

    PWM -> PWM_ENA = PWM_ENA_CHID0 | PWM_ENA_CHID1 | PWM_ENA_CHID2 | PWM_ENA_CHID3 | PWM_ENA_CHID4 | PWM_ENA_CHID5 | PWM_ENA_CHID6 | PWM_ENA_CHID7;
}

void MotorOff()
{
  PWM -> PWM_DIS = PWM_DIS_CHID0 | PWM_DIS_CHID1 | PWM_DIS_CHID2 | PWM_DIS_CHID3 | PWM_DIS_CHID4 | PWM_DIS_CHID5 | PWM_DIS_CHID6 | PWM_DIS_CHID7;
}


