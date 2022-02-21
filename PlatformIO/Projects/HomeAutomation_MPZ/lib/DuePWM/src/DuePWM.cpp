#include "DuePWM.h"
#include <Arduino.h>


//Global variables
bool PWMch[NumOfPWMChannels];


//Constructor
DuePWM::DuePWM()
    : DUE_CLK {84000000}, ch_duty {0}, ch_period {0}
{

}

DuePWM::DuePWM(int CLK)
    : DUE_CLK {CLK}, ch_duty {0}, ch_period {0}
{

}

//Destructor
DuePWM::~DuePWM()
{
    
    if(PWMch[PWM0])
        PWMch[PWM0] = !PWMch[PWM0];

    if(PWMch[PWM1])
        PWMch[PWM1] = !PWMch[PWM1];

    if(PWMch[PWM2])
        PWMch[PWM2] = !PWMch[PWM2];
    
    if(PWMch[PWM3])
        PWMch[PWM3] = !PWMch[PWM3];
    
    if(PWMch[PWM4])
        PWMch[PWM4] = !PWMch[PWM4];
    
    if(PWMch[PWM5])
        PWMch[PWM5] = !PWMch[PWM5];
    
    if(PWMch[PWM6])
        PWMch[PWM6] = !PWMch[PWM6];
    
    if(PWMch[PWM7])
        PWMch[PWM7] = !PWMch[PWM7];
}


//General Methods

//Setup------------------------------------------------------------------------------------------------
void  DuePWM::setup(int freq, int ch0, int ch1, int ch2, int ch3, int ch4, int ch5, int ch6, int ch7)
{
    frequency = freq;

    if(freq > 5)
    {
        period = DUE_CLK/(2*freq);
    }
    else
    {
        period = 8388608; //max value for the channel period register of the DUE is 2^23
    }

    //Ports
    PIOC -> PIO_PDR |= PIO_PDR_P21 | PIO_PDR_P2 | PIO_PDR_P4 | PIO_PDR_P6 | PIO_PDR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Enable peripheral control of PC21

    //PWM Setup 
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | ~PMC_WPMR_WPEN; //Enable PMC write access
    PMC -> PMC_PCER1 = PMC_PCER1_PID36; //Enable PWM peripheral clock
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | PMC_WPMR_WPEN; //Disable PMC write access
    
    PIOC -> PIO_ABSR |= PIO_ABSR_P21 | PIO_ABSR_P2 | PIO_ABSR_P4 | PIO_ABSR_P6 | PIO_ABSR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Select peripheral type B which is PWM Output

    PWM -> PWM_WPCR = PWM_WPCR_WPKEY(0x50574D) | 0x00; //Enable write access on all groups
    PWM -> PWM_CLK = PWM_CLK_PREA(1); //PREA_DIV  = 2


    if((ch0 == 0 || ch1 == 0 || ch2 == 0 || ch3 == 0 || ch4 == 0 || ch5 == 0 || ch6 == 0 || ch7 == 0) && !PWMch[PWM0])
    {
        //PWM Channel 0 - Pin 34 = PC2 (PWML0)
        REG_PWM_CMR0 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 0, left aligned 
        REG_PWM_CPRD0 = PWM_CPRD_CPRD(period);   //Set PWM period divider to x. 84Mhz / (2*x) = frequency
        REG_PWM_CDTY0 = PWM_CDTY_CDTY(period);   //default value
        PWM -> PWM_ENA |= PWM_ENA_CHID0;
        //ch0_duty = period;
        //ch0_period = period;
        //PWMch0 = true;
        ch_duty[PWM0] = period;
        ch_period[PWM0] = period;
        PWMch[PWM0] = true;
    }

    if((ch0 == 1 || ch1 == 1 || ch2 == 1 || ch3 == 1 || ch4 == 1 || ch5 == 1 || ch6 == 1 || ch7 == 1) && !PWMch[PWM1])
    {
        //PWM Channel 1 - Pin 36 = PC4 (PWML1)
        REG_PWM_CMR1 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 1, left aligned 
        REG_PWM_CPRD1 = PWM_CPRD_CPRD(period);   //Set PWM period divider to x. 84Mhz / (2*x) = frequency
        REG_PWM_CDTY1 = PWM_CDTY_CDTY(period); //default value
        PWM -> PWM_ENA |= PWM_ENA_CHID1;
        //ch1_duty = period;
        //ch1_period = period;
        //PWMch1 = true;
        ch_duty[PWM1] = period;
        ch_period[PWM1] = period;
        PWMch[PWM1] = true;

    }

    if((ch0 == 2 || ch1 == 2 || ch2 == 2 || ch3 == 2 || ch4 == 2 || ch5 == 2 || ch6 == 2 || ch7 == 2) && !PWMch[PWM2])
    {
        //PWM Channel 2 - Pin 38 = PC6 (PWML2)
        REG_PWM_CMR2 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 2, left aligned 
        REG_PWM_CPRD2 = PWM_CPRD_CPRD(period);   
        REG_PWM_CDTY2 = PWM_CDTY_CDTY(period); //default value
        PWM -> PWM_ENA |= PWM_ENA_CHID2;
        // ch2_duty = period;
        // ch2_period = period;
        // PWMch2 = true;
        ch_duty[PWM2] = period;
        ch_period[PWM2] = period;
        PWMch[PWM2] = true;
    }

    if((ch0 == 3 || ch1 == 3 || ch2 == 3 || ch3 == 3 || ch4 == 3 || ch5 == 3 || ch6 == 3 || ch7 == 3) && !PWMch[PWM3])
    {
        // PWM Channel 3 - Pin 40 = PC8 (PWML3)
        REG_PWM_CMR3 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 3, left aligned 
        REG_PWM_CPRD3 = PWM_CPRD_CPRD(period);   
        REG_PWM_CDTY3 = PWM_CDTY_CDTY(period); //default value
        PWM -> PWM_ENA |= PWM_ENA_CHID3;
        // ch3_duty = period;
        // ch3_period = period;
        // PWMch3 = true;
        ch_duty[PWM3] = period;
        ch_period[PWM3] = period;
        PWMch[PWM3] = true;
    }

    if((ch0 == 4 || ch1 == 4 || ch2 == 4 || ch3 == 4 || ch4 == 4 || ch5 == 4 || ch6 == 4 || ch7 == 4) && !PWMch[PWM4])
    {
        //PWM Channel 4 - Pin 9 = PC21 (PWM4L)
        REG_PWM_CMR4 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
        REG_PWM_CPRD4 = PWM_CPRD_CPRD(period);   
        REG_PWM_CDTY4 = PWM_CDTY_CDTY(period);
        PWM -> PWM_ENA |= PWM_ENA_CHID4;
        // ch4_duty = period;
        // ch4_period = period;
        // PWMch4 = true;
        ch_duty[PWM4] = period;
        ch_period[PWM4] = period;
        PWMch[PWM4] = true;
    }

    if((ch0 == 5 || ch1 == 5 || ch2 == 5 || ch3 == 5 || ch4 == 5 || ch5 == 5 || ch6 == 5 || ch7 == 5) && !PWMch[PWM6])
    {
        //PWM Channel 5 - Pin 8 = PC22 (PWML5)
        REG_PWM_CMR5 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
        REG_PWM_CPRD5 = PWM_CPRD_CPRD(period);   
        REG_PWM_CDTY5 = PWM_CDTY_CDTY(period); //default value
        PWM -> PWM_ENA |= PWM_ENA_CHID5;
        // ch5_duty = period;
        // ch5_period = period;
        // PWMch5 = true;
        ch_duty[PWM5] = period;
        ch_period[PWM5] = period;
        PWMch[PWM5] = true;
    }

    if((ch0 == 6 || ch1 == 6 || ch2 == 6 || ch3 == 6 || ch4 == 6 || ch5 == 6 || ch6 == 6 || ch7 == 6) && !PWMch[PWM6])
    {
        //PWM Channel 6 - Pin 7 = PC23 (PWML6)
        REG_PWM_CMR6 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
        REG_PWM_CPRD6 = PWM_CPRD_CPRD(period);   
        REG_PWM_CDTY6 = PWM_CDTY_CDTY(period); //default value
        PWM -> PWM_ENA |= PWM_ENA_CHID6;
        // ch6_duty = period;
        // ch6_period = period;
        // PWMch6 = true;
        ch_duty[PWM6] = period;
        ch_period[PWM6] = period;
        PWMch[PWM6] = true;
    }

    if((ch0 == 7 || ch1 == 7 || ch2 == 7 || ch3 == 7 || ch4 == 7 || ch5 == 7 || ch6 == 7 || ch7 == 7) && !PWMch[PWM7])
    {
        //PWM Channel 7 - Pin 6 = PC24 (PWML7)
        REG_PWM_CMR7 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
        REG_PWM_CPRD7 = PWM_CPRD_CPRD(period);   
        REG_PWM_CDTY7 = PWM_CDTY_CDTY(period); //default value
        PWM -> PWM_ENA |= PWM_ENA_CHID7;
        // ch7_duty = period;
        // ch7_period = period;
        // PWMch7 = true;
        ch_duty[PWM7] = period;
        ch_period[PWM7] = period;
        PWMch[PWM7] = true;
    }

}
//--------------------------------------------------------------------------------------


//Set duty cycle-------------------------------------------------------------------------
void DuePWM::duty(int percent, int ch0, int ch1, int ch2, int ch3, int ch4, int ch5, int ch6, int ch7)
{
    if((ch0 == 0 || ch1 == 0 || ch2 == 0 || ch3 == 0 || ch4 == 0 || ch5 == 0 || ch6 == 0 || ch7 == 0) && PWMch[PWM0])
    {
        DuePWM::duty0(percent);
    }
    if((ch0 == 1 || ch1 == 1 || ch2 == 1 || ch3 == 1 || ch4 == 1 || ch5 == 1 || ch6 == 1 || ch7 == 1) && PWMch[PWM1])
    {
        DuePWM::duty1(percent);
    }
    if((ch0 == 2 || ch1 == 2 || ch2 == 2 || ch3 == 2 || ch4 == 2 || ch5 == 2 || ch6 == 2 || ch7 == 2) && PWMch[PWM2])
    {
        DuePWM::duty2(percent);
    }
    if((ch0 == 3 || ch1 == 3 || ch2 == 3 || ch3 == 3 || ch4 == 3 || ch5 == 3 || ch6 == 3 || ch7 == 3) && PWMch[PWM3])
    {
        DuePWM::duty3(percent);
    }
    if((ch0 == 4 || ch1 == 4 || ch2 == 4 || ch3 == 4 || ch4 == 4 || ch5 == 4 || ch6 == 4 || ch7 == 4) && PWMch[PWM4])
    {
        DuePWM::duty4(percent);
    }
    if((ch0 == 5 || ch1 == 5 || ch2 == 5 || ch3 == 5 || ch4 == 5 || ch5 == 5 || ch6 == 5 || ch7 == 5) && PWMch[PWM5])
    {
        DuePWM::duty5(percent);
    }
    if((ch0 == 6 || ch1 == 6 || ch2 == 6 || ch3 == 6 || ch4 == 6 || ch5 == 6 || ch6 == 6 || ch7 == 6) && PWMch[PWM6])
    {
        DuePWM::duty6(percent);
    }
    if((ch0 == 7 || ch1 == 7 || ch2 == 7 || ch3 == 7 || ch4 == 7 || ch5 == 7 || ch6 == 7 || ch7 == 7) && PWMch[PWM7])
    {
        DuePWM::duty7(percent);
    }
}

void DuePWM::duty0(int percent)
{
    ch_duty[PWM0] = ch_period[PWM0]-((ch_period[PWM0]*percent)/100);
    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM0]);
    //ch0_duty = ch0_period-((ch0_period*percent)/100);
    //REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(ch0_duty);
}

void DuePWM::duty1(int percent)
{
    ch_duty[PWM1] = ch_period[PWM1]-((ch_period[PWM1]*percent)/100);
    REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM1]);
    // ch1_duty = ch1_period-((ch1_period*percent)/100);
    // REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(ch1_duty);
}

void DuePWM::duty2(int percent)
{
    ch_duty[PWM2] = ch_period[PWM2]-((ch_period[PWM2]*percent)/100);
    REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM2]);
    // ch2_duty = ch2_period-((ch2_period*percent)/100);
    // REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(ch2_duty);
}

void DuePWM::duty3(int percent)
{
    ch_duty[PWM3] = ch_period[PWM3]-((ch_period[PWM3]*percent)/100);
    REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM3]);
    // ch3_duty = ch3_period-((ch3_period*percent)/100);
    // REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(ch3_duty);
}

void DuePWM::duty4(int percent)
{
    ch_duty[PWM4] = ch_period[PWM4]-((ch_period[PWM4]*percent)/100);
    REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM4]);
    // ch4_duty = ch4_period-((ch4_period*percent)/100);
    // REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(ch4_duty);
}

void DuePWM::duty5(int percent)
{
    ch_duty[PWM5] = ch_period[PWM5]-((ch_period[PWM5]*percent)/100);
    REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM5]);
    // ch5_duty = ch5_period-((ch5_period*percent)/100);
    // REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(ch5_duty);
}

void DuePWM::duty6(int percent)
{
    ch_duty[PWM6] = ch_period[PWM6]-((ch_period[PWM6]*percent)/100);
    REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM6]);
    // ch6_duty = ch6_period-((ch6_period*percent)/100);
    // REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(ch6_duty);
}

void DuePWM::duty7(int percent)
{
    ch_duty[PWM7] = ch_period[PWM7]-((ch_period[PWM7]*percent)/100);
    REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(ch_duty[PWM7]);
    // ch7_duty = ch7_period-((ch7_period*percent)/100);
    // REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(ch7_duty);
}
//--------------------------------------------------------------------------------------

//Set duty cycle with actual register value, no conversion
void DuePWM::duty_direct(int val, int ch0, int ch1, int ch2, int ch3, int ch4, int ch5, int ch6, int ch7)
{
    if((ch0 == 0 || ch1 == 0 || ch2 == 0 || ch3 == 0 || ch4 == 0 || ch5 == 0 || ch6 == 0 || ch7 == 0) && PWMch[PWM0])
    {
        DuePWM::duty_direct0(val);
    }
    if((ch0 == 1 || ch1 == 1 || ch2 == 1 || ch3 == 1 || ch4 == 1 || ch5 == 1 || ch6 == 1 || ch7 == 1) && PWMch[PWM1])
    {
        DuePWM::duty_direct1(val);
    }
    if((ch0 == 2 || ch1 == 2 || ch2 == 2 || ch3 == 2 || ch4 == 2 || ch5 == 2 || ch6 == 2 || ch7 == 2) && PWMch[PWM2])
    {
        DuePWM::duty_direct2(val);
    }
    if((ch0 == 3 || ch1 == 3 || ch2 == 3 || ch3 == 3 || ch4 == 3 || ch5 == 3 || ch6 == 3 || ch7 == 3) && PWMch[PWM3])
    {
        DuePWM::duty_direct3(val);
    }
    if((ch0 == 4 || ch1 == 4 || ch2 == 4 || ch3 == 4 || ch4 == 4 || ch5 == 4 || ch6 == 4 || ch7 == 4) && PWMch[PWM4])
    {
        DuePWM::duty_direct4(val);
    }
    if((ch0 == 5 || ch1 == 5 || ch2 == 5 || ch3 == 5 || ch4 == 5 || ch5 == 5 || ch6 == 5 || ch7 == 5) && PWMch[PWM5])
    {
        DuePWM::duty_direct5(val);
    }
    if((ch0 == 6 || ch1 == 6 || ch2 == 6 || ch3 == 6 || ch4 == 6 || ch5 == 6 || ch6 == 6 || ch7 == 6) && PWMch[PWM6])
    {
        DuePWM::duty_direct6(val);
    }
    if((ch0 == 7 || ch1 == 7 || ch2 == 7 || ch3 == 7 || ch4 == 7 || ch5 == 7 || ch6 == 7 || ch7 == 7) && PWMch[PWM7])
    {
        DuePWM::duty_direct7(val);
    }
}

void DuePWM::duty_direct0(int val)
{
    ch_duty[PWM0] = val;
    //ch0_duty = val;
    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(val);
}

void DuePWM::duty_direct1(int val)
{
    ch_duty[PWM1] = val;
    //ch1_duty = val;
    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(val);
}

void DuePWM::duty_direct2(int val)
{
    ch_duty[PWM2] = val;
    //ch2_duty = val;
    REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(val);
}

void DuePWM::duty_direct3(int val)
{
    ch_duty[PWM3] = val;
    //ch3_duty = val;
    REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(val);
}

void DuePWM::duty_direct4(int val)
{
    ch_duty[PWM4] = val;
    //ch4_duty = val;
    REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(val);
}

void DuePWM::duty_direct5(int val)
{
    ch_duty[PWM5] = val;
    //ch5_duty = val;
    REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(val);
}

void DuePWM::duty_direct6(int val)
{
    ch_duty[PWM6] = val;
    //ch6_duty = val;
    REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(val);
}

void DuePWM::duty_direct7(int val)
{
    ch_duty[PWM7] = val;
    //ch7_duty = val;
    REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(val);
}
//--------------------------------------------------------------------------------------


//Set frequency-------------------------------------------------------------------------
void DuePWM::freq(int freq, int ch0, int ch1, int ch2, int ch3, int ch4, int ch5, int ch6, int ch7)
{
    if((ch0 == 0 || ch1 == 0 || ch2 == 0 || ch3 == 0 || ch4 == 0 || ch5 == 0 || ch6 == 0 || ch7 == 0) && PWMch[PWM0])
    {
        DuePWM::freq0(freq);
    }
    if((ch0 == 1 || ch1 == 1 || ch2 == 1 || ch3 == 1 || ch4 == 1 || ch5 == 1 || ch6 == 1 || ch7 == 1) && PWMch[PWM1])
    {
        DuePWM::freq1(freq);
    }
    if((ch0 == 2 || ch1 == 2 || ch2 == 2 || ch3 == 2 || ch4 == 2 || ch5 == 2 || ch6 == 2 || ch7 == 2) && PWMch[PWM2])
    {
        DuePWM::freq2(freq);
    }
    if((ch0 == 3 || ch1 == 3 || ch2 == 3 || ch3 == 3 || ch4 == 3 || ch5 == 3 || ch6 == 3 || ch7 == 3) && PWMch[PWM3])
    {
        DuePWM::freq3(freq);
    }
    if((ch0 == 4 || ch1 == 4 || ch2 == 4 || ch3 == 4 || ch4 == 4 || ch5 == 4 || ch6 == 4 || ch7 == 4) && PWMch[PWM4])
    {
        DuePWM::freq4(freq);
    }
    if((ch0 == 5 || ch1 == 5 || ch2 == 5 || ch3 == 5 || ch4 == 5 || ch5 == 5 || ch6 == 5 || ch7 == 5) && PWMch[PWM5])
    {
        DuePWM::freq5(freq);
    }
    if((ch0 == 6 || ch1 == 6 || ch2 == 6 || ch3 == 6 || ch4 == 6 || ch5 == 6 || ch6 == 6 || ch7 == 6) && PWMch[PWM6])
    {
        DuePWM::freq6(freq);
    }
    if((ch0 == 7 || ch1 == 7 || ch2 == 7 || ch3 == 7 || ch4 == 7 || ch5 == 7 || ch6 == 7 || ch7 == 7) && PWMch[PWM7])
    {
        DuePWM::freq7(freq);
    }
}

void DuePWM::freq0(int freq)
{
    if(freq > 5)
    {
        //ch0_period = DUE_CLK/(2*freq);
        ch_period[PWM0] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD0 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM0]);
    //REG_PWM_CPRDUPD0 = PWM_CPRDUPD_CPRDUPD(ch0_period);
}

void DuePWM::freq1(int freq)
{
    /*if(freq > 5)
    {
        ch1_period = DUE_CLK/(2*freq);
    }

    REG_PWM_CPRDUPD1 = PWM_CPRDUPD_CPRDUPD(ch1_period);*/
    if(freq > 5)
    {
        ch_period[PWM1] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD1 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM1]);
}

void DuePWM::freq2(int freq)
{
    // if(freq > 5)
    // {
    //     ch2_period = DUE_CLK/(2*freq);
    // }

    // REG_PWM_CPRDUPD2 = PWM_CPRDUPD_CPRDUPD(ch2_period);

    if(freq > 5)
    {
        ch_period[PWM2] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD2 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM2]);
}

void DuePWM::freq3(int freq)
{
    // if(freq > 5)
    // {
    //     ch3_period = DUE_CLK/(2*freq);
    // }

    // REG_PWM_CPRDUPD3 = PWM_CPRDUPD_CPRDUPD(ch3_period);
    if(freq > 5)
    {
        ch_period[PWM3] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD3 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM3]);
}

void DuePWM::freq4(int freq)
{
    // if(freq > 5)
    // {
    //     ch4_period = DUE_CLK/(2*freq);
    // }

    // REG_PWM_CPRDUPD4 = PWM_CPRDUPD_CPRDUPD(ch4_period);
    if(freq > 5)
    {
        ch_period[PWM4] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD4 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM4]);
}

void DuePWM::freq5(int freq)
{
    // if(freq > 5)
    // {
    //     ch5_period = DUE_CLK/(2*freq);
    // }

    // REG_PWM_CPRDUPD5 = PWM_CPRDUPD_CPRDUPD(ch5_period);
    if(freq > 5)
    {
        ch_period[PWM5] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD0 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM5]);
}

void DuePWM::freq6(int freq)
{
    // if(freq > 5)
    // {
    //     ch6_period = DUE_CLK/(2*freq);
    // }

    // REG_PWM_CPRDUPD6 = PWM_CPRDUPD_CPRDUPD(ch6_period);
    if(freq > 5)
    {
        ch_period[PWM6] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD6 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM6]);
}

void DuePWM::freq7(int freq)
{
    // if(freq > 5)
    // {
    //     ch7_period = DUE_CLK/(2*freq);
    // }

    // REG_PWM_CPRDUPD7 = PWM_CPRDUPD_CPRDUPD(ch7_period);
    if(freq > 5)
    {
        ch_period[PWM7] = DUE_CLK/(2*freq);
    }
    REG_PWM_CPRDUPD7 = PWM_CPRDUPD_CPRDUPD(ch_period[PWM7]);
}
//--------------------------------------------------------------------------------

//Set period with actual register value, no conversion
void DuePWM::period_direct(int val, int ch0, int ch1, int ch2, int ch3, int ch4, int ch5, int ch6, int ch7)
{
    if((ch0 == 0 || ch1 == 0 || ch2 == 0 || ch3 == 0 || ch4 == 0 || ch5 == 0 || ch6 == 0 || ch7 == 0) && PWMch[PWM0])
    {
        DuePWM::period_direct0(val);
    }
    if((ch0 == 1 || ch1 == 1 || ch2 == 1 || ch3 == 1 || ch4 == 1 || ch5 == 1 || ch6 == 1 || ch7 == 1) && PWMch[PWM1])
    {
        DuePWM::period_direct1(val);
    }
    if((ch0 == 2 || ch1 == 2 || ch2 == 2 || ch3 == 2 || ch4 == 2 || ch5 == 2 || ch6 == 2 || ch7 == 2) && PWMch[PWM2])
    {
        DuePWM::period_direct2(val);
    }
    if((ch0 == 3 || ch1 == 3 || ch2 == 3 || ch3 == 3 || ch4 == 3 || ch5 == 3 || ch6 == 3 || ch7 == 3) && PWMch[PWM3])
    {
        DuePWM::period_direct3(val);
    }
    if((ch0 == 4 || ch1 == 4 || ch2 == 4 || ch3 == 4 || ch4 == 4 || ch5 == 4 || ch6 == 4 || ch7 == 4) && PWMch[PWM4])
    {
        DuePWM::period_direct4(val);
    }
    if((ch0 == 5 || ch1 == 5 || ch2 == 5 || ch3 == 5 || ch4 == 5 || ch5 == 5 || ch6 == 5 || ch7 == 5) && PWMch[PWM5])
    {
        DuePWM::period_direct5(val);
    }
    if((ch0 == 6 || ch1 == 6 || ch2 == 6 || ch3 == 6 || ch4 == 6 || ch5 == 6 || ch6 == 6 || ch7 == 6) && PWMch[PWM6])
    {
        DuePWM::period_direct6(val);
    }
    if((ch0 == 7 || ch1 == 7 || ch2 == 7 || ch3 == 7 || ch4 == 7 || ch5 == 7 || ch6 == 7 || ch7 == 7) && PWMch[PWM7])
    {
        DuePWM::period_direct7(val);
    }
}


void DuePWM::period_direct0(int val)
{
    //ch0_period = val;
    ch_period[PWM0] = val;
    REG_PWM_CPRDUPD0 = PWM_CPRDUPD_CPRDUPD(val);
}

void DuePWM::period_direct1(int val)
{
    //ch1_period = val;
    ch_period[PWM1] = val;
    REG_PWM_CPRDUPD1 = PWM_CPRDUPD_CPRDUPD(val);
}

void DuePWM::period_direct2(int val)
{
    //ch2_period = val;
    ch_period[PWM2] = val;
    REG_PWM_CPRDUPD2 = PWM_CPRDUPD_CPRDUPD(val);
}

void DuePWM::period_direct3(int val)
{
    //ch3_period = val;
    ch_period[PWM3] = val;
    REG_PWM_CPRDUPD3 = PWM_CPRDUPD_CPRDUPD(val);
}

void DuePWM::period_direct4(int val)
{
    //ch4_period = val;
    ch_period[PWM4] = val;
    REG_PWM_CPRDUPD4 = PWM_CPRDUPD_CPRDUPD(val);
}

void DuePWM::period_direct5(int val)
{
    //ch5_period = val;
    ch_period[PWM5] = val;
    REG_PWM_CPRDUPD5 = PWM_CPRDUPD_CPRDUPD(val);
}

void DuePWM::period_direct6(int val)
{
    //ch6_period = val;
    ch_period[PWM6] = val;
    REG_PWM_CPRDUPD6 = PWM_CPRDUPD_CPRDUPD(val);
}

void DuePWM::period_direct7(int val)
{
    //ch7_period = val;
    ch_period[PWM7] = val;
    REG_PWM_CPRDUPD7 = PWM_CPRDUPD_CPRDUPD(val);
}

//--------------------------------------------------------------------------------



//Enable-------------------------------------------------------------------------
void DuePWM::ena(int ch0, int ch1, int ch2, int ch3, int ch4, int ch5, int ch6, int ch7)
{
    if((ch0 == 0 || ch1 == 0 || ch2 == 0 || ch3 == 0 || ch4 == 0 || ch5 == 0 || ch6 == 0 || ch7 == 0) && PWMch[PWM0])
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID0;
    }
    if((ch0 == 1 || ch1 == 1 || ch2 == 1 || ch3 == 1 || ch4 == 1 || ch5 == 1 || ch6 == 1 || ch7 == 1) && PWMch[PWM1]
    )
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID1;
    }
    if((ch0 == 2 || ch1 == 2 || ch2 == 2 || ch3 == 2 || ch4 == 2 || ch5 == 2 || ch6 == 2 || ch7 == 2) && PWMch[PWM2])
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID2;
    }
    if((ch0 == 3 || ch1 == 3 || ch2 == 3 || ch3 == 3 || ch4 == 3 || ch5 == 3 || ch6 == 3 || ch7 == 3) && PWMch[PWM3])
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID3;
    }
    if((ch0 == 4 || ch1 == 4 || ch2 == 4 || ch3 == 4 || ch4 == 4 || ch5 == 4 || ch6 == 4 || ch7 == 4) && PWMch[PWM4])
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID4;
    }
    if((ch0 == 5 || ch1 == 5 || ch2 == 5 || ch3 == 5 || ch4 == 5 || ch5 == 5 || ch6 == 5 || ch7 == 5) && PWMch[PWM5])
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID5;
    }
    if((ch0 == 6 || ch1 == 6 || ch2 == 6 || ch3 == 6 || ch4 == 6 || ch5 == 6 || ch6 == 6 || ch7 == 6) && PWMch[PWM6])
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID6;
    }
    if((ch0 == 7 || ch1 == 7 || ch2 == 7 || ch3 == 7 || ch4 == 7 || ch5 == 7 || ch6 == 7 || ch7 == 7) && PWMch[PWM7])
    {
        PWM -> PWM_ENA |= PWM_ENA_CHID7;
    }
}

void DuePWM::ena0()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID0;
}

void DuePWM::ena1()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID1;
}

void DuePWM::ena2()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID2;
}

void DuePWM::ena3()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID3;
}

void DuePWM::ena4()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID4;
}

void DuePWM::ena5()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID5;
}

void DuePWM::ena6()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID6;
}

void DuePWM::ena7()
{
    PWM -> PWM_ENA |= PWM_ENA_CHID7;
}

void DuePWM::enaAll()
{
    PWM -> PWM_ENA = PWM_ENA_CHID0 | PWM_ENA_CHID1 | PWM_ENA_CHID2 | PWM_ENA_CHID3 | PWM_ENA_CHID4 | PWM_ENA_CHID5 | PWM_ENA_CHID6 | PWM_ENA_CHID7;
}
//--------------------------------------------------------------------------------


//Disable-------------------------------------------------------------------------
void DuePWM::dis(int ch0, int ch1, int ch2, int ch3, int ch4, int ch5, int ch6, int ch7)
{
    if((ch0 == 0 || ch1 == 0 || ch2 == 0 || ch3 == 0 || ch4 == 0 || ch5 == 0 || ch6 == 0 || ch7 == 0) && PWMch[PWM0])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID0;
    }
    if((ch0 == 1 || ch1 == 1 || ch2 == 1 || ch3 == 1 || ch4 == 1 || ch5 == 1 || ch6 == 1 || ch7 == 1) && PWMch[PWM1])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID1;
    }
    if((ch0 == 2 || ch1 == 2 || ch2 == 2 || ch3 == 2 || ch4 == 2 || ch5 == 2 || ch6 == 2 || ch7 == 2) && PWMch[PWM2])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID2;
    }
    if((ch0 == 3 || ch1 == 3 || ch2 == 3 || ch3 == 3 || ch4 == 3 || ch5 == 3 || ch6 == 3 || ch7 == 3) && PWMch[PWM3])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID3;
    }
    if((ch0 == 4 || ch1 == 4 || ch2 == 4 || ch3 == 4 || ch4 == 4 || ch5 == 4 || ch6 == 4 || ch7 == 4) && PWMch[PWM4])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID4;
    }
    if((ch0 == 5 || ch1 == 5 || ch2 == 5 || ch3 == 5 || ch4 == 5 || ch5 == 5 || ch6 == 5 || ch7 == 5) && PWMch[PWM5])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID5;
    }
    if((ch0 == 6 || ch1 == 6 || ch2 == 6 || ch3 == 6 || ch4 == 6 || ch5 == 6 || ch6 == 6 || ch7 == 6) && PWMch[PWM6])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID6;
    }
    if((ch0 == 7 || ch1 == 7 || ch2 == 7 || ch3 == 7 || ch4 == 7 || ch5 == 7 || ch6 == 7 || ch7 == 7) && PWMch[PWM7])
    {
        PWM -> PWM_DIS |= PWM_DIS_CHID7;
    }
}

void DuePWM::dis0()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID0;
}

void DuePWM::dis1()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID1;
}

void DuePWM::dis2()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID2;
}

void DuePWM::dis3()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID3;
}

void DuePWM::dis4()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID4;
}

void DuePWM::dis5()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID5;
}

void DuePWM::dis6()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID6;
}

void DuePWM::dis7()
{
    PWM -> PWM_DIS |= PWM_DIS_CHID7;
}

void DuePWM::disAll()
{
    PWM -> PWM_DIS = PWM_DIS_CHID0 | PWM_DIS_CHID1 | PWM_DIS_CHID2 | PWM_DIS_CHID3 | PWM_DIS_CHID4 | PWM_DIS_CHID5 | PWM_DIS_CHID6 | PWM_DIS_CHID7;
}
//--------------------------------------------------------------------------------


//Get Methods

//Periods--------------------------------------
int DuePWM::get_period()
{
    return period;
}

int DuePWM::get_period(int pwm_ch)
{
    return ch_period[pwm_ch];
}

int DuePWM::get_period0()
{
    //return ch0_period;
    return ch_period[PWM0];
}

int DuePWM::get_period1()
{
    //return ch1_period;
    return ch_period[PWM1];
}

int DuePWM::get_period2()
{
    //return ch2_period;
    return ch_period[PWM2];
}

int DuePWM::get_period3()
{
    //return ch3_period;
    return ch_period[PWM3];
}

int DuePWM::get_period4()
{
    //return ch4_period;
    return ch_period[PWM4];
}

int DuePWM::get_period5()
{
    //return ch5_period;
    return ch_period[PWM5];
}

int DuePWM::get_period6()
{
    //return ch6_period;
    return ch_period[PWM6];
}

int DuePWM::get_period7()
{
    //return ch7_period;
    return ch_period[PWM7];
}
//-------------------------------------------------

//Periods in seconds-------------------------------
double DuePWM::get_period_sec(int pwm_ch)
{
    return (double) 1/(DuePWM::get_freq(pwm_ch));
}

double DuePWM::get_period_sec0()
{
    return (double) 1/(DuePWM::get_freq0());
}

double DuePWM::get_period_sec1()
{
    return (double) 1/(DuePWM::get_freq1());
}

double DuePWM::get_period_sec2()
{
    return (double) 1/(DuePWM::get_freq2());
}

double DuePWM::get_period_sec3()
{
    return (double) 1/(DuePWM::get_freq3());
}

double DuePWM::get_period_sec4()
{
    return (double) 1/(DuePWM::get_freq4());
}

double DuePWM::get_period_sec5()
{
    return (double) 1/(DuePWM::get_freq5());
}

double DuePWM::get_period_sec6()
{
    return (double) 1/(DuePWM::get_freq6());
}

double DuePWM::get_period_sec7()
{
    return (double) 1/(DuePWM::get_freq7());
}
//-------------------------------------------------

//Duty Cycles--------------------------------------
int DuePWM::get_duty(int pwm_ch)
{
    //Still needs to be implemented
    return ch_duty[pwm_ch];
}

int DuePWM::get_duty0()
{
    //return ch0_duty;
    return ch_duty[PWM0];
}

int DuePWM::get_duty1()
{
    //return ch1_duty;
    return ch_duty[PWM1];
}

int DuePWM::get_duty2()
{
    //return ch2_duty;
    return ch_duty[PWM2];
}

int DuePWM::get_duty3()
{
    //return ch3_duty;
    return ch_duty[PWM3];
}

int DuePWM::get_duty4()
{
    //return ch4_duty;
    return ch_duty[PWM4];
}

int DuePWM::get_duty5()
{
    //return ch5_duty;
    return ch_duty[PWM5];
}

int DuePWM::get_duty6()
{
    //return ch6_duty;
   return ch_duty[PWM6]; 
}

int DuePWM::get_duty7()
{
    //return ch7_duty;
    return ch_duty[PWM7];
}
//------------------------------------------------------------

//Duty Cycles in percent--------------------------------------
int DuePWM::get_duty_pct(int pwm_ch)
{
    return (1-(ch_duty[pwm_ch]/ch_period[pwm_ch]));
}

int DuePWM::get_duty_pct0()
{
    //return (1-(ch0_duty/ch0_period));
    return (1-(ch_duty[PWM0]/ch_period[PWM0]));
}

int DuePWM::get_duty_pct1()
{
    //return (1-(ch1_duty/ch1_period));
    return (1-(ch_duty[PWM1]/ch_period[PWM1]));
}

int DuePWM::get_duty_pct2()
{
    //return (1-(ch2_duty/ch2_period));
    return (1-(ch_duty[PWM2]/ch_period[PWM2]));
}

int DuePWM::get_duty_pct3()
{
    //return (1-(ch3_duty/ch3_period));
    return (1-(ch_duty[PWM3]/ch_period[PWM3]));
}

int DuePWM::get_duty_pct4()
{
    //return (1-(ch4_duty/ch4_period));
    return (1-(ch_duty[PWM4]/ch_period[PWM4]));
}

int DuePWM::get_duty_pct5()
{
    //return (1-(ch5_duty/ch5_period));
    return (1-(ch_duty[PWM5]/ch_period[PWM5]));
}

int DuePWM::get_duty_pct6()
{
    //return (1-(ch6_duty/ch6_period));
    return (1-(ch_duty[PWM6]/ch_period[PWM6]));
}

int DuePWM::get_duty_pct7()
{
    //return (1-(ch7_duty/ch7_period));
    return (1-(ch_duty[PWM7]/ch_period[PWM7]));
}
//------------------------------------------------------------

//frequency---------------------------------------------------
int DuePWM::get_freq()
{
    return frequency;
}

int DuePWM::get_freq(int pwm_ch)
{
    return (DUE_CLK/(2*ch_period[pwm_ch]));
}

int DuePWM::get_freq0()
{
    //return (DUE_CLK/(2*ch0_period));
    return (DUE_CLK/(2*ch_period[PWM0]));
}

int DuePWM::get_freq1()
{
    //return (DUE_CLK/(2*ch1_period));
    return (DUE_CLK/(2*ch_period[PWM1]));
}

int DuePWM::get_freq2()
{
    //return (DUE_CLK/(2*ch2_period));
    return (DUE_CLK/(2*ch_period[PWM2]));
}

int DuePWM::get_freq3()
{
    //return (DUE_CLK/(2*ch3_period));
    return (DUE_CLK/(2*ch_period[PWM3]));
}

int DuePWM::get_freq4()
{
    //return (DUE_CLK/(2*ch4_period));
    return (DUE_CLK/(2*ch_period[PWM4]));
}

int DuePWM::get_freq5()
{
    //return (DUE_CLK/(2*ch5_period));
    return (DUE_CLK/(2*ch_period[PWM5]));
}

int DuePWM::get_freq6()
{
    //return (DUE_CLK/(2*ch6_period));
    return (DUE_CLK/(2*ch_period[PWM6]));
}

int DuePWM::get_freq7()
{
    //return (DUE_CLK/(2*ch7_period));
    return (DUE_CLK/(2*ch_period[PWM7]));
}
//-----------------------------------------------
