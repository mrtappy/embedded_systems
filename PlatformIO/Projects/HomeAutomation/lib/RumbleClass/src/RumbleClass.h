#ifndef RumbleClass_h
#define RumbleClass_h


#include <DuePWM.h>

//define the max number of sectors used
#define MaxNumOfSectors 10

//Sector defines for better readability
#define SEC0 0
#define SEC1 1
#define SEC2 2
#define SEC3 3
#define SEC4 4
#define SEC5 5
#define SEC6 6
#define SEC7 7
#define SEC8 8
#define SEC9 9
#define SEC10 10

//Defines for the "SetMode" method
#define RUMBLE_DIRECT true
#define RUMBLE_INDIRECT false

//Inherite Rumble class from DuePWM class
class Rumble: public DuePWM
{
    public:

    int CalcDuty(double dist, int sec);         //Method to calculate the duty cycle of the PWM based on the distance. Sector assignment is handeled via input.
    void CalcFunc(int SecNum);                  //Method to calculate the linear function(s) which is(are) used in CalcDuty
    void CalcFunc_direct(int SecNum);           //Method to calculate the linear functions used in the "CalcDuty_direct" method
    void SetMode(bool mode = false);            //Method to set the mode for CalcDuty method. False: output in percent, true: output = register value
    
    //enable PWM channles via the assigned sector
    void ena(int sec0 = -1, int sec1 = -1, int sec2 = -1, int sec3 = -1, int sec4 = -1, int sec5 = -1, int sec6 = -1, int sec7 = -1);
    
    //disable PWM channels via the assigned sector
    void dis(int sec0 = -1, int sec1 = -1, int sec2 = -1, int sec3 = -1, int sec4 = -1, int sec5 = -1, int sec6 = -1, int sec7 = -1);
    
    //Update the duty cycle of the PWM channel which is assigned to the specified sector
    void duty(int percent, int sec0 = -1, int sec1 = -1, int sec2 = -1, int sec3 = -1, int sec4 = -1, int sec5 = -1, int sec6 = -1, int sec7 = -1);
    
    //Set Methods
    void SetSecMax(int sec, int max, bool calc_slope = true);   //Method to set the upper boundaries of the sectors. calc_slope determines wether the function parameters (slope of offset) should also be adjusted
    void SetSecSlope(int sec, int max);                         //Method to set a custom slope for linear function used in "CalcDuty"
    void SetSecOffset(int sec, int max);                        //Method to set a custom offset for linear function used in "CalcDuty"
    void SetSecPWMch(int sec, int PWM_ch);                      //Method to link sectors to pwm channels.

    //Get Methods
    int GetSecMax(int sec);
    double GetSecSlope(int sec);
    int GetSecOffset(int sec);
    int GetSecPWMch(int sec);
    int GetSecPeriod(int PWM_ch);
    int GetMinDistance();
    

    //Constructors
    Rumble();
    Rumble(int sec0, int sec1, int sec2, int sec3, int PWM_ch0, int PWM_ch1, int PWM_ch2, int PWM_ch3);
    Rumble(int CLK, int sec0, int sec1, int sec2, int sec3, int PWM_ch0, int PWM_ch1, int PWM_ch2, int PWM_ch3);

    private:
    double sector_slope [MaxNumOfSectors];          //Stores the slope (m) of the linear function
    double sector_offset [MaxNumOfSectors];         //Stores the +b of the linear function

    int min_distance;                               //lowest measureable distance. This is due to transmitter inference
    int sector_max [MaxNumOfSectors];               //Stores the upper limit of the sectors
    int sector_PWM_ch [MaxNumOfSectors];            //Stores which pwm channels refers to which section and therefore to which rumble motor
    int sector_pos;
};

#endif 