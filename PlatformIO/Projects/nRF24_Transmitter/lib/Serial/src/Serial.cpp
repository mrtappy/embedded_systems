/*
    SPI port overview of ATMega328P
    PB3 = MOSI
    PB4 = MISO
    PB5 = SCK
    PB2 = CSN = SS --> Chip Select
*/

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "Serial.h"



void SPI_MasterInit(void)
{
  //Setup device as Master
  DDRB |= _BV(PB3) | _BV(PB5) | _BV(PB2);

  //Enable SPI, set device as master, SCK = f_osc/16
  SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPR0);
}

void SPI_SlaveInit(void)
{
  //Setup device as slave
  DDRB |= _BV(PB4);

  //Enable SPI
  SPCR |= _BV(SPE);
}

uint8_t SPI_MasterTX(uint8_t MData)
{
  //Writing a byte to SPDR starts the SPI clock generator, transmission starts
  SPDR = MData;
  //SPIF in SPSR marks completed transmission
  while(!(SPSR & (1<<SPIF)));

  return SPDR;
}

uint8_t SPI_MasterRX(void)
{
  //Return received byte from slave
  while(!(SPSR & (1<<SPIF)));
  return SPDR;
}

void SlaveTransmit(uint8_t SData)
{
  SPDR = SData;
  while(!(SPSR & (1<<SPIF)));
}

uint8_t SlaveReceive(void)
{
  while(!(SPSR & (1<<SPIF)));
  return SPDR;
}

void SlaveSelect(uint8_t SSPortReg, uint8_t SSPortBit)
{
  //Iniate Slave Select pins, need to be high to prevent communication
  //Refers to PB2 ON THE SLAVE, free choice on Master!
  SSPortReg |= _BV(SSPortBit);
}

void InitComm(uint8_t SSPortReg, uint8_t SSPortBit)
{
  //Select Slave to communicate with
  SSPortReg &= ~(1<< SSPortBit);
}
