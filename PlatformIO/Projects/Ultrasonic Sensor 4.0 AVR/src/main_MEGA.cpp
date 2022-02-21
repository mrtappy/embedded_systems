
#define F_CPU 16000000L
#define __DELAY_BACKWARD_COMPATIBLE__                 //enable use of variables in delay functions
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <arduino.h>														//Arduino library for testing purposes.

//Global variables
unsigned char ack = 0;
int TimerOverflow = 0;

//Variables for distance measurement
long count = 0;
double distance = 0;

ISR(TIMER4_OVF_vect)																														
{
	TimerOverflow++;		                                                           // Increment Timer Overflow count
}

ISR(INT4_vect)                                                                  //External interrupt since AIN0 is nc on Arduino MEGA
{																														

  Serial.println(TCNT4);
    
}


int main()
{ 
  //Enable Interrupts
  sei();                                                      //Set global interrupt flag in SREG
  EICRB |= _BV(ISC40) | _BV(ISC41);                           //External interrupt 0 on rising edge                      

  //PWM for LEDs

  //Counter for travel time
  TIMSK4 = _BV(TOIE4);                                        //Enable Timer4 overflow interrupt
	TCCR4A = 0;																									//Normal operation
  
  //Arduino library for testing purposes
  Serial.begin(9600);

  TCCR4B |= _BV(CS40);                                   //Start counter 1
  EIMSK |= _BV(INT4);                                    //Enable external interrupt 0


  //Main loop
  while(1)
  {  
    //JUST WAIT FFS!
  }

}