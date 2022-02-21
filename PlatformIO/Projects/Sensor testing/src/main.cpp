//This program is inteded to drive a ultrasonic transducer via a 40 kHz PWM signal.
//The goal is to find the necessary gain for a LM324 in regard to a desired range.

#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


int main(void)
{
  sei();
  //DDRD |= B00111000;  // Sets D3, D4, D5 outputs
  DDRB = _BV(PB1) ;                                                             //Set PIN 9 as output
  TCCR1A = _BV(COM1A0)| _BV(WGM10) | _BV(WGM11);																//Toggle OC1A on compare match and fast PWM 8 bit
  TCCR1B = _BV(WGM12) | _BV(WGM13);                                             //fast PWM 8 bit and clock divider = 8
  TCNT1		= 0;                                                                  //Clear timer counter register

  OCR1AL = 199;                                                                 //Set top value for compare match
  OCR1AH = 0;

  while(1)
  { 
    TCNT1	= 0;                                                                //Clear timer counter register
    TCCR1B |= _BV(CS10);
    _delay_us(100);
    TCCR1B &= ~(1 << CS10);
    _delay_ms(100);
  }

}
