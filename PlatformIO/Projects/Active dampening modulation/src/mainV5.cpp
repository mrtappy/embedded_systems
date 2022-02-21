#include <Arduino.h>

long high_start {0};
long high_time {0};
long low_start {0};
long low_time {0}; 
long measure_start {0};
long measure_time {0};
long duration {0};
long start {0};
long obstacle_time {0};

bool start_code {false};
bool first {true};
bool en_low {false};
bool HIGH_FIRST {true};

int STATE {0};
int dect_count {0};
int decode[10] {9};
int average[10] {0};
int decode_pos {0};
int code {0};
int ref_code {10};
int shift {0};

double distance {0};

void setup() 
{
  pinMode(49, INPUT);
  pinMode(47, OUTPUT);
  pinMode(46, INPUT);

  measure_start = micros();
  Serial.begin(115200);
  Serial.println("Hello World!");
}

void loop() 
{

  duration = micros() - start;
  switch(STATE)
  {
    case 0:
    {
      decode_pos = 0;
      start = micros();
      duration = 0;
      
      digitalWrite(47, HIGH);
 
      STATE = 1;
      en_low = false;
      HIGH_FIRST = true;
      code = 0;
      break;
    }
    case 1:
    {  
      if(duration >= 45000)
      {
        STATE = 0;
      }
      else if(digitalRead(46) == 1)
      {
        digitalWrite(47, LOW);
        STATE = 2;  
      }
      break;
    }
    
    case 2:
    {

      if(digitalRead(49) == 1)
      {
        if(HIGH_FIRST)
        {
          obstacle_time = micros() - start;
          HIGH_FIRST = false;
        }
        
        
        decode[decode_pos] = 1;
        decode_pos++;
        STATE = 3;
        high_start = micros();
        en_low = false;
      }

      if(en_low)
      {
        low_time = micros() - low_start;  
      }
      else
      {
        low_time = 0;
      }

      if(duration >= 45000)
      {
        STATE = 0;
      }
      else if(en_low && low_time >= 2000)
      {
        STATE = 4;
      }

      break;
    }

    case 3:
    {

      if(digitalRead(49) == 0)
      {
        high_time = micros() - high_start;
        if(high_time <= 80)
        {
          decode_pos--;
          decode[decode_pos] = 9;
          STATE = 2;
          break;
        }
        else
        { 
          low_start = micros();
          decode[decode_pos] = 0;
          decode_pos++;
          STATE = 2;
          en_low = true;
        }
      
      }
      
      if(duration >= 45000)
      {
        STATE = 0;
      }

      break;
    }
    case 4:
    { 
      shift = decode_pos - 1;
      Serial.print("Decoded message: ");
      for(int i = 0; i < decode_pos; i++)
      {
        Serial.print(decode[i]);
        code |= (decode[i]<< shift);
        decode[i] = 9;
        shift--;
      }
      Serial.println();
      Serial.println(code, BIN);
      Serial.println(low_time);


      if(code == ref_code)
      {
        distance = (double) 343*obstacle_time/20000;
        Serial.println(distance);
      }
      else
      {
        Serial.println("Code doesn't match");
      }
      Serial.println('\n');
      STATE = 5;
      break;
    }
    case 5:
    {
      if(duration >= 45000)
      {
        STATE = 0;
      }
      break;
    }
  }

}