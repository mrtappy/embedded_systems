#define F_CPU 16000000L 
#include <avr/io.h>
#include <util/delay.h>

void setup() {
  // put your setup code here, to run once:

  DDRB = 0b00100000; 

}

void loop() {
  // put your main code here, to run repeatedly:

   PORTB = 0b00100000;
        _delay_ms(100);
        PORTB = 0b00000000; 
        _delay_ms(100);


        
}
