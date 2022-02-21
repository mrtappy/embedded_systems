#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "StateMachine.h"

#define ListeningDuration 1000
#define SendingDuration 100
#define AcknowledgeDuration 100
#define MeasureDuration 10000
#define bit32toByte 4
#define MaxNumOfSystems 5

#define DebugPin 9

#define BeaconID 0x24ul
#define CollisionID 0xDBul
#define AcknowledgeID 0x81ul
#define MeasureID 0x18ul

#ifndef DeviceID
#define DeviceID 1ul
#endif

//RF24 Object initialization
RF24 radio(7, 10); // CE, CSN

//Variables

const byte address[6] {"00001"};

const uint32_t BeaconMessage {(BeaconID << 24) | DeviceID};
const uint32_t CollisionMessage {(CollisionID << 24) | DeviceID};
const uint32_t AcknowledgeMessage {(AcknowledgeID << 24) | DeviceID};
const uint32_t MeasureMessage {(MeasureID << 24) | DeviceID};

int STATE {RadioReset};
int num_of_systems {0};
uint32_t system_list[MaxNumOfSystems] {0};
uint32_t neighbor {0xFFFFFFFF};
int i {0};

long start {0};
long duration {0};

bool first {true};
bool collision_first {true};
bool timeslot_first {true};
bool wait_first {true};
bool takeslot_first {true};
bool IsMaster {false};

//Function Prototypes
bool CheckSystemList(int system_address, uint32_t system_list[MaxNumOfSystems], int num_of_systems);
void AddSystem(int system_address, bool IsMaster = false);

void setup() 
{
  Serial.begin(115200);
  Serial.println(BeaconMessage, HEX);

  pinMode(DebugPin, OUTPUT);
  digitalWrite(DebugPin, LOW);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  Serial.println("Starting");
  start = millis();
}

void loop() 
{
  duration = millis() - start;

  switch(STATE)
  {
    case RadioReset:
    {
      Serial.println("Starting");
      
      first = true;
      collision_first = true;
      IsMaster = false;
      i = 0;

      radio.startListening();
      STATE = RadioScan;
      break;
    }

    case RadioScan:
    {
      if(first)
      {
        //Serial.println("Scanning for other system");
        start = millis();
        first = false;
        break;
      }

      if(duration <= ListeningDuration)
      {  
        if (radio.available()) 
        {  
          uint32_t received {0};
          uint32_t address {0};
          radio.read(&received, bit32toByte);
          address = received &  0x00FFFFFF;
          received = (received >> 24);

          //Debugging
          Serial.println(received, HEX);
          Serial.println(address, HEX);

          if(received == AcknowledgeID)
          {
            STATE = CollisionDetected;
            if(CheckSystemList(address, system_list, num_of_systems) == false)
            {
                AddSystem(address);
            }
          }
          
          if(received == BeaconID)
          { 
            STATE = OtherSystemDetected;
            if(CheckSystemList(address, system_list, num_of_systems) == false)
            {
                IsMaster = true;
                AddSystem(address, IsMaster);     //I am the master
            }
          }
        }
      }

      else
      {
        start = millis();
        STATE = RadioBeacon;
      }
      break;
    }

    case OtherSystemDetected:
    { 
      radio.stopListening();
      Serial.println("Sending distress signal");
      radio.write(&AcknowledgeMessage, bit32toByte);
      i++;
      if(i >= 5)
      {
        STATE = CollisionDetected;  
      }
      break;
    }

    case CollisionDetected:
    {
      if(collision_first)
      {
        Serial.println("Collision has been detected");  
        collision_first = false;

        //Debugging
        Serial.println(system_list[0]);
        Serial.println(num_of_systems);
        Serial.println(neighbor);

        if(IsMaster)
        {
          Serial.println("I am the master");
        }
      }

      if(IsMaster)
      {
        STATE = TakeTimeSlot;
      }
      else
      {
        STATE = CheckForTimeSlot;
      }
      
      break;
    }

    case RadioBeacon:
    {
      radio.stopListening();
      radio.write(&BeaconMessage, bit32toByte);
      STATE = RadioReset;
      break;
    }

    case CheckForTimeSlot:
    {
      if(timeslot_first)
      {
        //Serial.println("Scanning for other system");
        start = millis();
        timeslot_first = false;
        radio.startListening();
        
        //Debugging
        digitalWrite(DebugPin, LOW);
        Serial.println("Waiting");
        
        takeslot_first = true;
        break;
      }
  
      if (radio.available()) 
      {  
        uint32_t received {0};
        uint32_t address {0};
        radio.read(&received, bit32toByte);
        address = received &  0x00FFFFFF;
        received = (received >> 24);

        //Debugging
        Serial.println(received, HEX);
        Serial.println(address, HEX);

        if(received == MeasureID && address == neighbor) 
        {
          STATE = WaitForTimeSlot;
        }
      }

      if(duration > ListeningDuration)
      {
        STATE = RadioBeacon;
        num_of_systems = 0;
      }
      
      break;
    }

    case WaitForTimeSlot:
    {
      if(wait_first)
      {
        start = millis();
        wait_first = false;
        break;
      }

      if(duration >= MeasureDuration)
      {
        STATE = TakeTimeSlot;
      }
      break;
    }

    case TakeTimeSlot:
    {
      if(takeslot_first)
      {
        radio.stopListening();
        radio.write(&MeasureMessage, bit32toByte);
        start = millis();
        
        Serial.println("Measuring");
        digitalWrite(DebugPin, HIGH);

        takeslot_first = false;
        timeslot_first = true;
        wait_first = true;

        break;
      }

      if(duration >= MeasureDuration)
      {
        STATE = CheckForTimeSlot;
      }

      break;
    }

    default:
    {
      break;
    }
  } 
}

bool CheckSystemList(int system_address, uint32_t system_list[MaxNumOfSystems], int num_of_systems)
{
    for(int i = 0; i < num_of_systems; i++)
    {
        if(system_list[i] == system_address)
        {
            return true;
        }
    }
    return false;
}

void AddSystem(int system_address, bool IsMaster)
{
  if(IsMaster)
  {
    neighbor = 0;
    if(system_address > neighbor)
    {
      neighbor = system_address;
    }
  }
  else
  {
    if(system_address < neighbor)
    {
      neighbor = system_address;
    }
  }
  
  system_list[num_of_systems] = system_address;
  num_of_systems++;
}