#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <SPI.h>

//SPI transmitter code
int main(void)
{
    SPI_SlaveInit();

    //DDRB  |= _BV(PB1);
    //DDRD  |= _BV(PD3) | _BV(PD5);

    //ADC Setup for input from joystick
    uint16_t tot = 0;
    uint8_t low = 0;
    uint8_t high = 0;
    SPDR = 0;

    ADMUX   |= _BV(REFS0)  | _BV(ADLAR);
    ADCSRA  |= _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    //-----------------------------------------------------------------------



    while(1)
    {
      low = ADCH;
      //Servo control
      //low = ADCL;
      //high = ADCH;
      //tot = ((high<<8) | low);
      //SlaveTransmit(high);
      SlaveTransmit(low);

    }
}
