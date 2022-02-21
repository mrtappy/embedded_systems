#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <arduino.h>

int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
  TimerOverflow++;                                                               // Increment Timer Overflow count
}


int main(void)
{
  long count;
  double distance;

  DDRB = _BV(PB1);                                                                //Set PIN 9 output

  sei();                                                                         //Enable global interrupt
  TIMSK1 = _BV(TOIE1);                                                           //Enable Timer1 overflow interrupts
  TCCR1A = 0;                                                                    //Normal operation

  Serial.begin(9600);

  while(1)
  {
    PORTB |= _BV(PB1);
    _delay_us(10);
    PORTB &= ~(1<<PB1);

    TCNT1 = 0;                                                             //Clear Timer/Counter register
    TCCR1B |= _BV(ICES1)|_BV(CS10);                                        //Capture event on rising edge, internal clock no prescaler
    TIFR1 |=  _BV(ICF1) | _BV(TOV1);                                       //Clear ICP and TimerOverflow Flag

    while ((TIFR1 & _BV(ICF1)) == 0);                                      //Wait for rising edge
    TCNT1 = 0;                                                             //Clear timer counter
    TCCR1B = _BV(CS10);                                                    //Capture on falling edge, no prescaler
    TIFR1 = _BV(ICF1)|_BV(TOV1);                                           //Clear ICP and TimerOverflow Flag
                                                                           //Clear TimerOverFlow count

    while ((TIFR1 & (1 << ICF1)) == 0);                                    //Wait for falling edge
    count = ICR1 + (65535 * TimerOverflow);                               //Read timer count and add for occured overflows
    distance = count/932.95;                                              //16MHz timer frequency, speed of sound = 343 m/s

    Serial.print(distance);
    Serial.println(" cm");

    _delay_ms(200);

    TimerOverflow = 0;

  }

}
