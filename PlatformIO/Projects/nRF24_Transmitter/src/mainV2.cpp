#define F_CPU 16000000L

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <Arduino.h>
#include <nRF24L01.h>
#include <Serial.h> //my SPI library

#define CSN PB2
#define CE PD7

//Function Prototypes
void WriteNRF(uint8_t , uint8_t , uint8_t*, uint8_t);
void ResetIRQ();
void FlushNRF();
uint8_t getStatus();

//global variables
uint8_t status = 0;
uint8_t run = 0;

int main()
{
  //USART
  Serial.begin(9600);

  //SPI Setup
  SPI_MasterInit();

  //Ports
  DDRD = _BV(CE) | _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5); // PD2 - PD5 used for "debug LEDs"
  PORTB |= _BV(CSN);  //defined as output via SPI_MasterInit()
  PORTD &= ~_BV(CE); 
  
  PORTD |=  _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5);  //Check if LEDs are working

  _delay_ms(11);

  //nRF24 Init
  uint8_t FREQ[] = {0x50}; //Set frequency to 2,409 GHz
  WriteNRF(W_REGISTER, RF_CH, FREQ, 1); 

  uint8_t PLDW[] = {0x01}; //Set payload width, here 1 byte
  WriteNRF(W_REGISTER, RX_PW_P0, PLDW, 1);

  uint8_t ReDelay[] = {0x03}; //4000us, 10 retries
  WriteNRF(W_REGISTER, SETUP_RETR, ReDelay, 1);

  uint8_t RFSet[] = {0x07};  //LNA Gain max, 0dBm, 1 Mbps
  WriteNRF(W_REGISTER, RF_SETUP, RFSet, 1); 

  uint8_t AutoACK[] = {0x01};  //Enable AutoACK for data pipe 0
  WriteNRF(W_REGISTER, EN_AA, AutoACK, 1); 

  uint8_t ENRXADDR[] = {0x01}; //Enable data pipe 0
  WriteNRF(W_REGISTER, EN_RXADDR, ENRXADDR, 1);

  //uint8_t FEAT[] = {0x01};  //Enable W_TX_PAYLOAD_NOACK command
  //WriteNRF(W_REGISTER, FEATURE, FEAT, 1);

  uint8_t CONF[] = {0x0E};  //Powerup, CRC 2 bytes, Enable CRC, 
  WriteNRF(W_REGISTER, CONFIG, CONF, 1);

  _delay_ms(2); //Delay for state transition from power down to start up 

  PORTD &=  ~(_BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5)); //Turn off LEDs

  while(1)
  {
    status = getStatus();
    Serial.println(status, BIN);
    //Write data to be transmitted to nRF via SPI
    //uint8_t data[] = {5, 7};
  
    uint8_t data[] = {'a', 'b'};

    //Do not send the same data twice, chinese clones seem to discard repeated payloads!
    if(run == 1)
    {
      WriteNRF(0, W_TX_PAYLOAD, data, 1);
      run = 0;
      PORTD = _BV(PD2);
    }

    else if(run == 0)
    {
      WriteNRF(0, W_TX_PAYLOAD, (data+1), 1);
      run = 1;
      PORTD = _BV(PD3);
    }
    
    PORTD |= _BV(CE);
    _delay_us(150); //Tx settling, added 20 us to be sure
    
    while(PINB & _BV(PB0))
    {
      PORTD |= _BV(PD4);

    }

    Serial.println("Ack received!");
    status = getStatus();
    Serial.println(status, BIN);

    PORTD &= ~(_BV(PD4));

    _delay_us(10);
    
    PORTD &= ~_BV(CE);

    ResetIRQ();
    
    _delay_ms(500);
  }

}

void WriteNRF(uint8_t RW, uint8_t reg, uint8_t* regVal, uint8_t ValLen)
{
  _delay_us(10);
  PORTB &= ~(_BV(CSN));         //Slave select
  reg = RW + reg;               //combine command
  uint8_t slave_data = 125;     //Data send from slave
  _delay_us(10);

  slave_data = SPI_MasterTX(reg); //Send command

  _delay_us(10);

  uint8_t i = 0;
  for(i = 0; i < ValLen; i++)
  {
    slave_data = SPI_MasterTX(regVal[i]); //Send value 
    _delay_us(10);
  }

  PORTB |= _BV(CSN);

  _delay_us(1);
}


void ResetIRQ()
{
  _delay_us(10);

  PORTB &= ~(_BV(CSN));     

  uint8_t slave_data = 125;

  _delay_us(10);

  slave_data = SPI_MasterTX(W_REGISTER + STATUS);

  _delay_us(10);

  slave_data = SPI_MasterTX(0x70);

  _delay_us(10);

  PORTB |= _BV(CSN);

  _delay_us(10);
}

void FlushNRF()   //Flush transmit FIFO
{
  volatile uint8_t slave_data = 125;
  _delay_us(10);
  slave_data = SPI_MasterTX(FLUSH_TX);
  _delay_us(10);
}

uint8_t getStatus() //Function to check status register, used for debugging
{
  _delay_us(10);
  
  PORTB &= ~(_BV(CSN));     

  uint8_t slave_data = 125;
  _delay_us(10);

  slave_data = SPI_MasterTX(0);
  
  _delay_us(10);

  PORTB |= _BV(CSN);

  _delay_us(10);

  return slave_data;
}