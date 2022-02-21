#include <Arduino.h>
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define MessageLengthByte 8

//RF24 Object initialization
RF24 radio(7, 10); // CE, CSN

//Variables

const byte address[6] {"00001"};

uint32_t i {0};

void setup() 
{
  Serial.begin(115200);
  Serial.println("Listening: ");

  radio.begin();
  radio.openReadingPipe(1, address);
  radio.startListening();
  radio.setPALevel(RF24_PA_HIGH);
  radio.setAutoAck(false);
}

void loop() 
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
    Serial.print("Message No: ");
    Serial.println(i);
    Serial.println(messageID, HEX);
    Serial.println(source_address, HEX);
    Serial.println(target_address, HEX);
    Serial.println();
    i++;

    radio.flush_rx();
  }  
}