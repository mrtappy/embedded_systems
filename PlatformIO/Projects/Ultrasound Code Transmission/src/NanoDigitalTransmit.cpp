
#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Arduino.h>														//Arduino library for testing purposes.


#define SwingDuration 1250
#define MaxDistanceTime 45000


extern int RX_STRING_READY;

//Function Prototypes
void Encode(char digit);
char Decode();
void StartTransmission();
void EndTransmission();


//Global variables
int STATE_VARIABLE = 0;
int RUN = 0;
int duration = 0;
int PulseDelay = 0;
int MaxWordTime = 0;




void setup() 
{
  DDRB |= _BV(PB1);
  DDRB &= ~_BV(PB4);

  //PWM                                                                         
  TCCR1A = _BV(COM1A0)| _BV(WGM10) | _BV(WGM11);																//Toggle OC1A on compare match and fast PWM 8 bit
  TCCR1B = _BV(WGM12) | _BV(WGM13);                                             //fast PWM 8 bit and clock divider = 8
  OCR1AL = 198;                                                                 //Set top value for compare match, result: ca. 40.3 kHz (tested resonance frequency of the transducer)
  OCR1AH = 0;                                                                   //High-Byte for TOP value
  TCNT1 = 0;                                                                     //Clear counter               
  
  PulseDelay = 1500;
  duration = 25;


}

void loop() 
{
  //To be Transmitted: 1010

  StartTransmission();
  Encode(0);
  Encode(0);
  Encode(1);
  Encode(1);
  EndTransmission();

  delay(100);
}

void Encode(char digit)
{
  if(digit == 1)
  {
    TCCR1B |= _BV(CS10);
    delayMicroseconds(duration);         //wait to send a pulse chain
    TCCR1B &= ~_BV(CS10);

    delayMicroseconds(PulseDelay);

    TCCR1B |= _BV(CS10);
    delayMicroseconds(duration);         //wait to send a pulse chain
    TCCR1B &= ~_BV(CS10);

    delayMicroseconds(PulseDelay);
  }
  
  else if(digit == 0)
  {
    int zero_delay = 2*PulseDelay;
    delayMicroseconds(zero_delay);
  }
  
  else
  {
    Serial.println("invalid digit");
  }

}


void StartTransmission()
{
  //Send start bit
  TCCR1B |= _BV(CS10);
  delayMicroseconds(duration);         //wait to send a pulse chain
  TCCR1B &= ~_BV(CS10);
  
  delayMicroseconds(2*PulseDelay + SwingDuration);
}

void EndTransmission()
{
  //Send stop bit
  TCCR1B |= _BV(CS10);
  delayMicroseconds(duration);         //wait to send a pulse chain
  TCCR1B &= ~_BV(CS10);

  delayMicroseconds(2*PulseDelay + SwingDuration);
}