#include <Arduino.h>
#include <DuePWM.h>

#define C0_PIN 32
#define C1_PIN 30
#define C2_PIN 28
#define C3_PIN 26
#define TRIGGER_PIN 47
#define NANO_ACK_PIN 46
#define DETECT_PIN 49
#define CODE_PIN 50

#define MAX_bit_duration 500
#define MAX_measure_duration 45000

DuePWM Sync;


long start {0};
long duration {0};
long measure_start {0};
long measure_time {0};
long bit_start {0};
long bit_duration {0};
long TimeToObstacle {0};

int STATE {0};
int bit_num {0};
uint8_t PSK_Code {0};
uint8_t PSK_Decode {0};
int Decode[4] {0};
int TEMP {0};

double distance {0};

void set_code(uint8_t code);

void setup() 
{
  
  pinMode(C0_PIN, OUTPUT);
  pinMode(C1_PIN, OUTPUT);
  pinMode(C2_PIN, OUTPUT);
  pinMode(C3_PIN, OUTPUT);
  
  pinMode(DETECT_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(NANO_ACK_PIN, INPUT);
  pinMode(CODE_PIN, INPUT);

  Sync.setup(40300, PWM0);
  Sync.duty(50, PWM0);
  Sync.dis0();

  start = micros();
  Serial.begin(115200);
  Serial.println("Hello World!");

  STATE = 0;
}

void loop() 
{
  
  duration = micros() - start;
  
  switch(STATE)
  {
    case 0:
    {
      start = micros();
      bit_num = 0;
      PSK_Decode = 0;
      bit_duration = 0;
      duration = 0;
      distance = 0;
      
      Decode[0] = 0;
      Decode[1] = 0;
      Decode[2] = 0;
      Decode[3] = 0;

      //Sync.ena0();
      set_code(0b00001001);
      digitalWrite(TRIGGER_PIN, HIGH);
      
      STATE = 1;
      break;
    }

    case 1:
    {    
      if(duration >= MAX_measure_duration)
      {
        STATE = 0;
      }
      else if(digitalRead(NANO_ACK_PIN) == 1)
      {
        digitalWrite(TRIGGER_PIN, LOW);
        STATE = 2;
      }
      break;
    }
    
    case 2:
    {
    
      if(duration >= MAX_measure_duration)
      {
        STATE = 0;
        break;
      }

      // if(duration <= 2500)
      // {
      //    //Wait 
      // }

      else if(digitalRead(DETECT_PIN) == 1)
      {
        STATE = 3;
        Sync.ena0();
        TimeToObstacle = micros() - start;
        bit_start = micros();
      }

      break;
    }

    case 3:
    {
      bit_duration = micros() - bit_start;

      if(duration >= MAX_measure_duration)
      {
        STATE = 0;
        break;
      }
      
      if(bit_num == 4)
      {
        Sync.dis0();

        TEMP = bit_num - 1;

        for(int i = 0; i < 4; i++)
        {
          PSK_Decode |= (Decode[i] << TEMP);
          TEMP--;
        }
        if(Decode[0] == 0)
        {
          PSK_Decode = ~PSK_Decode & 0x0f;
        }
        
        if(PSK_Decode == PSK_Code)
        {
          STATE = 4;
        }
        else
        {
          STATE = 5;
        }
        break;
      }

      if(bit_duration >= MAX_bit_duration)
      {
        bit_num++;
        bit_start = micros();
      }
      else if(bit_duration <= 100) 
      {
        Decode[bit_num] = digitalRead(CODE_PIN);  
      }

      break;
    }

    case 4:
    {
        distance = (double) 343*TimeToObstacle/20000;
        Serial.println(distance);
        Serial.println(TimeToObstacle);
        Serial.println(PSK_Code, BIN);
        Serial.println(PSK_Decode, BIN);
        Serial.print(Decode[0]);
        Serial.print(Decode[1]);
        Serial.print(Decode[2]);
        Serial.println(Decode[3]);
        Serial.println();

        STATE = 6;
        break;
    }

    case 5:
    {
      distance = (double) 343*TimeToObstacle/20000;
      Serial.println("Codes don't match");
      Serial.println(distance);
      Serial.println(PSK_Code, BIN);
      Serial.println(PSK_Decode, BIN);
      Serial.print(Decode[0]);
      Serial.print(Decode[1]);
      Serial.print(Decode[2]);
      Serial.println(Decode[3]);
      Serial.println();

      STATE = 6;
      break;
    }

    case 6:
    {
      if(duration >= MAX_measure_duration)
      {
        STATE = 0;
      }
      break;
    }

  }

}

void set_code(uint8_t code)
{
  
  PSK_Code = code;
  
  if((0x01 & code) == 0x01)
  {
    digitalWrite(C0_PIN, HIGH);
  }
  else
  {
    digitalWrite(C0_PIN, LOW);   
  }

  if((0x02 & code) == 0x02)
  {
    digitalWrite(C1_PIN, HIGH);
  }
  else
  {
    digitalWrite(C1_PIN, LOW);   
  }

  if((0x04 & code) == 0x04)
  {
    digitalWrite(C2_PIN, HIGH);
  }
  else
  {
    digitalWrite(C2_PIN, LOW);   
  }

  if((0x08 & code) == 0x08)
  {
    digitalWrite(C3_PIN, HIGH);
  }
  else
  {
    digitalWrite(C3_PIN, LOW);   
  }
}