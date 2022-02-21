#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define ListeningDuration 1000
#define SendingDuration 100
#define AcknowledgeDuration 100


RF24 radio(7, 10); // CE, CSN

const byte address[6] {"00001"};

const char text[] {"Hello I am module 1"};
const char collision[] {"collision"};
const char send_ack[] {"ack"};

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
          //Serial.print("Other system detected");
          Serial.println(i);
          i++;
          char received[32] = "";
          radio.read(&received, sizeof(received));
          Serial.println(received);
          int eval = strcmp("collision", received);
          if(eval == 0)
          {
            STATE = 6;
          }
          else
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
      //Serial.println("Sending distress signal");
      delay(10);
      radio.write(&collision, sizeof(collision));
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
      delay(10);
      if(radio.available())
      {
        //Serial.println("Checking for acknowledgement");
        char ack[32] {"/0"};
        radio.read(&ack, sizeof(ack));
        Serial.println(ack);
        int eval = strcmp("ack", ack);
        if(eval == 0)
        {
          //Serial.println("collision acknowledged");
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
      //Serial.println("Sending");
      radio.stopListening();
      delay(10);
      radio.write(&text, sizeof(text));
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
      //Serial.println("Sending ACK");
      radio.stopListening();
      delay(10);
      radio.write(&send_ack, sizeof(send_ack));
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