#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define ListeningDuration 1000
#define SendingDuration 100
#define AcknowledgeDuration 100
#define bit32toByte 4

#define BeaconID 0x24ul
#define CollisionID 0xDBul
#define AcknowledgeID 0x81ul

#define DeviceID 1ul


RF24 radio(7, 10); // CE, CSN

const byte address[6] {"00001"};

// const unsigned long BeaconMessage =  (BeaconID << 24) | DeviceID;
// const unsigned long CollisionMessage {(CollisionID << 24) | DeviceID};
// const unsigned long AcknowledgeMessage {(AcknowledgeID << 24) | DeviceID};

const uint32_t BeaconMessage =  (BeaconID << 24) | DeviceID;
const uint32_t CollisionMessage {(CollisionID << 24) | DeviceID};
const uint32_t AcknowledgeMessage {(AcknowledgeID << 24) | DeviceID};

int STATE {0};
int i {0};

long start {0};
long duration {0};

bool first {true};
bool first2 {true};
bool first3 {true};
bool first6 {true};
bool collision_first {true};

void setup() 
{
  Serial.begin(115200);
  Serial.println(BeaconMessage, HEX);

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
    case 0:
    {
      Serial.println("Starting");
    

      first = true;
      first2 = true;
      first3 = true;
      first6 = true;
      collision_first = true;

      radio.startListening();
      STATE = 1;
      break;
    }

    case 1:
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
          radio.read(&received, bit32toByte);
          Serial.println(received, HEX);
          received = (received >> 24);

          if(received == CollisionID)
          {
            STATE = 6;
          }
          if(received == BeaconID)
          {
            STATE = 2;  
          }
        }
      }
      else
      {
        start = millis();
        STATE = 5;
      }
      break;
    }

    case 2:
    {
      radio.stopListening();
      Serial.println("Sending distress signal");
      radio.write(&CollisionMessage, bit32toByte);
      STATE = 3;
      break;
    }

    case 3:
    {
      if(first3)
      {
        start = millis();
        first3 = false;
        break;
      }
      
      radio.startListening();

      if(radio.available())
      {
        Serial.println("Checking for acknowledgement");
        uint32_t ack = {0};
        radio.read(&ack, bit32toByte);
        Serial.println(ack, HEX);
        ack = (ack >> 24);

        if(ack == AcknowledgeID)
        {
          STATE = 4;
          break;
        }
      }

      if(duration > AcknowledgeDuration)
      {
        STATE = 2;
      }

      break;
    }

    case 4:
    {
      if(collision_first)
      {
        Serial.println("Collision has been detected");  
        collision_first = false;
      }
      break;
    }

    case 5:
    {
      radio.stopListening();
      radio.write(&BeaconMessage, bit32toByte);
      STATE = 0;
      break;
    }

    case 6:
    {
      if(first6)
      {
        start = millis();
        first6 = false;
        break;
      }
      Serial.println("Sending ACK");
      radio.stopListening();
      radio.write(&AcknowledgeMessage, bit32toByte);
      if(duration > AcknowledgeDuration)
      {
        STATE = 4;
      }
      
      break;
    }

    default:
    {
      break;
    }
  }
  
}