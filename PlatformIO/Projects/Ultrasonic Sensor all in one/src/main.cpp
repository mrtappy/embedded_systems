
#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <arduino.h>														//Arduino library for testing purposes.

unsigned char ack = 0;
int TimerOverflow = 0;
unsigned long loops = 0;
unsigned long maxloops = 15000;																									//max loops in case the sensor cant sense an echo, otherwise infinite loop lol

ISR(TIMER1_OVF_vect)																														//ISR = Interupt Service Routine, is called upon occurance of an Interrupt event. Stops program flow in main function. Is a "hardwired-function" which is handled by the compiler
{
	TimerOverflow++;		                                                           // Increment Timer Overflow count
}


ISR(ANALOG_COMP_vect)
{																														
  ack = 1;                                       //Set acknowledge to break loop
  TCCR1B &= ~(1<<CS10);                       
  ACSR  &= ~(1<<ACIE);                           //Disable analog comparator interrupt
  ACSR  |= (1<<ACD);                             //Turn off analog comparator to prevent interrupt without measuremnent
}

int main(void)
{ 
  //Variables for distance measurement
  long count;
	double distance;
  
  //Enable Interrupts
  sei();

  //PORTS
  DDRB = _BV(PB3);                                             //Set PIN 11 as output
  
  //PWM                                                                        
  TCCR2A = _BV(COM2A0)| _BV(WGM20) | _BV(WGM21);						  //Toggle OCR0A on compare match and fast PWM 8 bit
  TCCR2B = _BV(CS21) | _BV(WGM22);                            //fast PWM 8 bit and clock divider = 8
  OCR2A = 50;                                                 //Set top value for compare match
  TCNT2 = 0;                                                  //Clear counter

  //Counter for travel time
  TIMSK1 = _BV(TOIE1);                                        //Enable Timer1 overflow interrupts
	TCCR1A = 0;																									//Normal operation
	TCCR1B = 0;                                                 //Normal operation

  
  //Analog comparator 
  ACSR   |= _BV(ACIS1) | _BV(ACIS0) | _BV(ACD);                                 

  //Arduino library for testing purposes
  Serial.begin(9600);

  //Main loop
  while(1)
  {  
    
    ACSR  &= ~(1<<ACIE);                           //Disable analog comparator interrupt
    TCCR1B &= ~(1<<CS10); 
    loops = 0;
    TimerOverflow = 0;
    distance  = 0;

    //Setup for pulse duration measurement
    TCNT1 = 0;                                                //Clear Timer/Counter register
    TIFR1 |=  _BV(ICF1) | _BV(TOV1);                          //Clear ICP and TimerOverflow Flag
    //---------------------------------
   
    //Reset at measurement start  
    ack = 0;                                                 //Clear acknowledge variable
    TCNT2 = 0;                                               //Clear counter

    //Start measurement
    TCCR2B |= _BV(CS21);                                 //Start PWM
    TCCR1B |= _BV(CS10);                                 //Start counter 1
    _delay_us(150);                                      //Transmitter pulse length
    TCCR2B &= ~(1 << CS21);                              //Stop PWM after 100 us (ca. 4 cycles)                     
    _delay_us(1500);                                     //Added delay to prevent Transmitter interference. Can be reduced to 200 us if Analog comparator bias voltage is increased to 1.8V. This will limit the maximum distance to about 2 m.
    
    //Start comparator
    ACSR  &= ~(1<<ACD);                                  //Turn on analog comparator
    ACSR  |= _BV(ACIE);                                  //Enable analog comparator interrupt
    
    //Wait for comparator to sense reflected signal. 
    while((ack == 0 && loops <= maxloops))
    {
      loops++;
    }
    
    //Calculate distance
    count = TCNT1 + (65535*TimerOverflow);
    distance = count/932.95;                  //16MHz timer frequency, speed of sound = 343 m/s

    Serial.print(distance);										//arduino.h code for precision tests
    Serial.println(" cm");										//arduino.h code for precision tests
  
    _delay_ms(100);
  }

}
