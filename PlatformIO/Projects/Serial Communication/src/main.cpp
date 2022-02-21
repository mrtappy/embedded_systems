#define F_CPU 16000000L

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

//-------------------------------LIB STUFF-------------------
#define BUFFER_SIZE 64

//Function Prototypes

//Transmit
void USART_init(uint32_t);
void USART_Transmit(uint8_t);
void USART_Transmit_String(char[]);
void USART_Transmit_Num(double);

//Receive
char USART_Receive();
int USART_Receive_String(char* str, int, uint8_t);          //Requires EOL to be LF only! Look at the settings of your serial monitor/terminal! --eol LF for platformios monitor!
void USART_Receive_String_Echo(char*);                    
int RESET_STRING(char*, int );
int Read_Command(char*, int, uint8_t, int);
void FLUSH_RX_BUFF();

//Global variables

//Transmit Buffer
volatile uint8_t USART_TX_BUFF[BUFFER_SIZE];
volatile uint8_t tx_newest_data;
volatile uint8_t tx_oldest_data;
volatile uint8_t tx_next_data;
volatile uint8_t TX_BUF_OV;
volatile uint8_t TX_BUF_IR;

//Receive Buffer
volatile uint8_t USART_RX_BUFF[BUFFER_SIZE];
volatile uint8_t rx_newest_data;
volatile uint8_t rx_oldest_data;
volatile uint8_t rx_next_data;
volatile uint8_t RX_BUF_OV;
volatile uint8_t RX_BUF_IR;
volatile uint8_t RX_STRING_READY;


//ISRs

//Transmit
ISR(USART_UDRE_vect)
{
  UDR0 = USART_TX_BUFF[tx_oldest_data];

  if((tx_oldest_data+1) > (BUFFER_SIZE-1))
  {
    tx_oldest_data = 0;  
  }

  else
  {
    tx_oldest_data++;
  }

  TX_BUF_OV = 0;
  
  if(tx_oldest_data == tx_newest_data)
  {
    UCSR0B &= ~(_BV(UDRIE0));
    TX_BUF_IR = 0;  
  }
}

//Receive
ISR(USART_RX_vect)
{
  USART_RX_BUFF[rx_newest_data]= UDR0;  //Read receive register and save value to buffer
  
  if(rx_newest_data == (BUFFER_SIZE-1))
  {
    rx_newest_data = 0;
  }

  else
  {
    rx_newest_data++;
  }

  RX_BUF_IR = 1;                    //Signal data received
}


//-------------------------------------------------------------


//------------------------------testing stuff-------------------------------


//---------------------------------------------------------------------------



//---------------------------------main for testing------------------------
int main()
{
  USART_init(9600);
  sei();
  char data = '0';
  volatile uint8_t len  = 64;
  char str[len] = "\0";
  int i = 0;
  volatile int j = 1;
  volatile char dummy = '\0';
 

  while(1)
  {

    i = USART_Receive_String(str, i , len);
    
    if(RX_STRING_READY == 1)
    {
      str[i-1] = '\0';

      if(strcmp(str, "kill") == 0)
      {
        //USART_Transmit_String(str);
        USART_Transmit_String("Process stopped\n");
        //USART_Transmit_String(USART_RX_BUFF);

        j = 0;
      }

      else
      {
        //USART_Transmit_String(str);
        USART_Transmit_String("No valid command, resume process\n");
        //USART_Transmit_String(USART_RX_BUFF);
        
        j = 1;
      }

      RX_STRING_READY = 0;

      i = RESET_STRING(str, i);
      //FLUSH_RX_BUFF();

    }

    if(j == 1)
    {
      USART_Transmit_String("H");
    }
    

  }

 
  
}
//-------------------------------------------------------------------------

//Functions

void USART_init(uint32_t baud)                        //Not working on 115200 Baud
{
  //uint16_t UBRR = F_CPU/16/baud-1;
  
  
  double temp = (double) F_CPU/16/baud-1;
  uint16_t UBRR = (uint16_t) temp;
  double delta = (double) temp - UBRR;

  if(delta >= 0.5)
  {
    UBRR++;
  }
  
  UBRR0H = (UBRR>>8);
  UBRR0L = UBRR;   
  

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); //Asynchronous UART, no parity, 1 stop bit, 8 bit data 
  UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0); //enable transmitter and data register empty interrupt

  //Reset Buffer Indexes:
  tx_newest_data = 0;
  tx_oldest_data = 0;
  TX_BUF_IR = 0;
  TX_BUF_OV = 0;

  //Reset Buffer Indexes:
  rx_newest_data = 0;
  rx_oldest_data = 0;
  RX_BUF_IR = 0;
  RX_BUF_OV = 0;
  RX_STRING_READY = 0;

}

void USART_Transmit(uint8_t data)
{
  
  if((tx_newest_data+1) > (BUFFER_SIZE-1))
  {
    
    if(tx_oldest_data == 0)
    {
      TX_BUF_OV = 1;
      while(TX_BUF_OV == 1)
      {
        //wait
      }
    }

    tx_next_data = 0;  
  }

  else if((tx_newest_data+1) == tx_oldest_data)
  {
    TX_BUF_OV = 1;
    while(TX_BUF_OV == 1)
    {
      //wait
    }
    tx_next_data = tx_newest_data+1;
  }

  else
  {
    tx_next_data = tx_newest_data + 1;
  }

  USART_TX_BUFF[tx_newest_data] = data;
  tx_newest_data = tx_next_data;

  if(TX_BUF_IR == 0)
  {
    TX_BUF_IR = 1;
    UCSR0B |= _BV(UDRIE0);
  }

}

void USART_Transmit_String(char str[])
{
  int i = 0;
  int len = strlen(str);
  
  while(i != len)
  {
    USART_Transmit(str[i++]);
  }
  
}

void USART_Transmit_Num(double num)
{
  
  int var = num*100;
  int i = 0;
  
  while((var != 0) && (i <= 10))
  {
    var = var/10;
    i++;
  }
  
  char str[i+1];
  dtostrf(num, i+1, 2, str);

  USART_Transmit_String(str);
}

char USART_Receive()
{
  char rx_data = USART_RX_BUFF[rx_oldest_data];
  
  if(rx_oldest_data == (BUFFER_SIZE-1))
  {
    rx_oldest_data = 0;
  }
  
  else
  {
    rx_oldest_data++;
  }

  RX_BUF_IR = 0;

  return rx_data;
}

int USART_Receive_String(char* str, int i, uint8_t len)
{

  if(RX_BUF_IR == 1)
  {
    
    str[i] = USART_Receive();

    if(str[i] == '\n')
    {
      RX_STRING_READY = 1; 
    }
      
    else
    {
      if(i == (len))
      {
        i = 0;
      }
      
      else
      {
        i++;
      }
      
      RX_STRING_READY = 0;  

    }
    
  }


  return i; 

}

void USART_Receive_String_Echo(char* str)
{
  if(RX_STRING_READY == 1)
  {
    USART_Transmit_String(str);
    RX_STRING_READY = 0;
  }
}

int RESET_STRING(char* str, int i)
{
  
  while(i >= 0)
  {
    str[i] = '\0';
    i--;
  }

  i = 0;
  return i;
}

void FLUSH_RX_BUFF()
{
  rx_newest_data = 0;
  rx_oldest_data = 0;
  RX_BUF_IR = 0;
  RX_BUF_OV = 0;
}
