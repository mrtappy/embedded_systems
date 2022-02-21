
#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <arduino.h>														//Arduino library for testing purposes.

unsigned char ack = 0;

int main(void)
{ 
  //Enable Interrupts
  sei();
  
  DDRB |= _BV(PB1);
  DDRB &= ~_BV(PB4);

  //PWM                                                                         
  TCCR1A = _BV(COM1A0)| _BV(WGM10) | _BV(WGM11);																//Toggle OC1A on compare match and fast PWM 8 bit
  TCCR1B = _BV(WGM12) | _BV(WGM13);                                             //fast PWM 8 bit and clock divider = 8
  OCR1AL = 198;                                                                 //Set top value for compare match, result: ca. 40.3 kHz (tested resonance frequency of the transducer)
  OCR1AH = 0;                                                                   //High-Byte for TOP value
  TCNT1 = 0;                                                                    //Clear counter                             
  //TCCR1B |= _BV(CS10); 
  
  

  //Main loop
  while(1)
  {  
    
    if((PINB & _BV(PB4)))
    {

      TCCR1B |= _BV(CS10);
    }
    
    else
    {
      TCCR1B &= ~(_BV(CS10));
      TCNT1 = 0;
    }
        
  }

}
