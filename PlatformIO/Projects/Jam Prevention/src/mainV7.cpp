//Version 7 is currently updated and used for uploading and testing.
//Calling it version 7 is unfortunate but happedn since a symlink of version 7 was created to use the same source code for every (of the currently three) systems

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "StateMachine.h"

#define ListeningDuration 10
#define SendingDuration 1
#define DistressDuration 10
#define SyncDuration 10
#define WaitAckDuration 10
#define CheckSlotDuration 4000
#define MeasureDuration 2000
#define MaxNumOfTakeRetries 100
#define MessageLengthByte 8

#define DebugPin 9
#define SendingPin 4

#define BeaconID 0x24
#define CollisionID 0xDB
#define AcknowledgeID 0x81
#define MeasureID 0x18
#define RequestID 0xBD
#define AnswerID 0x42

#ifndef DeviceID
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

uint8_t RequestMessage[]
{ RequestID,
  ((DeviceID & 0x00FF0000)>>16),
  ((DeviceID & 0x0000FF00)>>8),
  ((DeviceID & 0x0000FF)),
  0, 
  0, 
  0, 
  1                              };

uint8_t AnswerMessage[]
{ AnswerID,
  ((DeviceID & 0x00FF0000)>>16),
  ((DeviceID & 0x0000FF00)>>8),
  ((DeviceID & 0x0000FF)),
  0, 
  0, 
  0, 
  1                              };



int STATE {RadioReset};
unsigned int OLD_STATE {0xFFFF};
int num_of_systems {1};
int my_position {1};
int take_retries {0};
int request_position {0};
uint32_t message_target {0};
uint32_t neighbor_up {0x00FFFFFF};
uint32_t neighbor_down {0x00FFFFFF};

//Debugging
int check_runs {0};

long start {0};
long duration {0};

//Check if state has been entered for the first time since last run through
bool scan_first {true};
bool beacon_first {true};
bool detected_first {true};
bool waitack_first {true};
bool collision_first {true};
bool checkslot_first {true};
bool waitslot_first {true};
bool holdslot_first {true};
bool waitsync_first {true};
bool neighborrequest_first {true};
bool neighborlisten_first {true};
bool neighborwaitrequest_first {true};
bool neighborwaitacknowledge_first {true};
bool waitinsertion_first {true};
bool answerrequest_first {true};

//Determine measure priority
bool IsMaster {false};
bool LastInLine {false};
bool I_am_done {false};

//If this variable is treu the system which is last in line will save the address of 
//the system which is first in line after it received the acknowledge for the measure message
bool first_contact {true};

//Determine what the acknowledge is for
bool SyncAck {false};
bool MeasureAck {false};

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

  delayMicroseconds(1500);

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
      waitack_first = true;
      checkslot_first = true;
      waitslot_first = true;
      holdslot_first = true;
      waitsync_first = true;
      neighborrequest_first = true;
      neighborlisten_first = true,
      neighborwaitrequest_first = true;
      neighborwaitacknowledge_first = true;
      waitinsertion_first = true;
      answerrequest_first = true;

      //Debugging
      digitalWrite(SendingPin, LOW);

      radio.flush_rx();

      IsMaster = false;
      LastInLine = false;
      first_contact = true;
      I_am_done = false;

      SyncAck = false;
      MeasureAck = false;

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
          uint8_t systems {0};
          
          radio.read(&received, MessageLengthByte);
          
          messageID = received[0];
          source_address = (received[1]<<16) | (received[2]<<8) | received[3];
          target_address = (received[4]<<16) | (received[5]<<8) | received[6];
          systems = received[7];

          //Debugging
          Serial.println(messageID, HEX);
          Serial.println(source_address, HEX);
          Serial.println(target_address, HEX);
          Serial.println(systems);

          if(messageID == BeaconID)
          {
            message_target = source_address;
            neighbor_down = message_target;

            radio.flush_rx();
            SyncAck = true;
            STATE = OtherSystemDetected;
            break;
          }
          else if(messageID = CollisionID)
          {
            message_target = source_address;
            neighbor_up = message_target;

            if(systems == 1)
            {
              neighbor_down = message_target;
            }

            radio.flush_rx();
            SyncAck = true;
            STATE = SendAcknowledge;
            LastInLine = true;

            num_of_systems = systems+1;
            my_position = systems+1;

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
        CollisionMessage[7] = num_of_systems;
      
        radio.openWritingPipe(address);

        //Debugging
        Serial.println("Sending distress signal");
        // for(int i = 0; i < MessageLengthByte; i++)
        // {
        //   Serial.println(CollisionMessage[i], HEX);
        // }
        detected_first = false;
      }

      if(duration >= SendingDuration)
      {
        radio.write(&CollisionMessage, MessageLengthByte);
        STATE = WaitForAcknowledge;
      }
        
      break;
    }

    case SendAcknowledge:
    {
      radio.stopListening();
      start = millis();
      duration = 0;

      if(SyncAck)
      {
        //Debugging
        Serial.println("SyncAck");

        AcknowledgeMessage[4] = ((message_target & 0x00FF0000)>>16);
        AcknowledgeMessage[5] = ((message_target & 0x0000FF00)>>8);
        AcknowledgeMessage[6] = ((message_target & 0x000000FF));
        AcknowledgeMessage[7] = my_position;
        STATE =  CollisionDetected;
        SyncAck = false;
      }

      else if(MeasureAck)
      {
        //Debugging
        Serial.println("MeasureAck");

        AcknowledgeMessage[4] = ((neighbor_up & 0x00FF0000)>>16);
        AcknowledgeMessage[5] = ((neighbor_up & 0x0000FF00)>>8);
        AcknowledgeMessage[6] = ((neighbor_up & 0x000000FF));
        AcknowledgeMessage[7] = my_position; 
        MeasureAck = false;
        STATE = WaitForTimeSlot;
      }
      
      radio.openWritingPipe(address);
    
      //Debugging
      Serial.println("Sending acknowledge");
      // for(int i = 0; i < MessageLengthByte; i++)
      // {
      //     Serial.println(AcknowledgeMessage[i], HEX);
      // }
        
      radio.write(&AcknowledgeMessage, MessageLengthByte);  
      
      break;
    }

    case WaitForAcknowledge:
    {
      if(waitack_first)
      {
        start = millis();
        duration = 0;
        
        //Debugging
        if(SyncAck)
        {
          Serial.println("Waiting for sync acknowledge");
        }
        else if(MeasureAck)
        {
          Serial.println("Waiting for measure acknowledge");
        }
        
        waitack_first = false;
        
        radio.openReadingPipe(1, address);
        radio.startListening();
      }
      
      if(duration <= WaitAckDuration)
      {
        if(radio.available()) 
        {  
          uint8_t received [MessageLengthByte] {0};
          uint32_t source_address {0};
          uint32_t target_address {0};
          uint8_t messageID {0};
          uint8_t systems {0};
          
          radio.read(&received, MessageLengthByte);
          
          messageID = received[0];
          source_address = (received[1]<<16) | (received[2]<<8) | received[3];
          target_address = (received[4]<<16) | (received[5]<<8) | received[6];
          systems = received[7];

          //Debugging
          Serial.println(messageID, HEX);
          Serial.println(source_address, HEX);
          Serial.println(target_address, HEX);
          Serial.println(systems);
          
          if(messageID == AcknowledgeID)
          {
            //Successfull sync
            radio.flush_rx();
            
            if(SyncAck && (source_address == message_target))
            {
              SyncAck = false;
              if(LastInLine == false)
              {
                STATE = CollisionDetected;
                waitack_first = true;
                
                neighbor_up = message_target;
                IsMaster = true;
                num_of_systems++;
                my_position = 1;
              }
              else
              {
                waitack_first = true;
                LastInLine = false;
                checkslot_first = true;
                waitslot_first = true;
                holdslot_first = true;
                
                neighbor_down = message_target;
                num_of_systems++;
                
                STATE = OLD_STATE;
                Serial.println("Here I am");
                Serial.println(STATE);            
              }
            }
            else if(MeasureAck && (target_address == DeviceID))
            {
              MeasureAck = false;
              waitack_first = true;
              if(first_contact)
              {
                neighbor_down = source_address;
                first_contact = false;
              }
              STATE = HoldTimeSlot;
            }
            break;
          }
          else if(messageID == BeaconID && LastInLine && MeasureAck)
          {
            radio.flush_rx();
            detected_first = true;
            waitack_first = true;
            OLD_STATE = STATE;
            STATE = OtherSystemDetected;
            SyncAck = true;
            message_target = source_address;
            break;
          }
          else if(messageID == BeaconID && LastInLine == false && MeasureAck)
          {
            radio.flush_rx();
            OLD_STATE = STATE;
            message_target = source_address;
            STATE =  WaitForSync; 
            break;
          }
        }
      }
      else
      {
        if(SyncAck)
        {
          STATE = OtherSystemDetected;
          detected_first = true;
          waitack_first = true;
        }
        else if(MeasureAck)
        {
          waitack_first = true;
          STATE = TakeTimeSlot;
        }
        break;
      }

      break;
    }

    case CollisionDetected:
    {
      if(collision_first)
      {
        //Serial.println("Collision has been detected");
        Serial.println("Sync successfull");
        answerrequest_first = true;
        collision_first = false;
      }

      //If the neighbor changes the measure message needs to be updated!!!!!!!!!!!!!
      MeasureMessage[4] = ((neighbor_down & 0x00FF0000)>>16);
      MeasureMessage[5] = ((neighbor_down & 0x0000FF00)>>8);
      MeasureMessage[6] = ((neighbor_down & 0x000000FF));
      MeasureMessage[7] = my_position;

      if(IsMaster)
      {
        STATE = TakeTimeSlot;
        break;
      }
      else
      {
        STATE = CheckForTimeSlot;
        checkslot_first = true;
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
        checkslot_first = false;
        radio.openReadingPipe(1, address);
        radio.startListening();
        
        //Debugging
        digitalWrite(DebugPin, LOW);
        Serial.println("Checking");

        Serial.println(neighbor_up, HEX);
        Serial.println(neighbor_down, HEX);
        
        start = millis();
        duration = 0;

        message_target = 0xFFFFFFFF;
        waitslot_first = true;
        holdslot_first = true;
      }
    
      if(duration <= CheckSlotDuration)
      {
        check_runs++;
        if(radio.available()) 
        {  
          uint8_t received [MessageLengthByte] {0};
          uint32_t source_address {0};
          uint32_t target_address {0};
          uint8_t messageID {0};
          uint8_t systems {0};
              
          radio.read(&received, MessageLengthByte);
              
          messageID = received[0];
          source_address = (received[1]<<16) | (received[2]<<8) | received[3];
          target_address = (received[4]<<16) | (received[5]<<8) | received[6];
          systems = received[7];

          //Debugging
          Serial.println(messageID, HEX);
          Serial.println(source_address, HEX);
          Serial.println(target_address, HEX);
          
          if(messageID == MeasureID && source_address == neighbor_up) 
          {
            radio.flush_rx();
            STATE = SendAcknowledge;
            MeasureAck = true;
            break;
          }
          else if(messageID == MeasureID && source_address != neighbor_up)
          {
            I_am_done = false;
            start = millis();
            duration = 0;
            Serial.println("Some one else is measuring");
            break;
          }

          else if(messageID == BeaconID && LastInLine)
          {
            radio.flush_rx();
            detected_first = true;
            waitack_first = true;
            OLD_STATE = STATE;
            STATE = OtherSystemDetected;
            SyncAck = true;
            message_target = source_address;
            break;
          }
          else if(messageID == BeaconID && LastInLine == false)
          {
            radio.flush_rx();
            OLD_STATE = STATE;
            message_target = source_address;
            STATE =  WaitForSync; 
            break;
          }
          else if(messageID == CollisionID && target_address == DeviceID)
          {
            MeasureAck = true;
            STATE = SendAcknowledge;
            checkslot_first = true; 
            break;
          }
          else if(messageID == CollisionID && target_address != DeviceID)
          {
            STATE = WaitForSync;
            checkslot_first = true; 
            break;
          }
          else if(messageID == AnswerID)
          {
            if(source_address == neighbor_up)
            {
              STATE = SendNeighborAcknowledge;
            }
            else
            {
              STATE = WaitForInsertion;
            }
          }
          else if(messageID == RequestID)
          {
            STATE = WaitForNeighborRequest;
          }
        }
      }
      else
      {
        Serial.print("Checkslot duration exceeded ");
        Serial.print(duration);
        Serial.print(" ");
        Serial.println(check_runs);

        if(I_am_done)
        {
          STATE = NeighborRequest;
          if(my_position + 2 <= num_of_systems)
          {
            request_position = my_position + 2;
          }
          else
          {
            request_position = 1;
          }  
        }
        else
        {
          STATE = WaitForNeighborRequest;
        }
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
        Serial.println("Waiting");

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
        
        if(messageID == BeaconID && LastInLine)
        {
          radio.flush_rx();
          detected_first = true;
          waitack_first = true;
          OLD_STATE = STATE;
          STATE = OtherSystemDetected;
          SyncAck = true;
          message_target = source_address;
          break;
        }
        else if(messageID == BeaconID && LastInLine == false)
        {
          radio.flush_rx();
          OLD_STATE = STATE;
          message_target = source_address;
          STATE =  WaitForSync; 
          break;
        }
        else if(messageID == MeasureID && source_address == neighbor_up) 
        {
          radio.flush_rx();
          STATE = SendAcknowledge;
          MeasureAck = true;
          waitslot_first = true;
          break;
        }
      }

      if(duration >= MeasureDuration)
      {
        STATE = TakeTimeSlot;
      }
      break;
    }

    case TakeTimeSlot:
    {
      if(take_retries > MaxNumOfTakeRetries)
      {
        take_retries = 0;
        if(my_position + 2 <= num_of_systems)
        {
          request_position = my_position + 2;
        }
        else
        {
          request_position = 1;
        }  
        STATE = NeighborRequest;
        break;
      }
      radio.stopListening();
      radio.openWritingPipe(address);
        
      Serial.println("Measuring");

      checkslot_first = true;
      waitslot_first = true;
      
      radio.write(&MeasureMessage, MessageLengthByte);
      
      MeasureAck = true;
      take_retries++;
      STATE = WaitForAcknowledge;
  
      break;
    }

    case HoldTimeSlot:
    {
      if(holdslot_first)
      {
        Serial.println("Holding");
        digitalWrite(DebugPin, HIGH);
        
        start = millis();
        duration = 0;
        take_retries = 0;

        radio.openReadingPipe(1, address);
        radio.startListening();
        
        holdslot_first = false;
      }

      if(radio.available()) 
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
        
        if(messageID == BeaconID && LastInLine)
        {
          radio.flush_rx();
          detected_first = true;
          waitack_first = true;
          OLD_STATE = STATE;
          STATE = OtherSystemDetected;
          SyncAck = true;
          message_target = source_address;
          break;
        }
        else if(messageID == BeaconID && LastInLine == false)
        {
          radio.flush_rx();
          OLD_STATE = STATE;
          message_target = source_address;
          STATE =  WaitForSync; 
          break;
        }
      }

      if(duration >= MeasureDuration)
      {
        radio.flush_rx();
        STATE = CheckForTimeSlot;
        I_am_done = true;
        break;
      }

      break;
    }

    case WaitForSync:
    {
      if(waitsync_first)
      {
        start = millis();
        duration = 0;
        waitsync_first = false;

        checkslot_first = true;
        waitslot_first = true;
        holdslot_first = true;

        Serial.println("Waiting for sync");

        radio.openReadingPipe(1, address);
        radio.startListening();
      }

      // if(duration <= SyncDuration)
      // {
        if(radio.available()) 
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
            STATE = OLD_STATE;
            waitsync_first = true;
            if(IsMaster)
            {
              neighbor_up = message_target;  
            }
            break;
          }
          else if(messageID = CollisionID && target_address == message_target)
          {
            start = millis();
            break;
          }
        }
      // }
      // else
      // {
      //   STATE = RadioReset;
      //   break;
      // }

      break;
    }

    case NeighborRequest:
    {
      if(neighborrequest_first)
      { 
        RequestMessage[4] = ((neighbor_up & 0x00FF0000)>>16);
        RequestMessage[5] = ((neighbor_up & 0x0000FF00)>>8);
        RequestMessage[6] = ((neighbor_up & 0x000000FF));
        
        if(request_position > num_of_systems)
        {
          request_position = 1;
        }

        RequestMessage[7] = request_position;

        //Debugging
        Serial.println("Requesting neighbor");
        
        start = millis();
        duration = 0;

        radio.stopListening();
        radio.openWritingPipe(address);
        
        neighborrequest_first = false;
        neighborlisten_first = true;
      }

      if(duration <= SendingDuration)
      {
        radio.write(&RequestMessage, MessageLengthByte);  
      }
      else
      {
        STATE = ListenForNeighbor;  
      }
      break;
    }

    case ListenForNeighbor:
    {
      if(neighborlisten_first)
      {
        radio.openReadingPipe(1, address);
        radio.startListening();

        //Debugging
        Serial.println("Listening for neighbor");
        
        start = millis();
        duration = 0;

        neighborrequest_first = true;
        neighborlisten_first = false;
      }

      if(duration <= ListeningDuration)
      {
        if(radio.available())
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
          
          if(messageID == AnswerID && target_address == DeviceID)
          {
            neighbor_down = source_address;
            message_target = source_address;
            LastInLine = true;
            STATE = SendNeighborAcknowledge;
          }
          else if(messageID == RequestID)
          {
            message_target = source_address;
            STATE = AnswerNeighborRequest;
            neighborlisten_first = true;
            break;
          }
        }
      }
      else
      {
        STATE = NeighborRequest;
        request_position++;
      }
      
      break;
    }

    case SendNeighborAcknowledge:
    {
      //Debugging
      Serial.println("Sending acknowledge to neighbor");
      
      neighborlisten_first = true;
      
      AcknowledgeMessage[4] = ((message_target & 0x00FF0000)>>16);
      AcknowledgeMessage[5] = ((message_target & 0x0000FF00)>>8);
      AcknowledgeMessage[6] = ((message_target & 0x000000FF));
      AcknowledgeMessage[7] = my_position;
        
      start = millis();
      duration = 0;

      radio.stopListening();
      radio.openWritingPipe(address);

      radio.write(&AcknowledgeMessage, MessageLengthByte);
      LastInLine = true;
      collision_first = true;
      checkslot_first = true;
      STATE = CollisionDetected;
      
      break;
    }

    case WaitForNeighborRequest:
    {
      if(neighborwaitrequest_first)
      {
        radio.openReadingPipe(1, address);
        radio.startListening();

        //Debugging
        Serial.println("Waiting for neighbor request");
        
        start = millis();
        duration = 0;
        
        neighborwaitrequest_first = false;
      }

      if(radio.available())
      {
        uint8_t received [MessageLengthByte] {0};
        uint32_t source_address {0};
        uint32_t target_address {0};
        uint8_t messageID {0};
        uint8_t system_position {0};
        
        radio.read(&received, MessageLengthByte);
        
        messageID = received[0];
        source_address = (received[1]<<16) | (received[2]<<8) | received[3];
        target_address = (received[4]<<16) | (received[5]<<8) | received[6];
        system_position = received[7];

        //Debugging
        Serial.println(messageID, HEX);
        Serial.println(source_address, HEX);
        Serial.println(target_address, HEX);
        
        if(messageID == RequestID && system_position == my_position)
        {
          message_target = source_address;
          STATE = AnswerNeighborRequest;
        }
        else if(messageID == AnswerID)
        {
          STATE = WaitForInsertion;
          neighborwaitrequest_first = true;
        }
        
      }

      break;
    }
    
    case AnswerNeighborRequest:
    {
      if(answerrequest_first)
      {
        //Debugging
        Serial.println("Answering neighbor request");
        
        neighborwaitrequest_first = true;
        
        AnswerMessage[4] = ((message_target & 0x00FF0000)>>16);
        AnswerMessage[5] = ((message_target & 0x0000FF00)>>8);
        AnswerMessage[6] = ((message_target & 0x000000FF));
        AnswerMessage[7] = my_position;
          
        start = millis();
        duration = 0;

        radio.stopListening();
        radio.openWritingPipe(address);
        answerrequest_first = false;
      }
    
      if(duration >= SendingDuration)
      {
        radio.write(&AnswerMessage, MessageLengthByte);
        STATE = WaitForNeighborAcknowledge;
        answerrequest_first = true;
      }

      break;
    }

    case WaitForNeighborAcknowledge:
    {
      if(neighborwaitacknowledge_first)
      {
        //Debugging
        Serial.println("Waiting for neighbor acknowledge");
        
        start = millis();
        duration = 0;

        radio.openReadingPipe(1, address);
        radio.startListening();

        neighborwaitacknowledge_first = false;
      }

      if(duration <= ListeningDuration)
      {
        if(radio.available())
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
            neighbor_up = source_address;
            IsMaster = true;
            collision_first = true;
            checkslot_first = true;
            STATE = CollisionDetected;
            break;
          }
        }
      }
      else
      {
        STATE = AnswerNeighborRequest;
        neighborwaitacknowledge_first = true;
      }
      
      break;
    }

    case WaitForInsertion:
    {
      if(waitinsertion_first)
      {
        radio.openReadingPipe(1, address);
        radio.startListening();

        //Debugging
        Serial.println("Waiting for insertion");
        
        start = millis();
        duration = 0;
        
        waitinsertion_first = false;
      }

      if(radio.available())
      {
        uint8_t received [MessageLengthByte] {0};
        uint32_t source_address {0};
        uint32_t target_address {0};
        uint8_t messageID {0};
        uint8_t system_position {0};
        
        radio.read(&received, MessageLengthByte);
        
        messageID = received[0];
        source_address = (received[1]<<16) | (received[2]<<8) | received[3];
        target_address = (received[4]<<16) | (received[5]<<8) | received[6];
        system_position = received[7];

        //Debugging
        Serial.println(messageID, HEX);
        Serial.println(source_address, HEX);
        Serial.println(target_address, HEX);
        
        if(messageID == AcknowledgeID)
        {
          message_target = source_address;
          collision_first = true;
          checkslot_first = true;
          STATE = CollisionDetected;
        }
      }
      break;
    } 

    default:
    {
      break;
    }

  }
}
