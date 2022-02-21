#include "RumbleClass.h"

//Constructors
Rumble::Rumble()
    :min_distance {50}, sector_max{}, sector_slope{}, sector_offset{}, sector_PWM_ch{}, sector_pos{0}
{
    sector_max[0] = 150;
    sector_max[1] = 250;
    sector_max[2] = 350;
    sector_max[3] = 450;

    sector_PWM_ch[0] = PWM3;
    sector_PWM_ch[1] = PWM2;
    sector_PWM_ch[2] = PWM1;
    sector_PWM_ch[3] = PWM0;

    Rumble::CalcFunc(SEC0);
    Rumble::CalcFunc(SEC1);
    Rumble::CalcFunc(SEC2);
    Rumble::CalcFunc(SEC3);
}

Rumble::Rumble(int sec0, int sec1, int sec2, int sec3, int PWM_ch0, int PWM_ch1, int PWM_ch2, int PWM_ch3)
    :min_distance {50}, sector_max{}, sector_slope{}, sector_offset{}, sector_PWM_ch{}, sector_pos{0}
{
    sector_max[0] = sec0;
    sector_max[1] = sec1;
    sector_max[2] = sec2;
    sector_max[3] = sec3;

    sector_PWM_ch[0] = PWM_ch0;
    sector_PWM_ch[1] = PWM_ch1;
    sector_PWM_ch[2] = PWM_ch2;
    sector_PWM_ch[3] = PWM_ch3;

    Rumble::CalcFunc(SEC0);
    Rumble::CalcFunc(SEC1);
    Rumble::CalcFunc(SEC2);
    Rumble::CalcFunc(SEC3);
}

Rumble::Rumble(int CLK, int sec0, int sec1, int sec2, int sec3, int PWM_ch0, int PWM_ch1, int PWM_ch2, int PWM_ch3)
    :DuePWM(CLK), min_distance {50}, sector_max{}, sector_pos{0}
{
    sector_max[0] = sec0;
    sector_max[1] = sec1;
    sector_max[2] = sec2;
    sector_max[3] = sec3;

    sector_PWM_ch[0] = PWM_ch0;
    sector_PWM_ch[1] = PWM_ch1;
    sector_PWM_ch[2] = PWM_ch2;
    sector_PWM_ch[3] = PWM_ch3;
  
    Rumble::CalcFunc(SEC0);
    Rumble::CalcFunc(SEC1);
    Rumble::CalcFunc(SEC2);
    Rumble::CalcFunc(SEC3);
}

//General Methods
void Rumble::CalcFunc(int SecNum)
{
    if(SecNum == 0)
    {
        sector_slope[SecNum] = (double) 100/(min_distance - sector_max[SecNum]);
        sector_offset[SecNum] = -sector_slope[SecNum]*sector_max[SecNum];
    }
    else
    {
        sector_slope[SecNum] = (double) 100/(sector_max[SecNum-1] - sector_max[SecNum]);
        sector_offset[SecNum] = -sector_slope[SecNum]*sector_max[SecNum]; 
    }
    sector_pos++;
}

int Rumble::CalcDuty(double dist, int sec)
{
    int speed {0};
    if(dist < min_distance)
    {
        speed = 100;
    }
    else
    {
        double temp_speed = (double) sector_slope[sec]*dist+sector_offset[sec];
        speed = (int) temp_speed;
    }

   Rumble::duty(speed, sec);

   return speed;
}

void Rumble::CalcFunc_direct(int SecNum)
{
   if(SecNum == 0)
   {
        sector_slope[SecNum] = (double) -(Rumble::GetSecPeriod(Rumble::GetSecPWMch(SecNum))/(min_distance-sector_max[SecNum]));
        sector_offset[SecNum] = -sector_slope[SecNum]*min_distance;
   }
   else
   {
        sector_slope[SecNum] = (double) -(Rumble::GetSecPeriod(Rumble::GetSecPWMch(SecNum)))/(sector_max[SecNum-1]-sector_max[SecNum]);
        sector_offset[SecNum] = -sector_slope[SecNum]*sector_max[SecNum-1];
   }
}

void Rumble::SetMode(bool mode)
{
    if(mode)
    {
        Rumble::CalcFunc_direct(0);
        Rumble::CalcFunc_direct(1);
        Rumble::CalcFunc_direct(2);
        Rumble::CalcFunc_direct(3);
    }
}

void Rumble::ena(int sec0, int sec1, int sec2, int sec3, int sec4, int sec5, int sec6, int sec7)
{
    DuePWM::ena(Rumble::GetSecPWMch(sec0),Rumble::GetSecPWMch(sec1),Rumble::GetSecPWMch(sec2),Rumble::GetSecPWMch(sec3),Rumble::GetSecPWMch(sec4),Rumble::GetSecPWMch(sec5),Rumble::GetSecPWMch(sec7));
}

void Rumble::dis(int sec0, int sec1, int sec2, int sec3, int sec4, int sec5, int sec6, int sec7)
{
    DuePWM::dis(Rumble::GetSecPWMch(sec0),Rumble::GetSecPWMch(sec1),Rumble::GetSecPWMch(sec2),Rumble::GetSecPWMch(sec3),Rumble::GetSecPWMch(sec4),Rumble::GetSecPWMch(sec5),Rumble::GetSecPWMch(sec7));
}

void Rumble::duty(int percent, int sec0, int sec1, int sec2, int sec3, int sec4, int sec5, int sec6, int sec7)
{
    DuePWM::duty(percent, Rumble::GetSecPWMch(sec0),Rumble::GetSecPWMch(sec1),Rumble::GetSecPWMch(sec2),Rumble::GetSecPWMch(sec3),Rumble::GetSecPWMch(sec4),Rumble::GetSecPWMch(sec5),Rumble::GetSecPWMch(sec7));
}

//Set Methods
void Rumble::SetSecMax(int sec, int max, bool calc_slope)
{
    if(sec <= MaxNumOfSectors)
    {
        sector_max[sec] = max;

        if(calc_slope)
        {
            Rumble::CalcFunc(sec);

            if((sec < MaxNumOfSectors) && (sector_max[sec+1] != 0))
                Rumble::CalcFunc(sec+1);      
        }   
    }
}

void Rumble::SetSecSlope(int sec, int slope)
{
    if(sec <= MaxNumOfSectors)
    {
        sector_max[sec] = slope;
    }
}

void Rumble::SetSecOffset(int sec, int offset)
{
    if(sec < MaxNumOfSectors)
    {
        sector_max[sec] = offset;
    }
}

void Rumble::SetSecPWMch(int sec, int PWM_ch)
{
    if(sec < MaxNumOfSectors)
    {
        sector_PWM_ch[sec] = PWM_ch;
    }
    
}

//Get Methods
int Rumble::GetSecMax(int sec)
{
    if(sec < MaxNumOfSectors)
    {
        return sector_max[sec];
    }
    else
    {
        return 0;
    }
}

double Rumble::GetSecSlope(int sec)
{
    if(sec < MaxNumOfSectors)
    {
        return sector_slope[sec];
    }
    else
    {
        return 0;
    }
}

int Rumble::GetSecOffset(int sec)
{
    if(sec < MaxNumOfSectors)
    {
        return sector_offset[sec];
    }
    else
    {
        return 0;
    }
}

int Rumble::GetSecPWMch(int sec)
{
    if(sec < MaxNumOfSectors && sec >= 0)
    {
        return sector_PWM_ch[sec];
    }
    else
    {
        return -1;
    }
}


int Rumble::GetSecPeriod(int sec)
{
    int temp_PWMch {0};
    
    if(sec < MaxNumOfSectors && sec >= 0)
    {
        temp_PWMch = sector_PWM_ch[sec];
    }
    else
    {
        temp_PWMch = -1;
    }
    
    return Rumble::get_period(temp_PWMch);

    //Wrong implementation
    // if(PWM_ch == 0)
    // { 
    //     return Rumble::get_period0();
    // }
    // else if(PWM_ch == 1)
    // {
    //     return Rumble::get_period1();
    // }
    // else if(PWM_ch == 2)
    // {
    //     return Rumble::get_period2();
    // }
    // else if(PWM_ch == 3)
    // {
    //     return Rumble::get_period3();
    // }
    // else if(PWM_ch == 4)
    // {
    //     return Rumble::get_period4();
    // }
    // else if(PWM_ch == 5)
    // {
    //     return Rumble::get_period5();
    // }
    // else if(PWM_ch == 6)
    // {
    //     return Rumble::get_period6();
    // }
    // else if(PWM_ch == 7)
    // {
    //     return Rumble::get_period7();
    // }
}

int Rumble::GetMinDistance()
{
    return min_distance;
}





