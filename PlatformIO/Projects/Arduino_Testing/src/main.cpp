#include <Arduino.h>
#define len 20 

void RESET_STRING(char*);
void READ_String(char*);
void INTERPRET_STRING(char*);

char str[len];
int i = 0;
int j = 1;
int RX_STRING_READY = 0;


void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  
  READ_String(str);
  INTERPRET_STRING(str);
  
  if(j == 1)
  {
    Serial.println("Hello World!");
  }

}

void RESET_STRING(char* str)
{
  
  while(i >= 0)
  {
    str[i] = '\0';
    i--;
  }

  i = 0;
}

void READ_String(char* str)
{
  if(Serial.available() > 0)
  {
    
    str[i] = Serial.read();
     
    if(str[i] == '\n')
    {
      RX_STRING_READY = 1;
      str[i-1] = '\0';
    } 

    else
    {
      if(i < len)
      {
        i++;
      }

      else
      {
        i = 0;
      }
    }
  }
}

void INTERPRET_STRING(char* str)
{
  if(RX_STRING_READY == 1)
  {
    if(strcmp(str, "kill") == 0)
    {
      j = 0;
      Serial.println("Process stopped");
    }
      
    else
    {
      j = 1;
      Serial.println("no valid command, proceeding...");
    }

    RX_STRING_READY = 0;
    RESET_STRING(str);
  }
}

