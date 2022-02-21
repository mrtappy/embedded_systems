
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
int loops = 0;
int maxloops = 25000;

//Variables for distance measurement
long count = 0;
double distance = 0;

ISR(TIMER4_OVF_vect)																														
{
	TimerOverflow++;		                                                           // Increment Timer Overflow count
}

ISR(INT4_vect)                                                                  //External interrupt since AIN0 is nc on Arduino MEGA
{																														
  TCCR4B &= ~(1<<CS40);                               //Stop timer
  EIMSK &= ~(1<<INT4);                                //Disable external interrupt
  EICRB &= ~(_BV(ISC40) | _BV(ISC41));                //External interrupt 0 on rising edge disabled

  count = TCNT4 + (65535*TimerOverflow);               //Account for occured overflows
  distance = count/932.95;                             //Calculate distance

  Serial.print(" @ ");
  Serial.println(distance);
  
  ack = 1;
}


int main()
{ 
  //Enable Interrupts
  sei();                                                      //Set global interrupt flag in SREG
  EICRB |= _BV(ISC40) | _BV(ISC41);                           //External interrupt 0 on rising edge                      

  //40 kHz PWM
  TCCR5A = _BV(COM5A0)| _BV(WGM50) | _BV(WGM51);						  //Toggle OCR0A on compare match and fast PWM 8 bit
  TCCR5B = _BV(WGM52) | _BV(WGM53);                           //fast PWM 8 bit and clock divider = 8
  OCR5A = 198;                                                //Set top value for compare match
  TCNT5 = 0;                                                  //Clear counter

  //Ports
  DDRL |= _BV(PL3);

  //DDRD |= _BV(PD0);
  //PORTD &= 0xFE;
  
  //Counter for travel time
  TIMSK4 = _BV(TOIE4);                                        //Enable Timer4 overflow interrupt
	TCCR4A = 0;																									//Normal operation
  
  //ADC
  ADMUX = 0;                                                                         //ARef, channel 0, rigth adjusted
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);            //ADC enabled, Fcpu divided by 128             
  ADCSRB = _BV(ADTS1);                                                               //Conversion triggered by positive edge from analog comparator

  //Arduino library for testing purposes
  Serial.begin(9600);

  //Main loop
  while(1)
  {  
    TimerOverflow = 0;
    loops = 0;
    ack = 0;

    EIMSK &= ~(1<< INT4);                               //Disable interrupt in case interrupt wasnt triggered
    EICRB &= ~(_BV(ISC40) | _BV(ISC41));                //External interrupt 0 on rising edge disabled
    
    TCCR4B &= ~(1<<CS40);                               //Stop timer in case no reflected signal has been sensed
    TIFR4 |= _BV(TOV4);

    TCNT4 = 0;                                          //Clear timer 4 
    TCNT5 = 0;                                          //Clear timer 5

    TCCR5B |= _BV(CS50);                                //Start 40 kHz pulse
    TCCR4B |= _BV(CS40);                                //Start counter 4
    _delay_us(150);                                     //Set pulse duration
    TCCR5B &= ~(1<<CS50);                               //Stop pulse
    _delay_us(2500);                                    //Account for transmitter interference
  
    EICRB |= _BV(ISC40) | _BV(ISC41);                   //External interrupt 0 on rising edge                      
    EIMSK |= _BV(INT4);                                 //Enable external interrupt 0

    while((ack == 0 ) && (loops <= maxloops))
    {
      loops++;
      //just wait
    }

    if(loops == maxloops)
    {
      Serial.println("Range exceeded");
    }
  
    _delay_ms(100);
  }

}