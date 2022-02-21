#include <Arduino.h>
#include <DuePWM.h>

#define C0_PIN 32
#define C1_PIN 30
#define C2_PIN 28
#define C3_PIN 26


DuePWM Sync;


long measure_start {0};
long measure_time {0};
long duration {0};
long start {0};


int STATE {0};
int Summation[10] {0};
int Sum_pos {0};
int Samples[10] {0};
int average {0};

double distance {0};

void set_code(uint8_t code);

void setup() 
{
  
  pinMode(C0_PIN, OUTPUT);
  pinMode(C1_PIN, OUTPUT);
  pinMode(C2_PIN, OUTPUT);
  pinMode(C3_PIN, OUTPUT);
  
  pinMode(49, INPUT);
  pinMode(47, OUTPUT);
  pinMode(46, INPUT);

  Sync.setup(40300, PWM0);
  Sync.duty(50, PWM0);
  Sync.dis0();

  measure_start = micros();
  Serial.begin(115200);
  Serial.println("Hello World!");
}

void loop() 
{
  
  duration = micros() - start;
  measure_time = micros() - measure_start;
  switch(STATE)
  {
    case 0:
    {
      start = micros();
      measure_start = start;
      duration = 0;
      Sum_pos = 0;
      //Sync.duty0(50);
      Sync.ena0();
      set_code(0b00001011);
      digitalWrite(47, HIGH);
      STATE = 1;
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
      // Summation[Sum_pos] += analogRead(A0);
      // Samples[Sum_pos]++;

      // if(measure_time >= 500)
      // {
      //   Sum_pos++;
      //   measure_start = micros();
      // }

      if(duration >= 45000)
      {
        //STATE = 3;
        STATE = 0;
      }
      else if(duration >= 10000)
      {
        //Sync.duty0(0);
        Sync.dis0();
      }

      break;
    }

    // case 3:
    // {
    //   Serial.print("Samples: ");
    //   for(int i = 0; i < Sum_pos; i++)
    //   {
    //     average = Summation[i]/Samples[i];
    //     Serial.print(average);
    //     Serial.print("  ");
    //     Summation[i] = 0;
    //     Samples[i] = 0;
    //   }
    //   Serial.println();
      
    //   STATE = 0;
    //   break;
    // }

  }

}

void set_code(uint8_t code)
{
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