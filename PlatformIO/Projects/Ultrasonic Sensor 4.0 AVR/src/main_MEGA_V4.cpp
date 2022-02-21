
#define F_CPU 16000000L
#define __DELAY_BACKWARD_COMPATIBLE__           //enable use of variables in delay functions
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <Arduino.h>
#include <SCOMM.h>

#define maxloops 50000                        //Higher values lead to undefined behavior, proper value needs to be tested
#define maxloops2 500

//Global variables
static volatile int ack = 0;
volatile int TimerOverflow = 0;
volatile int loops = 0;
volatile int loops2 = 0;
 
volatile uint16_t adc0 = 0;
volatile uint16_t adc1 = 0;
volatile uint16_t end = 9999; 

//Variables for distance measurement
volatile long count = 0;
volatile double distance = 0;
volatile double time = 0;
volatile double height= 0;


ISR(TIMER4_OVF_vect)																														
{
	TimerOverflow++;		                                //Increment Timer Overflow count
}

ISR(TIMER4_CAPT_vect)                                 
{																														
  
  TIMSK4 &= ~(1<<ICIE4);                              //Disable capture event interrupt, needs to be done first!
  TCCR4B &= ~(1<<CS40);                               //Stop timer
  //PORTC |= _BV(PC0);                                //Start pulse, for testing of peak detection
  TCCR0B |= _BV(CS02);                                // Start timer 0
  
  while((end >= 0.5*adc1) && (loops2 <= maxloops2))   //The factor 0.6 was experimentally determined
  {
    adc1 = adc0;                                      //Store last conversion result for comparison
    ADCSRA |= _BV(ADSC);                              //Start conversion
    
    while((ADCSRA & _BV(ADSC)) == 1)
    {
      //wait for conversion to end
    }

    adc0 = ADC;
    end = adc0;

    if(adc0 < adc1)
    {
      adc0 = adc1;
    }

    loops2++;

  }

  TCCR0B &= ~(_BV(CS02));                    //Stop timer                                         

  
  if(loops2 >= maxloops2)
  {
    //PORTC &= ~_BV(PC0);                               //Stop pulse, for testing of peak detection
    USART_Transmit_String("Duration exceeded\n");       //No Calculation if loops >= maxloops
  }
 

  else
  {
    //PORTC &= ~_BV(PC0);                               //Stop pulse, for testing of peak detection

    time = (double) TCNT0*0.016;                        //Read timer and calculate time in ms
    count = ICR4 + (65535*TimerOverflow);               //Account for occured overflows
    distance = (double) count/932.95;                   //Calculate distance
    height = (double) (adc0*1.074);
  
    
    USART_Transmit_String("\nValue: ");
    USART_Transmit_Num(height);                     //Print max. amplitude in mV
    USART_Transmit_String(" mV @ ");
    USART_Transmit_Num(distance);
    USART_Transmit_String(" cm pulse length: ");
    USART_Transmit_Num(time);
    USART_Transmit_String(" ms ");
    

    /*
    Serial.print("Value: ");
    Serial.print(height);                     //Print max. amplitude in mV
    Serial.print(" mV @ ");
    Serial.print(distance);
    Serial.print(" m pulse length: ");
    Serial.print(time);
    Serial.println(" ms ");
    */

  }

  ack = 1;
}


int main()
{ 
  
  //Arduino library for testing purposes
  USART_init(9600);
  //Serial.begin(9600);
  
  //Enable Interrupts
  sei();                                                      //Set global interrupt flag in SREG                    

  //Timer 5 for 40 kHz PWM
  TCCR5A = _BV(COM5A0)| _BV(WGM50) | _BV(WGM51);						  //Toggle OCR0A on compare match and fast PWM 8 bit
  TCCR5B = _BV(WGM52) | _BV(WGM53);                           //fast PWM 8 bit and clock divider = 8
  OCR5A = 198;                                                //Set top value for compare match
  TCNT5 = 0;                                                  //Clear counter

  //Ports
  DDRL |= _BV(PL3);                                           //Output for timer 4
  DDRC |= _BV(PC0);                                           //Output to test pulse length measurement
  PORTC &= ~_BV(PC0);

  //Timer 4 for travel time
  TIMSK4 = _BV(TOIE4);                                        //Enable Timer4 overflow interrupt
	TCCR4A = 0;																									//Normal operation
  TCCR4B = _BV(ICES4);

  //Timer 0 for pulse length measurement
  TCCR0A = 0;                                                 //Normal operation

  //ADC
  ADMUX = _BV(REFS1);                                                   //Internat ref 1.1V, channel 0, rigth adjusted
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);            //ADC enabled, Fcpu divided by 128             
  ADCSRB = 0;                                                           //Free running mode
  
  
  //Main loop
  while(1)
  {  
    //Reset variables
    TimerOverflow = 0;
    loops = 0;
    ack = 0;
    adc1 = 0;
    adc0 = 0;
    loops2 = 0;
    end = 9999;

    //Timer 4 reset
    TIFR4 |= _BV(TOV4) | _BV(ICF4);                     //Clear flags
    TCNT4 = 0;                                          //Clear timer 4 
     
    //Timer 0 reset
    TCCR0B &= ~(_BV(CS00) | _BV(CS02));                 //Stop timer
    TIFR0 |= _BV(TOV0);                                 //Clear flag
    TCNT0 = 0;
    
    //Timer 5 reset
    TCNT5 = 0;                                          //Clear timer 5

    //Measurement start
    TCCR5B |= _BV(CS50);                                //Start 40 kHz pulse
    TCCR4B |= _BV(CS40);                                //Start counter 4
    _delay_us(200);                                     //Set pulse duration
    TCCR5B &= ~(1<<CS50);                               //Stop pulse
    
    _delay_us(2500);                                    //Account for transmitter interference

    ICR4 = 0;                                           //Clear capture register since capture event was already triggered by the transmitter
    TIFR4 |= _BV(ICF4);                                 //Clear flag since capture event was already triggered by the transmitter
    TIMSK4 |= _BV(ICIE4);                               //Enable capture event interrupt

    //Wait for response, response is handled by ISR 
    while((ack == 0) && (loops < maxloops))
    {
      //just wait
      loops++;
    }

    //Check range
    if(loops >= maxloops)
    {
      USART_Transmit_String("Range exceeded\n");
    }

    //Make it readable 
    _delay_ms(100);
  }
  

}