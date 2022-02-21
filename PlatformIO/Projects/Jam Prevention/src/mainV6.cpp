//Sync of two systems. Key difference: the messages are send multiple times and not only once. Idea: if the messages are send more often the chance that the receiver will sense the message rises.
//Problem: if the receiver reacts to early his acknowledge will be sent while the transmitter is still sending

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "StateMachine.h"

#define ListeningDuration 10
#define SendingDuration 1
#define MeasureDuration 2000
#define MessageLengthByte 8

#define DebugPin 9
#define SendingPin 4

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

long start {0};
long duration {0};

//Check if state has been entered for the first time since last run through
bool scan_first {true};
bool beacon_first {true};
bool detected_first {true};
bool sendack_first {true};
bool waitack_first {true};
bool collision_first {true};
bool checkslot_first {true};
bool waitslot_first {true};
bool takeslot_first {true};

//Determine measure priority
bool IsMaster {false};
bool LastInLine {false};

void setup() 
{
  Serial.begin(115200);
  for(int i = 0; i < MessageLengthByte; i++)
  {
    Serial.println(BeaconMessage[i], HEX);    
  }

  //Debugging
  pinMode(DebugPin, OUTPUT);
  digitalWrite(DebugPin, LOW);
  pinMode(SendingPin, OUTPUT);
  digitalWrite(SendingPin, LOW);

  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setAutoAck(false);
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
      
      scan_first = true;
      beacon_first = true;
      detected_first = true;
      collision_first = true;
      sendack_first = true;
      waitack_first = true;
      checkslot_first = true;
      waitslot_first = true;
      takeslot_first = true;

      //Debugging
      digitalWrite(SendingPin, LOW);

      radio.flush_rx();

      IsMaster = false;

      STATE = RadioScan;
      break;
    }

    case RadioScan:
    {
      if(scan_first)
      {
        //Serial.println("Scanning for other system");
        start = millis();
        scan_first = false;
        radio.openReadingPipe(1, address);
        radio.startListening();
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
            message_target = source_address;
            radio.flush_rx();
            STATE = OtherSystemDetected;
            break;
          }
          else if(messageID = CollisionID)
          {
            message_target = source_address;
            neighbor = message_target;
            radio.flush_rx();
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
      if(detected_first)
      {
        radio.stopListening();
        start = millis();
        duration = 0;

        CollisionMessage[4] = ((message_target & 0x00FF0000)>>16);
        CollisionMessage[5] = ((message_target & 0x0000FF00)>>8);
        CollisionMessage[6] = ((message_target & 0x000000FF));
      
        radio.openWritingPipe(address);

        //Serial.println("Sending distress signal");
        for(int i = 0; i < MessageLengthByte; i++)
        {
          Serial.println(CollisionMessage[i]);
        }

        detected_first = false;
      }      
      
      if(duration <= SendingDuration)
      {
        radio.write(&CollisionMessage, MessageLengthByte);  
      }
      else
      {
        STATE =  WaitForAcknowledge;  
      }
      break;
    }

    case SendAcknowledge:
    {
      if(sendack_first)
      {
        radio.stopListening();
        start = millis();
        duration = 0;

        AcknowledgeMessage[4] = ((message_target & 0x00FF0000)>>16);
        AcknowledgeMessage[5] = ((message_target & 0x0000FF00)>>8);
        AcknowledgeMessage[6] = ((message_target & 0x000000FF));
        
        radio.openWritingPipe(address);
      
        //Debugging
        //Serial.println("Sending acknowledge");
        for(int i = 0; i < MessageLengthByte; i++)
        {
            Serial.println(AcknowledgeMessage[i]);
        }
        sendack_first = false;
      }

      if(duration <= SendingDuration)
      {
        radio.write(&AcknowledgeMessage, MessageLengthByte);  
      }
      else
      {
        STATE =  CollisionDetected;  
      }
      break;
    }

    case WaitForAcknowledge:
    {
      if(waitack_first)
      {
          //Serial.println("Waiting for acknowledge");
          waitack_first = false;
          radio.openReadingPipe(1, address);
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
            radio.flush_rx();
            STATE = CollisionDetected;
            neighbor = message_target;
            IsMaster = true;
            break;
        }
      }
      break;
    }

    case CollisionDetected:
    {
      if(collision_first)
      {
        //Serial.println("Collision has been detected");
        Serial.println("Sync successfull");
        collision_first = false;
      }

      if(IsMaster)
      {
        STATE = TakeTimeSlot;
        break;
      }
      else
      {
        STATE = CheckForTimeSlot;
        break;
      }


      break;
    }

    case RadioBeacon:
    {
      if(beacon_first)
      {
        //Serial.println("Sending");
        start = millis();
        duration = 0;
        radio.stopListening();
        radio.openWritingPipe(address);
        beacon_first = false;
        digitalWrite(SendingPin, HIGH);
      }

      if(duration <= SendingDuration)
      {
        radio.write(&BeaconMessage, MessageLengthByte);  
      }
      else
      {
        STATE = RadioReset;  
      }
      break;
    }

    case CheckForTimeSlot:
    {
      if(checkslot_first)
      {
        //Serial.println("Scanning for other system");
        start = millis();
        duration = 0;
        checkslot_first = false;
        radio.openReadingPipe(1, address);
        radio.startListening();
        
        //Debugging
        digitalWrite(DebugPin, LOW);
        Serial.println("Waiting");
        
        waitslot_first = true;
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
          radio.flush_rx();
          STATE = WaitForTimeSlot;
        }
      }

      if(duration > ListeningDuration)
      {
        STATE = RadioReset;
        num_of_systems = 0;
      }
      
      break;
    }

    case WaitForTimeSlot:
    {
      if(waitslot_first)
      {
        start = millis();
        duration = 0;
        waitslot_first = false;
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
        start = millis();
        duration = 0;

        Serial.println("Measuring");
        digitalWrite(DebugPin, HIGH);

        takeslot_first = false;
        checkslot_first = true;
        waitslot_first = true;
      }

      if(duration <= SendingDuration)
      {
        radio.write(&MeasureMessage, MessageLengthByte);
        break;
      }

      else if(duration >= MeasureDuration)
      {
        radio.flush_rx();
        STATE = CheckForTimeSlot;
        break;
      }
      break;
    }

    default:
    {
      break;
    }

  }
}
