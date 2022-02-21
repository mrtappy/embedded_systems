#include "RumbleStates.h"
#include <Arduino.h>
#include <StateMachine.h>



extern StateMachine RumbleAutomat;
extern double distance;

extern bool MeasureState;
extern bool RumbleState;

double temp_speed = 250;
int speed = 250;
int sec0_max = 150;
int sec1_max = 250;
int sec2_max = 350;


//State Functions
void smRumbleInit()
{
  //Serial.println("Rumble Automat Init");
  RumbleAutomat.enter(MotorControlSEC0);
}

void MotorControlSEC0()
{
    Serial.print("SEC0: ");
    if(distance < sec0_max)
      {
        
        if(distance < 50)
        {
          speed = 0;
        }
        else
        {
          temp_speed = (double) 2.5*distance - 125;
          speed = (int) temp_speed;
        }
        
        PWM -> PWM_ENA = PWM_ENA_CHID3;
        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(speed);
        
        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC1);
      }
      
      else if(distance < sec1_max)
      {
        temp_speed = (double) 2.5*distance - 375;
        speed = (int) temp_speed;

        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID2;
        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(speed);
        
        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC2);
      }
      else if(distance <= sec2_max)
      {
        
        temp_speed = (double) 2.5*distance - 625;
        speed = (int) temp_speed;
        
        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID1;
        REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);            
   
        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC3);
      }
      else if(distance > sec2_max)
      { 
        temp_speed = (double) 2.5*distance - 875;
        speed = (int) temp_speed;
       
        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
        REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID0;
        REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
        
        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC0);
      }
      else
      {
        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC0);
      }
      
}

void MotorControlSEC1()
{
    Serial.print("SEC1: ");
    if(distance < sec0_max)
    {
      MeasureState = true;
      RumbleState = false;
      //no action section already used
    }
      
    else if(distance < sec1_max)
    {
      temp_speed = (double) 2.5*distance - 375;
      speed = (int) temp_speed;
      
      PWM -> PWM_ENA = PWM_ENA_CHID2;
      REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(speed);              
  
      MeasureState = true;
      RumbleState = false;
      RumbleAutomat.enter(MotorControlSEC2);
    }
    else if(distance < sec2_max)
    {
        temp_speed = (double) 2.5*distance - 625;
        speed = (int) temp_speed;

        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID1;
        REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);              

        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC3);
    }
    else if(distance > sec2_max)
    { 
        temp_speed = (double) 2.5*distance - 875;
        speed = (int) temp_speed;

        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
        REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
        PWM -> PWM_ENA = PWM_ENA_CHID0;
        REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
        
        MeasureState = true;
        RumbleState = false;
        RumbleAutomat.enter(MotorControlSEC0);
    }

    else
    {
      MeasureState = true;
      RumbleState = false;
      RumbleAutomat.enter(MotorControlSEC0);
    }
}

void MotorControlSEC2()
{
  Serial.print("SEC2: ");
  if(distance < sec1_max)
    {
      MeasureState = true;
      RumbleState = false;
      //no action section already used
    }

  else if(distance < sec2_max)
    {
      temp_speed = (double) 2.5*distance - 625;
      speed = (int) temp_speed;
    
      PWM -> PWM_ENA = PWM_ENA_CHID1;
      REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);              
      
      MeasureState = true;
      RumbleState = false;
      RumbleAutomat.enter(MotorControlSEC3);
    }
    else if(distance > sec2_max)
    { 
      temp_speed = (double) 2.5*distance - 875;
      speed = (int) temp_speed;
    
      REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
      PWM -> PWM_ENA = PWM_ENA_CHID0;
      REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
      
      MeasureState = true;
      RumbleState = false;
      RumbleAutomat.enter(MotorControlSEC0);
    }

    else
    {
      MeasureState = true;
      RumbleState = false;
      RumbleAutomat.enter(MotorControlSEC0);
    }

}

void MotorControlSEC3()
{
  Serial.print("SEC3: ");
  if(distance < sec2_max)
  {
    MeasureState = true;
    RumbleState = false;
    //no action section already used
  }
  
  else if(distance > sec2_max)
  { 
    temp_speed = (double) 2.5*distance - 875;
    speed = (int) temp_speed;
  
    PWM -> PWM_ENA = PWM_ENA_CHID0;
    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
    
    MeasureState = true;
    RumbleState = false;
    RumbleAutomat.enter(MotorControlSEC0);
  }
  else
  {
  
    REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(250);
    
    MeasureState = true;
    RumbleState = false;
    RumbleAutomat.enter(MotorControlSEC0);
  }
}


//Other Functions
void PWM_Setup()
{
    int default_speed = 125;
    //Ports
    PIOC -> PIO_PDR = PIO_PDR_P21 | PIO_PDR_P2 | PIO_PDR_P4 | PIO_PDR_P6 | PIO_PDR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Enable peripheral control of PC21

    //PWM Setup Use PWM 9 on Arduino pin 9
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | ~PMC_WPMR_WPEN; //Enable PMC write access
    PMC -> PMC_PCER1 = PMC_PCER1_PID36; //Enable PWM peripheral clock
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | PMC_WPMR_WPEN; //Disable PMC write access
    
    PIOC -> PIO_ABSR = PIO_ABSR_P21 | PIO_ABSR_P2 | PIO_ABSR_P4 | PIO_ABSR_P6 | PIO_ABSR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Select peripheral type B which is PWM Output

    PWM -> PWM_WPCR = PWM_WPCR_WPKEY(0x50574D) | 0x00; //Enable write access on all groups
    PWM -> PWM_CLK = PWM_CLK_PREA(1); //PREA_DIV  = 2
    REG_PWM_CMR4 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 

    //PWM Channel 0 - Pin 34 = PC2 (PWML0)
    REG_PWM_CMR0 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD0 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY0 = PWM_CDTY_CDTY(default_speed);   //default value

    //PWM Channel 1 - Pin 36 = PC4 (PWML1)
    REG_PWM_CMR1 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD1 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY1 = PWM_CDTY_CDTY(default_speed); //default value
  
    //PWM Channel 2 - Pin 38 = PC6 (PWML2)
    REG_PWM_CMR2 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD2 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY2 = PWM_CDTY_CDTY(default_speed); //default value

    // PWM Channel 3 - Pin 40 = PC8 (PWML3)
    REG_PWM_CMR3 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD3 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY3 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 4 - Pin 9 = PC21 (PWM4L)
    REG_PWM_CPRD4 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY4 = PWM_CDTY_CDTY(default_speed);

    //PWM Channel 5 - Pin 8 = PC22 (PWML5)
    REG_PWM_CMR5 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD5 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY5 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 6 - Pin 7 = PC23 (PWML6)
    REG_PWM_CMR6 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD6 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY6 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 7 - Pin 6 = PC24 (PWML7)
    REG_PWM_CMR7 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD7 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY7 = PWM_CDTY_CDTY(default_speed); //default value

    PWM -> PWM_ENA = PWM_ENA_CHID0 | PWM_ENA_CHID1 | PWM_ENA_CHID2 | PWM_ENA_CHID3 | PWM_ENA_CHID4 | PWM_ENA_CHID5 | PWM_ENA_CHID6 | PWM_ENA_CHID7;
}

void MotorOff()
{
  PWM -> PWM_DIS = PWM_DIS_CHID0 | PWM_DIS_CHID1 | PWM_DIS_CHID2 | PWM_DIS_CHID3 | PWM_DIS_CHID4 | PWM_DIS_CHID5 | PWM_DIS_CHID6 | PWM_DIS_CHID7;
}