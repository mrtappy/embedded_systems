#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <arduino.h>														//Arduino library for testing purposes.

//Global variables
uint16_t adc = 0;
uint32_t value = 0;
uint32_t TimerOverflow = 0;
uint32_t time = 0;

ISR(ANALOG_COMP_vect)
{
  while((ADCSRA & _BV(ADSC)) == 1);            //wait for adc to finish conversion
  
  //Read ADC
  adc = ADC; 
  time = (ICR1 + (65535*TimerOverflow))/16000000;
  Serial.print("Value: ");
  Serial.print(adc);
  Serial.print(" @ ");
  Serial.println(time);

  //clear ADC flag
  //ADCSRA &= ~(1 << ADIF);
}

ISR(TIMER1_OVF_vect)																														//ISR = Interupt Service Routine, is called upon occurance of an Interrupt event. Stops program flow in main function. Is a "hardwired-function" which is handled by the compiler
{
	TimerOverflow++;		                                                           // Increment Timer Overflow count
}

int main()
{
  //interrupts
  sei();

  //ADC Setup
  ADMUX = 0;
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0) | _BV(ADATE);
  ADCSRB = _BV(ADTS0);

  //Analog comparator
  ACSR   |= _BV(ACIS1) | _BV(ACIS0) | _BV(ACD) | _BV(ACIC);      
  ACSR  &= ~(1<<ACD);                                  //Turn on analog comparator
  ACSR  |= _BV(ACIE);                                  //Enable analog comparator interrupt
  
  //Timer1 for time stamps
  TIMSK1 = _BV(TOIE1);                                        //Enable Timer1 overflow interrupts
	TCCR1A = 0;																									//Normal operation
	TCCR1B = _BV(ICES1);                                        //Normal operation
  TCCR1B |= _BV(CS10);                                        //Start counter 1
  
  //Testing
  Serial.begin(9600);

  while(1)
  {
    //Interrupts
  }
}