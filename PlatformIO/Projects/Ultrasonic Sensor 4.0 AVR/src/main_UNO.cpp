
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
unsigned long loops = 0;
unsigned long maxloops = 15000;																									//max loops in case the sensor cant sense an echo, otherwise infinite loop lol
uint16_t adc = 0;

//Variables for distance measurement
long count = 0;
double distance = 0;


//function prototypes
void PWM(int, int, char);
void DistanceMeter(double, int);


ISR(TIMER1_OVF_vect)																														//ISR = Interupt Service Routine, is called upon occurance of an Interrupt event. Stops program flow in main function. Is a "hardwired-function" which is handled by the compiler
{
	TimerOverflow++;		                                                           // Increment Timer Overflow count
}

ISR(ANALOG_COMP_vect)
{																														
  while((ADCSRA & _BV(ADSC)) == 1)               //wait for adc to finish conversion
  {
    Serial.println("Reading value");
  }
  //Read ADC
  adc = ADC;   
  ack = 1;                                       //Set acknowledge to break loop
  
  TCCR1B &= ~(1<<CS10);                       
  ACSR  &= ~(1<<ACIE);                           //Disable analog comparator interrupt
  ACSR  |= (1<<ACD);                             //Turn off analog comparator to prevent interrupt without measuremnet

  count = ICR1 + (65535*TimerOverflow);

  //clear ADC flag
  ADCSRA &= ~(1 << ADIF);
}


int main()
{ 
  //Enable Interrupts
  sei();

  //PORTS
  DDRB = _BV(PB0) | _BV(PB1) | _BV(PB3);                                             //Set PIN 11 as output
  DDRD = _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5);            //LED Ports

  PORTB &= 0xFC;
  PORTD &= 0xC3;


  //PWM                                                                        
  TCCR2A = _BV(COM2A0)| _BV(WGM20) | _BV(WGM21);						  //Toggle OCR0A on compare match and fast PWM 8 bit
  TCCR2B = _BV(CS21) | _BV(WGM22);                            //fast PWM 8 bit and clock divider = 8
  OCR2A = 50;                                                 //Set top value for compare match
  TCNT2 = 0;                                                  //Clear counter

  //Counter for travel time
  TIMSK1 = _BV(TOIE1);                                        //Enable Timer1 overflow interrupts
	TCCR1A = 0;																									//Normal operation
	TCCR1B = _BV(ICES1);                                        //Normal operation

  
  //Analog comparator 
  ACSR   |= _BV(ACIS1) | _BV(ACIS0) | _BV(ACD) | _BV(ACIC);                                 
  
  //ADC
  ADMUX = 0;                                                                         //ARef, channel 0, rigth adjusted
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);            //ADC enabled, Fcpu divided by 128             
  ADCSRB = _BV(ADTS0);                                                               //Conversion triggered by positive edge from analog comparator
  

  //Arduino library for testing purposes
  Serial.begin(9600);

  //Main loop
  while(1)
  {  
    
    ACSR  &= ~(1<<ACIE);                           //Disable analog comparator interrupt
    TCCR1B &= ~(1<<CS10);                          //Stop timer 
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
    _delay_us(2000);                                     //Added delay to prevent Transmitter interference. Can be reduced to 200 us if Analog comparator bias voltage is increased to 1.8V. This will limit the maximum distance to about 2 m.
    
    //Start comparator
    ACSR  &= ~(1<<ACD);                                  //Turn on analog comparator
    ACSR  |= _BV(ACIE);                                  //Enable analog comparator interrupt
    
    //Wait for comparator to sense reflected signal. 
    while((ack == 0 && loops <= maxloops))
    {
      loops++;
    }
    
    distance = count/932.95;                  //16MHz timer frequency, speed of sound = 343 m/s
    Serial.print("Value: ");
    Serial.print(adc);
    Serial.print(" @ ");
    Serial.println(distance);

    DistanceMeter(distance, adc);

    //_delay_ms(100);
  }

}

void PWM(int cycle, int PortPin, char Port)
{
  int x = cycle;
  int y = 100 - cycle;
  int i = 0;

  if(Port == 'D')
  {
    for(i = 1000; i != 0; i--)
    {
      PORTD |= _BV(PortPin);
      _delay_us(x);
      PORTD &= ~(1<<PortPin);
      _delay_us(y);
    }
  }
  else if(Port == 'B')
  {
    for(i = 1000; i != 0; i--)
    {
      PORTB |= _BV(PortPin);
      _delay_us(x);
      PORTB &= ~(1<<PortPin);
      _delay_us(y);
    }
  }

}

void DistanceMeter(double distance, int adc)
{
  int cycle = 0;

  if(adc <= 716)
  {
    cycle = 0.139*adc;
  }
  else
  {
    cycle = 100;
  }

  if(distance >= 150)
	{
		PWM(cycle, PB1, 'B');
	}
	
	else if(distance >= 120)
	{
		PWM(cycle, PB0, 'B');
	}
	
	else if(distance >= 90)
	{
		PWM(cycle, PD5, 'D');
	}
		
	else if(distance >= 60)
	{
		PWM(cycle, PD4, 'D');
	}
	
	else if(distance >= 30)
	{
		PWM(cycle, PD3, 'D');
	}
	
	else
	{
		PWM(cycle, PD2, 'D');
	}
}