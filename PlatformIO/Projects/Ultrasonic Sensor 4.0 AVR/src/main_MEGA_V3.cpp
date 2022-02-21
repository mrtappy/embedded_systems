
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
unsigned char fin = 0;
int TimerOverflow = 0;
int loops = 0;
int maxloops = 25000;
uint16_t adc0 = 0;
uint16_t adc1 = 0;

//Variables for distance measurement
long count = 0;
double distance = 0;

ISR(TIMER4_OVF_vect)																														
{
	TimerOverflow++;		                                //Increment Timer Overflow count
}

ISR(TIMER4_CAPT_vect)                                 
{																														
  TIMSK4 &= ~(1<<ICIE4);                              //Disable capture event interrupt, needs to be done first!
  TCCR4B &= ~(1<<CS40);                               //Stop timer

  TCCR0B |= _BV(CS00) | _BV(CS02);                    // Start timer 0, overflow occurs aftern approx. 2 ms (fits to transmitter-receiver-response)
 
  while((TIFR0 & _BV(TOV0)) == 0)
  {
    ADCSRA |= _BV(ADSC);   
    while((ADCSRA & _BV(ADSC)) == 1)
    {
      //wait for conversion to end
    }

    adc0 = ADC;

    if(adc0 < adc1)
    {
      adc0 = adc1;
    }

  }
  TCCR0B &= ~(_BV(CS00) | _BV(CS02));

  count = ICR4 + (65535*TimerOverflow);               //Account for occured overflows
  distance = count/932.95;                             //Calculate distance

  Serial.print("Value: ");
  Serial.print(adc0);
  Serial.print(" @ ");
  Serial.println(distance);
  

  adc1 = 0;                                           //Reset temporary storage value
  adc0 = 0;                                           //Reset temporary storage value
  ack = 1;
}


int main()
{ 
  //Enable Interrupts
  sei();                                                      //Set global interrupt flag in SREG                    

  //40 kHz PWM
  TCCR5A = _BV(COM5A0)| _BV(WGM50) | _BV(WGM51);						  //Toggle OCR0A on compare match and fast PWM 8 bit
  TCCR5B = _BV(WGM52) | _BV(WGM53);                           //fast PWM 8 bit and clock divider = 8
  OCR5A = 198;                                                //Set top value for compare match
  TCNT5 = 0;                                                  //Clear counter

  //Ports
  DDRL |= _BV(PL3);
  
  //Counter for travel time
  TIMSK4 = _BV(TOIE4);                                        //Enable Timer4 overflow interrupt
	TCCR4A = 0;																									//Normal operation
  TCCR4B = _BV(ICES4);

  //Timer 0 for pulse heigth measurement
  TCCR0A = 0;                                                 //Normal operation
  TCNT0 = 128;                                                //Set start value

  //ADC
  ADMUX = _BV(REFS1);                                                   //Internat ref 1.1V, channel 0, rigth adjusted
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);            //ADC enabled, Fcpu divided by 128             
  ADCSRB = 0;                                                           //Free running mode

  //Arduino library for testing purposes
  Serial.begin(9600);

  //Main loop
  while(1)
  {  
    TimerOverflow = 0;
    loops = 0;
    ack = 0;
    fin = 0;
    
    TIMSK4 &= ~(1<<ICIE4);                              //Disable capture event interrupt
    TCCR4B &= ~(1<<CS40);                               //Stop timer in case no reflected signal has been sensed
    TIFR4 |= _BV(TOV4) | _BV(ICF4);                     //Clear flags
    TCCR0B &= ~(_BV(CS00) | _BV(CS02));                 //Stop timer
    TIFR0 |= _BV(TOV0);                                 //Clear flag
    

    TCNT0 = 128;
    TCNT4 = 0;                                          //Clear timer 4 
    TCNT5 = 0;                                          //Clear timer 5

    TCCR5B |= _BV(CS50);                                //Start 40 kHz pulse
    TCCR4B |= _BV(CS40);                                //Start counter 4
    _delay_us(300);                                     //Set pulse duration
    TCCR5B &= ~(1<<CS50);                               //Stop pulse
    _delay_us(2500);                                    //Account for transmitter interference

    TIFR4 |= _BV(ICF4);                                 //Clear flag since capture event is already triggered by transmitter
    TIMSK4 |= _BV(ICIE4);                               //Enable capture event interrupt

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