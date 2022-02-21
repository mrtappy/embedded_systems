#include <Arduino.h>


long high_start {0};
long high_time {0};
long low_start {0};
long low_time {0}; 
long measure_start {0};
long measure_time {0};
bool start_code {false};
bool first {true};
bool en_low {false};
int STATE {0};
int dect_count {0};
int decode[10] {0};
int decode_pos {0};

void setup() 
{
  pinMode(49, INPUT);
  measure_start = micros();
  Serial.begin(115200);
  Serial.println("Hello World!");
}

void loop() 
{
  switch(STATE)
  {
    case 0:
    {
      if(digitalRead(49) == 1)
      {
        
        decode[decode_pos] = 1;
        decode_pos++;
        STATE = 1;
        Serial.println("HIGH");
        measure_time = micros() - measure_start;

      }

      if(en_low)
      {
        low_time = micros() - low_start;
      }
      else
      {
        low_time = 0;
      }

      if(low_time >= 2000)
      {
        decode_pos = 0;
        Serial.print("Decoded message: ");
        Serial.print(decode[0]);
        Serial.print(decode[1]);
        Serial.println(decode[2]);
        Serial.println(low_time);
        Serial.println(measure_time);
        decode[0] = 9;
        decode[1] = 9;
        decode[2] = 9;
        low_time = 0;
        en_low = false;
        measure_start = micros();
      }
      break;

    }

    case 1:
    {
      if(digitalRead(49) == 0)
      {
        en_low = true;
        low_start = micros();
        decode[decode_pos] = 0;
        decode_pos++;
        STATE = 0;
        Serial.println("LOW");
      }
      break;
    }

  }
  

}