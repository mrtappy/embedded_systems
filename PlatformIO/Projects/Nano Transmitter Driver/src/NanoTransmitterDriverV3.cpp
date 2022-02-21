
#define F_CPU 16000000L
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <arduino.h>														//Arduino library for testing purposes.

#define CodeLength 4

int Code[CodeLength] {1,1,0,0};

void HH_Pulse();
void LH_Pulse();
void L_Pulse();
void H_Pulse();
void Dampen();
void Amplify();
void get_code();
void Signal_Generator();

int main(void)
{ 
  
  //Ports
  DDRB |= _BV(PB1) | _BV(PB5);
  DDRB &= ~_BV(PB4);


  //PWM                                                                         
  TCCR1A = _BV(COM1A0)| _BV(WGM10) | _BV(WGM11);																//Toggle OC1A on compare match and fast PWM 8 bit
  TCCR1B = _BV(WGM12) | _BV(WGM13);                                             //fast PWM 8 bit and clock divider = 8
  OCR1AL = 198;                                                                 //Set top value for compare match, result: ca. 40.3 kHz (tested resonance frequency of the transducer)
  OCR1AH = 0;                                                                   //High-Byte for TOP value
  TCNT1 = 0;                                                                    //Clear counter                             
  //TCCR1B |= _BV(CS10); 
  
  Serial.begin(9600);

  //Main loop
  while(1)
  {  
    // Code[0] = 0;
    // Code[1] = 0;
    // Code[2] = 0;
    // Code[3] = 0;
    
    
    if(PINB & _BV(PB4)) //Wait for DUE Trigger
    {
      get_code();
      Signal_Generator();

      // H_Pulse();
      // Amplify();
      // Dampen();
      // H_Pulse();
      // Dampen();
      // H_Pulse();
      // Dampen();
      
      // H_Pulse();
      // HH_Pulse();
      // HH_Pulse();
      
      //Signal finished transmission to due
      PORTB |= _BV(PB5);
      _delay_us(200);
      PORTB &= ~_BV(PB5);
      
      // Code[0] = 0;
      // Code[1] = 0;
      // Code[2] = 0;
      // Code[3] = 0;
        
    }
    
    Code[0] = 0;
    Code[1] = 0;
    Code[2] = 0;
    Code[3] = 0;  
  }

}


// void HH_Pulse()
// {
//   _delay_us(25);
//   TCCR1B |= _BV(CS10);
//   _delay_us(75);
//   TCCR1B &= ~_BV(CS10);
//   _delay_us(282);         
//   TCNT1 = 0;
//   TCCR1B |= _BV(CS10);
//   _delay_us(50);
//   TCCR1B &= ~_BV(CS10);
//   TCNT1 = 0;
// }

// void LH_Pulse()
// {
//   _delay_us(134);
//   TCCR1B |= _BV(CS10);
//   _delay_us(25);
//   TCCR1B &= ~_BV(CS10);
//   TCNT1 = 0;
//   _delay_us(325);
// }

// void L_Pulse()
// {
//   _delay_us(450);
// }

// void H_Pulse()
// {
//     _delay_us(25);
//   TCCR1B |= _BV(CS10);
//   _delay_us(75);
//   TCCR1B &= ~_BV(CS10);
//   _delay_us(270);         
//   TCNT1 = 0;
//   TCCR1B |= _BV(CS10);
//   _delay_us(50);
//   TCCR1B &= ~_BV(CS10);
//   TCNT1 = 0;
//    _delay_us(282);         
//   TCNT1 = 0;
//   TCCR1B |= _BV(CS10);
//   _delay_us(50);
//   TCCR1B &= ~_BV(CS10);
//   TCNT1 = 0;       
// }

void H_Pulse()
{
  _delay_us(25);
  TCCR1B |= _BV(CS10);
  _delay_us(75);
  TCCR1B &= ~_BV(CS10);
  TCNT1 = 0;
}

void Dampen()
{
  _delay_us(282);         
  TCCR1B |= _BV(CS10);
  _delay_us(50);
  TCCR1B &= ~_BV(CS10);
  TCNT1 = 0;
  H_Pulse();
}

void Amplify()
{
  _delay_us(270);         
  TCCR1B |= _BV(CS10);
  _delay_us(50);
  TCCR1B &= ~_BV(CS10);
  TCNT1 = 0;
}

void get_code()
{
  // Code[0] = PIND & _BV(PD7);
  // Code[1] = PIND & _BV(PD6);
  // Code[2] = PIND & _BV(PD5);
  // Code[3] = PIND & _BV(PD4);

  if(PIND & _BV(PD7))
  {
    Code[3] = 1;
  }
  else
  {
    Code[3] = 0;
  }
  
  if(PIND & _BV(PD6))
  {
    Code[2] = 1;
  }
  else
  {
    Code[2] = 0;
  }
  
  if(PIND & _BV(PD5))
  {
    Code[1] = 1;
  }
  else
  {
    Code[1] = 0;
  }

  if(PIND & _BV(PD4))
  {
    Code[0] = 1;
  }
  else
  {
    Code[0] = 0;
  }
  
  
  // Code[3] = (PIND & _BV(PD7));
  // Code[2] = (PIND & _BV(PD6));
  // Code[1] = (PIND & _BV(PD5));
  // Code[0] = (PIND & _BV(PD4));

  // Serial.print("Code: ");
  // Serial.println(Code[0]);
  // Serial.println(Code[1]);
  // Serial.println(Code[2]);
  // Serial.println(Code[3]);
  // Serial.println();

}

void Signal_Generator()
{
  if(Code[0])
  {
    H_Pulse();
  }
  else
  {
    return;
  }
  if(Code[1] == 1)
  {
    Amplify();
  }
  else if(Code[1] == 0)
  {
    Dampen();
  }
  
  if(Code[1] == Code[2])
  {
    Amplify();
  }
  else if(Code[1] != Code[2])
  {
    Dampen();
  }

  if(Code[2] == Code[3])
  {
    Amplify();
  }
  else if(Code[2] != Code[3])
  {
    Dampen();
  }
}