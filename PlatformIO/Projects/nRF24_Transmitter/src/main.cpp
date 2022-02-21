#define F_CPU 16000000L

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <Arduino.h>
#include <nRF24L01.h>
#include <Serial.h>

#define CSN PB2
#define CE PD4

//Function Prototypes
void WriteNRF(uint8_t , uint8_t , uint8_t);




int main()
{
  
  //USART
  Serial.begin(9600);

  //SPI Setup
  SPI_MasterInit();

  //Ports
  DDRD = _BV(PD4);
  PORTB |= _BV(CSN);
  PORTD &= ~_BV(CE);

  
  //nRF24 Init
  uint8_t ADW = 3;
  WriteNRF(W_REGISTER, SETUP_AW, ADW); //Set address widths to 5 bytes

  uint8_t FREQ = 64;
  WriteNRF(W_REGISTER, RF_CH, FREQ); //Set frequency to 2,464 GHz
  
 


  //Check Init
  uint8_t dummy = 0;
  WriteNRF(R_REGISTER, SETUP_AW, dummy);
  WriteNRF(R_REGISTER, RF_CH, dummy);


  while(1)
  {
    //main loop
    Serial.println("Hello");
    _delay_ms(500);
  }
}

void WriteNRF(uint8_t RW, uint8_t reg, uint8_t regVal)
{

  _delay_us(10);
  PORTB &= ~(_BV(CSN));     

  reg = RW + reg;
  uint8_t slave_data = 125;
  _delay_us(10);

  slave_data = SPI_MasterTX(reg);

  Serial.println(slave_data);

  _delay_us(10);

  slave_data = SPI_MasterTX(regVal);

  Serial.println(slave_data);

  _delay_us(10);

  PORTB |= _BV(CSN);
  
}