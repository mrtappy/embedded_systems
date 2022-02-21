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



void setup() 
{
  pinMode(53, OUTPUT); //Trigger Pin for Arduino Nano (Transmitter PWM)
  pinMode(49, INPUT);
  digitalWrite(53, LOW);
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

      Serial.println("Enter pulse duration");
      
      while(RX_STRING_READY == 0)
      {
        duration = READ_int(str);
      }
      RESET_STRING(str);
      Serial.print("You entered: ");
      Serial.println(duration);

      SERIAL_FLUSH();

      Serial.println("Enter delay between pulses");
      
      while(RX_STRING_READY == 0)
      {
        PulseDelay = READ_int(str);
      }
      RESET_STRING(str);
      Serial.print("You entered: ");
      Serial.println(PulseDelay);

      MaxWordTime = PulseDelay + SwingDuration;

      Serial.print("MaxWordTime is: ");
      Serial.println(MaxWordTime);

      STATE_VARIABLE = 1;
      break;
    }

    case 1:
    {
      if(RUN == 0)
      {
        Serial.println("Starting pulse transmission, press any key to stop");
      }

      digitalWrite(53, HIGH);         //Start PWM @ Nano
      delayMicroseconds(duration);         //wait to send a pulse chain
      digitalWrite(53, LOW);          //Stop PWM @ Nano

      delayMicroseconds(PulseDelay);

      digitalWrite(53, HIGH);         //Start PWM @ Nano
      delayMicroseconds(duration);         //wait to send a pulse chain
      digitalWrite(53, LOW);          //Stop PWM @ Nano

      delayMicroseconds(PulseDelay);

      if(Serial.available() > 0)
      {
        Serial.println("Ending transmission");
        STATE_VARIABLE = 2;
        SERIAL_FLUSH();
      }

      else
      {
        STATE_VARIABLE = 1; //else continue transmitting pulses
      }
  
      delay(100);
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

void Encode(char digit)
{


  if(digit == 1)
  {
    digitalWrite(53, HIGH);         //Start PWM @ Nano
    delayMicroseconds(duration);         //wait to send a pulse chain
    digitalWrite(53, LOW);          //Stop PWM @ Nano

    delayMicroseconds(PulseDelay);

    digitalWrite(53, HIGH);         //Start PWM @ Nano
    delayMicroseconds(duration);         //wait to send a pulse chain
    digitalWrite(53, LOW);          //Stop PWM @ Nano

    delayMicroseconds(PulseDelay);
  }
  else if(digit == 0)
  {
    int zero_delay = 2*(SwingDuration + PulseDelay);
    delayMicroseconds(zero_delay);
  }
  else
  {
    Serial.println("invalid digit");
  }

}

char Decode()
{
  long start_word = 0;
  long DecodeDuration = 0;
  char digit = '0';
 
  start_word = micros();
  while(DecodeDuration < MaxWordTime)
  {
    if(digitalRead(49))
    {
      digit = '1';
    }
    DecodeDuration = micros() - start_word;
  }

  return digit;
}


void StartTransmission()
{
  //Send start bit
  digitalWrite(53, HIGH);         //Start PWM @ Nano
  delayMicroseconds(duration);         //wait to send a pulse chain
  digitalWrite(53, LOW);          //Stop PWM @ Nano

  delayMicroseconds(PulseDelay);
}

void EndTransmission()
{
  //Send stop bit
  digitalWrite(53, HIGH);         //Start PWM @ Nano
  delayMicroseconds(duration);         //wait to send a pulse chain
  digitalWrite(53, LOW);          //Stop PWM @ Nano

  delayMicroseconds(PulseDelay);
}