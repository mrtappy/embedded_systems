#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <arduino.h>																													//Arduino library for testing purposes. fck nub.
//#define b_low 20
//#define b_high 250

int TimerOverflow = 0;
unsigned long loops = 0;
unsigned long maxloops = 15000;																									//max loops in case the sensor cant sense an echo, otherwise infinite loop lol

ISR(TIMER1_OVF_vect)																														//ISR = Interupt Service Routine, is called upon occurance of an Interrupt event. Stops program flow in main function. Is a "hardwired-function" which is handled by the compiler
{
	TimerOverflow++;		                                                            // Increment Timer Overflow count
}


int main(void)
{
	sei();
	long count;
	double distance;

	DDRB = _BV(PB1) | _BV(PB3);                                 //Set PIN 9 & 11 as output

	sei();                                                      //Enable global interrupt
	TIMSK1 = _BV(TOIE1);                                        //Enable Timer1 overflow interrupts
	TCCR1A = 0;																																		//Normal operation
	TCCR1B = 0;                                                 //Normal operation

	 Serial.begin(9600);

	//Setup for rumble engine PWM control with timer/counter 2 (8 bit)
	TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);				//Clear OC2A on compare match, set OC2A at bottom, fast PWM Mode
	OCR2A = 0;																																		//no PWM after initialisation



  while(1)
  {

	//Setup for pulse duration measurement
	TCNT1 = 0;                                                  //Clear Timer/Counter register
    TCCR1B |= _BV(ICES1);	                                    //Capture event on rising edge, internal clock no prescaler
    TIFR1 |=  _BV(ICF1) | _BV(TOV1);                            //Clear ICP and TimerOverflow Flag
	//---------------------------------

	//Trigger the ultrasonic sensor
	PORTB |= _BV(PB1);
    _delay_us(10);
    PORTB &= ~(1<<PB1);
	
	//ICP1 is input pin for timer stop signal
	//wait for response from sensor, echo pin after ultrasonic signal is sent
    while (((TIFR1 & _BV(ICF1)) == 0) && (loops <= maxloops))
	{
		loops++;
	}
	
	loops = 0;
	TIFR1 = _BV(ICF1)|_BV(TOV1);                                  //Clear ICP and TimerOverflow Flag
	TimerOverflow = 0;                                            //Clear TimerOverFlow count
	TCCR1B = _BV(CS10);

	//wait for failing edge on echo pin. Sensor sets the echo pin on 0 after the echo is measured
	while (((TIFR1 & (1 << ICF1)) == 0) && (loops <= maxloops))	//Wait for falling edge, timer starts when input event occured at ICP1 (ICF1 will be set)
	{
		loops++;
	}

    TCCR1B &= ~(1 << CS10);																											//Stop the timer
	count = ICR1 + (65535 * TimerOverflow);	                    //Read timer count and add for occured overflows
	distance = count/932.95;                                    //16MHz timer frequency, speed of sound = 343 m/s

    Serial.print(distance);										//arduino.h code for precision tests
    Serial.println(" cm");										//arduino.h code for precision tests

		/*
		//PWM rumble engine control
		TCCR2B = _BV(CS20);																													//start timer/counter, no prescaler

		if(distance < b_low)																													//if distance is less then 70 cm FULL THROTTLE!
		{
			OCR2A = 255;
		}
		else if(distance > b_high)																										//if distance is more then 150 cm no power! no!
		{
			OCR2A = 0;
		}
		else																																				//linear function of throttle in between 150 cm and 70 cm
		{
			OCR2A = ((255+2*b_low)-10) - 2*distance;
		}
		*/
		_delay_ms(100);																														//slow down the process if you are lazy
		
  }

}
