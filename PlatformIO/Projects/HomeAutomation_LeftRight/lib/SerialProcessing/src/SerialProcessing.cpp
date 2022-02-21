#include <Arduino.h>
#include <stdlib.h>
#include "SerialProcessing.h"

int STRING_POSITION = 0;
int RX_STRING_READY = 0;

char READ_Char()
{
  char temp = 0;
  if(Serial.available() > 0)
  {
    temp = Serial.read();  
  }

  return temp;
}

int READ_Digit()
{
  char temp = 0;
  int num = 0;
  
  if(Serial.available() > 0)
  {
    temp = Serial.read();
    num = temp - '0';  
  }
  
  return num;
}

void READ_String(char* str)
{
  if(Serial.available() > 0)
  {
    
    str[STRING_POSITION] = Serial.read();
     
    if(str[STRING_POSITION] == '\n')
    {
      RX_STRING_READY = 1;
      str[STRING_POSITION] = '\0';                //The string position to be exchanged with '\0' depends on the terminal. If EOL is CRLF then -1 else no substraction needed.
    } 

    else
    {
      if(STRING_POSITION < len)
      {
        STRING_POSITION++;
      }

      else
      {
        STRING_POSITION = 0;
      }
    }
  }
}

int READ_int(char* str)
{
  READ_String(str);
  return atoi(str);
}

double READ_double (char* str)
{
  READ_String(str);
  return strtod(str, NULL);
}

void RESET_STRING(char* str)
{
  
  while(STRING_POSITION >= 0)
  {
    str[STRING_POSITION] = '\0';
    STRING_POSITION--;
  }

  RX_STRING_READY = 0;
  STRING_POSITION = 0;
}

char SERIAL_FLUSH()
{
  char t;
  while(Serial.available() > 0)
  {
    t = Serial.read();
    Serial.println("flushing");
    
  }
  return t;
}