
#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <arduino.h>														//Arduino library for testing purposes.

unsigned char ack = 0;

ISR(ANALOG_COMP_vect)
{																														
	ack = 1;                                       //Set acknowledge to break loop
  PORTB &= ~(1 << PB0);                          //Set Pin LOW to signal measurement end to 8051.                          
  ACSR  &= ~(1<<ACIE);                           //Disable analog comparator interrupt
  ACSR  |= (1<<ACD);                             //Turn off analog comparator to prevent interrupt without measuremnet
}

int main(void)
{ 
  //Enable Interrupts
  sei();

  //PORTS
  DDRB = _BV(PB1) | _BV(PB0) | _BV(PB2);                                        //Set PIN 8, 9 and 10 as output
  
  //PWM                                                                         
  TCCR1A = _BV(COM1A0)| _BV(WGM10) | _BV(WGM11);																//Toggle OC1A on compare match and fast PWM 8 bit
  TCCR1B = _BV(WGM12) | _BV(WGM13);                                             //fast PWM 8 bit and clock divider = 8
  OCR1AL = 198;                                                                 //Set top value for compare match, result: ca. 40.3 kHz (tested resonance frequency of the transducer)
  OCR1AH = 0;                                                                   //High-Byte for TOP value
  TCNT1 = 0;                                                                    //Clear counter

  //Analog comparator 
  ACSR   |= _BV(ACIS1) | _BV(ACIS0) | _BV(ACD);                                 

  //Main loop
  while(1)
  {  
    //Reset at measurement start
    PORTB &= ~(1 << PB0);                                    //If loop is ended by 8051, ensure LOW at the start of a new measurement  
    ack = 0;                                                 //Clear acknowledge variable
    TCNT1 = 0;                                               //Clear counter

    
    if((PINB & _BV(PB4)))                                    //Check if measurement start signal is send by 8051
    {
        TCCR1B |= _BV(CS10);                                 //Start PWM
        _delay_us(100);           
        TCCR1B &= ~(1 << CS10);                              //Stop PWM after 100 us (ca. 4 cycles)
        PORTB |= _BV(PB0);                                   //Set PB0 HIGH to signal 8051 that the measurement has been started                      
        _delay_us(1500);                                     //Added delay to prevent Transmitter interference. Can be reduced to 200 us if Analog comparator bias voltage is increased to 1.8V. This will limit the maximum distance to about 2 m.
        ACSR  &= ~(1<<ACD);                                  //Turn on analog comparator
        ACSR  |= _BV(ACIE);                                  //Enable analog comparator interrupt
    }
     
    //Wait for comparator to sense reflected signal. 8051 can break loop by setting PB2 HIGH if no return signal is sensed. 
    while((ack == 0))
    {
      if(PINB & _BV(PB4)) //Check if 8051 wants to break the loop due to exceedance of the measurement time limit
      {
        PORTB &= ~(1 << PB0);                                 //If loop is ended by 8051, ensure LOW at the start of a new measurement 
        ACSR  &= ~(1<<ACIE);                                  //Disable analog comparator interrupt
        ACSR  |= (1<<ACD);                                    //Turn off analog comparator to prevent interrupt without measuremnet
        break;                                                //Break the loop if 8051 wants to
      }
    }
    
  }

}
