#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "StateMachine.h"

#define ListeningDuration 1000
#define SendingDuration 100
#define AcknowledgeDuration 100
#define MeasureDuration 10000
#define MessageLengthByte 8
#define MaxNumOfSystems 5

#define DebugPin 9

#define BeaconID 0x24
#define CollisionID 0xDB
#define AcknowledgeID 0x81
#define MeasureID 0x18

#ifndef DeviceID
//#define DeviceID 1ul
#define DeviceID 0x01ul
#endif

//RF24 Object initialization
RF24 radio(7, 10); // CE, CSN

//Variables

const byte address[6] {"00001"};

const uint8_t BeaconMessage[] 
{   BeaconID, 
    ((DeviceID & 0x00FF0000)>>16),
    ((DeviceID & 0x0000FF00)>>8),
    ((DeviceID & 0x0000FF)),
    0, 
    0, 
    0, 
    1                              };

uint8_t CollisionMessage[]    
{   CollisionID, 
    ((DeviceID & 0x00FF0000)>>16),
    ((DeviceID & 0x0000FF00)>>8),
    ((DeviceID & 0x0000FF)),
    0, 
    0, 
    0, 
    1                              };

uint8_t AcknowledgeMessage[]
{   AcknowledgeID, 
    ((DeviceID & 0x00FF0000)>>16),
    ((DeviceID & 0x0000FF00)>>8),
    ((DeviceID & 0x0000FF)),
    0, 
    0, 
    0, 
    1                              };

uint8_t MeasureMessage[]
{   MeasureID, 
    ((DeviceID & 0x00FF0000)>>16),
    ((DeviceID & 0x0000FF00)>>8),
    ((DeviceID & 0x0000FF)),
    0, 
    0, 
    0, 
    1                              };

int STATE {RadioReset};
int num_of_systems {1};
uint32_t message_target {0};
uint32_t neighbor {0x00FFFFFF};
int i {0};

long start {0};
long duration {0};

bool first {true};
bool collision_first {true};
bool waitack_first {true};
bool timeslot_first {true};
bool wait_first {true};
bool takeslot_first {true};
bool IsMaster {false};

// //Function Prototypes
// bool CheckSystemList(int system_address, uint32_t system_list[MaxNumOfSystems], int num_of_systems);
// void AddSystem(int system_address, bool IsMaster = false);

void setup() 
{
  Serial.begin(115200);
  for(int i = 0; i < MessageLengthByte; i++)
  {
    Serial.println(BeaconMessage[i], HEX);    
  }

  pinMode(DebugPin, OUTPUT);
  digitalWrite(DebugPin, LOW);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setAutoAck(0, false);
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
      waitack_first = true;
      takeslot_first = true;
      timeslot_first = true;
      wait_first = true;
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
        radio.openReadingPipe(0, address);
        break;
      }

      if(duration <= ListeningDuration)
      {  
        if (radio.available()) 
        {  
          uint8_t received [MessageLengthByte] {0};
          uint32_t source_address {0};
          uint32_t target_address {0};
          uint8_t messageID {0};
          
          radio.read(&received, MessageLengthByte);
          
          messageID = received[0];
          source_address = (received[1]<<16) | (received[2]<<8) | received[3];
          target_address = (received[4]<<16) | (received[5]<<8) | received[6];

          //Debugging
          Serial.println(messageID, HEX);
          Serial.println(source_address, HEX);
          Serial.println(target_address, HEX);
          
          if(messageID == BeaconID)
          {
              //Collision has been detected
              message_target = source_address;
              STATE = OtherSystemDetected;
              break;
          }
          
          if(messageID ==  CollisionID && target_address == DeviceID)
          {
            message_target = source_address;
            STATE = SendAcknowledge;
            break;
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

      CollisionMessage[4] = ((message_target & 0x00FF0000)>>16);
      CollisionMessage[5] = ((message_target & 0x0000FF00)>>8);
      CollisionMessage[6] = ((message_target & 0x000000FF));
      
      radio.openWritingPipe(address);
      radio.write(&CollisionMessage, MessageLengthByte);

      //Debugging
      Serial.println("Sending distress signal");
      for(int i = 0; i < MessageLengthByte; i++)
      {
          Serial.println(CollisionMessage[i]);
      }
      STATE =  WaitForAcknowledge;
      break;
    }

    case WaitForAcknowledge:
    {
        if(waitack_first)
        {
            Serial.println("Waiting for acknowledge");
            waitack_first = false;
            radio.openReadingPipe(0, address);
            radio.startListening();
        }
        
        if (radio.available()) 
        {  
            uint8_t received [MessageLengthByte] {0};
            uint32_t source_address {0};
            uint32_t target_address {0};
            uint8_t messageID {0};
            
            radio.read(&received, MessageLengthByte);
            
            messageID = received[0];
            source_address = (received[1]<<16) | (received[2]<<8) | received[3];
            target_address = (received[4]<<16) | (received[5]<<8) | received[6];

            //Debugging
            Serial.println(messageID, HEX);
            Serial.println(source_address, HEX);
            Serial.println(target_address, HEX);
            
            if(messageID == AcknowledgeID && source_address == message_target)
            {
                //Successfull sync
                neighbor = message_target;
                IsMaster = true;
                STATE = CollisionDetected;
                break;
            }
            else if(messageID == BeaconID && source_address == message_target)
            {
                //resend collision message
                STATE =  OtherSystemDetected;
                waitack_first = true;
            }
        }
        break;
    }

    case SendAcknowledge:
    {
      radio.stopListening();

      AcknowledgeMessage[4] = ((message_target & 0x00FF0000)>>16);
      AcknowledgeMessage[5] = ((message_target & 0x0000FF00)>>8);
      AcknowledgeMessage[6] = ((message_target & 0x000000FF));
      
      radio.openWritingPipe(address);
      radio.write(&AcknowledgeMessage, MessageLengthByte);

      //Debugging
      Serial.println("Sending acknowledge");
      for(int i = 0; i < MessageLengthByte; i++)
      {
          Serial.println(AcknowledgeMessage[i]);
      }
      neighbor = message_target;
      STATE =  CollisionDetected;
      break;
    }

    case CollisionDetected:
    {
      if(collision_first)
      {
        Serial.println("Collision has been detected");  
        collision_first = false;
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
      radio.openWritingPipe(address);
      radio.write(&BeaconMessage, MessageLengthByte);
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
        radio.openReadingPipe(0, address);
        radio.startListening();
        

        //Debugging
        digitalWrite(DebugPin, LOW);
        Serial.println("Waiting");
        
        wait_first = true;
        takeslot_first = true;
        break;
      }
  
      if (radio.available()) 
      {  
        uint8_t received [MessageLengthByte] {0};
        uint32_t source_address {0};
        uint32_t target_address {0};
        uint8_t messageID {0};
            
        radio.read(&received, MessageLengthByte);
            
        messageID = received[0];
        source_address = (received[1]<<16) | (received[2]<<8) | received[3];
        target_address = (received[4]<<16) | (received[5]<<8) | received[6];

        //Debugging
        Serial.println(messageID, HEX);
        Serial.println(source_address, HEX);
        Serial.println(target_address, HEX);
        
        if(messageID == MeasureID && source_address == neighbor) 
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
        radio.openWritingPipe(address);
        radio.write(&MeasureMessage, MessageLengthByte);
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



// bool CheckSystemList(int system_address, uint32_t system_list[MaxNumOfSystems], int num_of_systems)
// {
//     for(int i = 0; i < num_of_systems; i++)
//     {
//         if(system_list[i] == system_address)
//         {
//             return true;
//         }
//     }
//     return false;
// }

// void AddSystem(int system_address, bool IsMaster)
// {
//   if(IsMaster)
//   {
//     neighbor = 0;
//     if(system_address > neighbor)
//     {
//       neighbor = system_address;
//     }
//   }
//   else
//   {
//     if(system_address < neighbor)
//     {
//       neighbor = system_address;
//     }
//   }
  
//   system_list[num_of_systems] = system_address;
//   num_of_systems++;
// }