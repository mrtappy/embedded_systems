#include <Arduino.h>
#include <SerialProcessing.h>

#define SwingDuration 1250
#define MaxDistanceTime 45000


extern int RX_STRING_READY;

//Function Prototypes
void Encode(char digit);
char Decode();
void StartTransmission();
void EndTransmission();


//Global variables
int STATE_VARIABLE = 0;
int RUN = 0;
char str[len];
int duration = 0;
int PulseDelay = 0;
int MaxWordTime = 0;
int Word[4] = {0,0,0,0};



void setup() 
{
  pinMode(49, INPUT);
  pinMode(47, OUTPUT);
  digitalWrite(47, LOW);
  Serial.begin(115200);
}

void loop() 
{
  switch(STATE_VARIABLE)
  {
    case 0:
    {
      RUN = 0;
      
      SERIAL_FLUSH();

      Serial.println("Enter delay between pulses");
      
      while(RX_STRING_READY == 0)
      {
        PulseDelay = READ_int(str);
      }
      RESET_STRING(str);
      Serial.print("You entered: ");
      Serial.println(PulseDelay);

      MaxWordTime = 2*PulseDelay;

      Serial.print("MaxWordTime is: ");
      Serial.println(MaxWordTime);

      STATE_VARIABLE = 1;
      break;
    }

    case 1:
    {
      int i = 0;
      bool start_bit = false;
      //Serial.println("Starting reception, press any key to stop");
      long TimeDiff = 0;
      long start = micros();

      if(digitalRead(49))
      {
        digitalWrite(47, HIGH);
        Serial.println("Checking for start");
        delayMicroseconds(PulseDelay); //Wait for start bit to end
        start_bit = true;
        while(TimeDiff < 4250)
        {
          if(digitalRead(49))
          {
            start_bit= false;
            break;
          }
          TimeDiff = micros() - start;
        }
        digitalWrite(47, LOW);
      }
      

      if(start_bit == true)
      {
        Serial.println();
        while(i < 4)
        {  
          Word[i] = Decode();
          Serial.print("Decoded: ");
          Serial.print(Word[i]);
          Serial.print(" bit #");
          Serial.println(i);
          i++;
        }

        if(Serial.available() > 0)
        {
          Serial.println("Ending reception");
          STATE_VARIABLE = 2;
          SERIAL_FLUSH();
          break;
        }
      }
      
    
      //Check for Stopp bit
      bool stop_bit = false;
      TimeDiff = 0;
      start = micros();
      
      if(start_bit)
      {
        while(TimeDiff < PulseDelay)
        {
          if(digitalRead(49))
          {
            stop_bit = true;
          }
          TimeDiff = micros() - start;
        }
        TimeDiff = 0;

        while(TimeDiff < PulseDelay)
        {
          if(digitalRead(49))
          {
            stop_bit = false;
            break;
          }
          TimeDiff = micros() - start;
        }
      }

      if(Serial.available() > 0)
      {
          Serial.println("Ending reception");
          STATE_VARIABLE = 2;
          SERIAL_FLUSH();
          break;
      }
    
      if(stop_bit == false && start_bit)
      {
        Serial.println("No stop bit received");
        Serial.println("Word discarded, waiting for new transmission");
        Serial.println();        
      }

      else if(stop_bit && start_bit)
      {
        for(int i = 0; i < 4; i++)
        {
          Serial.print(Word[i]);
        }

        Serial.println();
      }
      
      break;
    }

    case 2:
    {
      int temp = 0;
      Serial.println("What now?");
      Serial.println("1: Restart");
      Serial.println("2: End");

      while(temp == 0)
      {
        temp = READ_Digit();
      }

      if(temp == 1)
      {
        STATE_VARIABLE = 0;
        SERIAL_FLUSH();
      }
      else if(temp == 2)
      {
        STATE_VARIABLE = 11000;
      }
      else
      {
        Serial.println("Invalid input, try again");
        STATE_VARIABLE = 2;
      }
      break;
    }
    default:
    {
      //Do Nothing
      break;
    }
  }

}


char Decode()
{
  digitalWrite(47, HIGH);
  long start_word = 0;
  long DecodeDuration = 0;
  int digit = 0;
  bool pulse1 = false;
  bool pulse2 = false;
 
  start_word = micros();
  while(DecodeDuration < PulseDelay)
  {
    if(digitalRead(49))
    {
      pulse1 = true;
    }
    DecodeDuration = micros() - start_word;
  }

  DecodeDuration = 0;
  start_word = micros();
  while(DecodeDuration < PulseDelay)
  {
    if(digitalRead(49))
    {
      pulse2 = true;
    }
    DecodeDuration = micros() - start_word;
  }

  if(pulse1 && pulse2)
  {
    digit = 1;
  }
  else if(pulse1 == false && pulse2 == false)
  {
    digit = 0;
  }
  else
  {
    digit = 9;
  }

  digitalWrite(47, LOW);
  return digit;
}


