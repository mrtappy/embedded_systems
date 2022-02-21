
/*
    Arduino Due (SAM3x) PWM example by Daniel Tappe
*/


#include <Arduino.h>
#include <DueTimer.h>
#include <sam3.h>

int  duty_cycle = 255;

//bool PWM_ON = false;

void setup()
{
    
    //Ports
    PIOC -> PIO_PDR = PIO_PDR_P21; //Enable peripheral control of PC21

    //PWM Setup Use PWM 9 on Arduino pin 9
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | ~PMC_WPMR_WPEN; //Enable PMC write access
    PMC -> PMC_PCER1 = PMC_PCER1_PID36; //Enable PWM peripheral clock
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | PMC_WPMR_WPEN; //Disable PMC write access
    PIOC -> PIO_ABSR = PIO_ABSR_P21; //Select peripheral type B which is PWM Output

    PWM -> PWM_WPCR = PWM_WPCR_WPKEY(0x50574D) | 0x00; //Enable write access on all groups
    
    PWM -> PWM_CLK = PWM_CLK_PREA(1); //PREA_DIV  = 2
    REG_PWM_CMR4 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD4 = PWM_CPRD_CPRD(50000);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY4 = PWM_CDTY_CDTY(25000);
    //PWM -> PWM_ENA = PWM_ENA_CHID4;   //Enable PWM CHannel4 (see datasheet p.973)
    
}    


void loop()
{
    
    PWM -> PWM_ENA |= PWM_ENA_CHID4;

    delay(5000);

    REG_PWM_CPRDUPD4 = PWM_CPRD_CPRD(25000);
    REG_PWM_CDTYUPD4 = PWM_CDTY_CDTY(12500);

    delay(5000);

    REG_PWM_CPRDUPD4 = PWM_CPRD_CPRD(50000);
    REG_PWM_CDTYUPD4 = PWM_CDTY_CDTY(25000);


    //delay(5);
    //PWM -> PWM_DIS |= PWM_DIS_CHID4;
    //  delay(5);


}

