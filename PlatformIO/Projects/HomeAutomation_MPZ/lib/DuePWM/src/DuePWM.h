//(!)(!)(!)(!)(!)(!)(!)(!)(!)ATTENTION(!)(!)(!)(!)(!)(!)(!)(!)(!)(!)
//In order to use this library properly the setup method has to be called AFTER any other arduino like 
//pin initializsations (via pinMode)
//Otherwise the setup will overwrite the settings provided by pinMode (reason unkown)

#ifndef DuePWM_h
#define DuePWM_h

//Defines for the PWM Channels
#define PWM0 0   
#define PWM1 1
#define PWM2 2
#define PWM3 3
#define PWM4 4
#define PWM5 5
#define PWM6 6
#define PWM7 7
#define NumOfPWMChannels 8



class DuePWM
{
    public:

    //General Methods
    
    //General setup of the Due's PWM modules. You can specify the frequency and the channels you want to initialize
    void setup(int freq = 1000, int ch0 = -1, int ch1 = -1, int ch2 = -1, int ch3 = -1, int ch4 = -1, int ch5 = -1, int ch6 = -1, int ch7 = -1);
    
    //Methods to change the PWM duty cycle. Input value in % (e.g. 50)
    void duty(int percent, int ch0 = -1, int ch1 = -1, int ch2 = -1, int ch3 = -1, int ch4 = -1, int ch5 = -1, int ch6 = -1, int ch7 = -1);
    void duty0(int percent);
    void duty1(int percent);
    void duty2(int percent);
    void duty3(int percent);
    void duty4(int percent);
    void duty5(int percent);
    void duty6(int percent);
    void duty7(int percent);

    //Methods to change the PWM duty cycle. Input value is the actual register value. Direct access, no conversion! 
    void duty_direct(int val, int ch0 = -1, int ch1 = -1, int ch2 = -1, int ch3 = -1, int ch4 = -1, int ch5 = -1, int ch6 = -1, int ch7 = -1);
    void duty_direct0(int val);
    void duty_direct1(int val);
    void duty_direct2(int val);
    void duty_direct3(int val);
    void duty_direct4(int val);
    void duty_direct5(int val);
    void duty_direct6(int val);
    void duty_direct7(int val);

    //Methods to change the PWM period. Input value is the actual register value. Direct access, no conversion!
    void period_direct(int val, int ch0 = -1, int ch1 = -1, int ch2 = -1, int ch3 = -1, int ch4 = -1, int ch5 = -1, int ch6 = -1, int ch7 = -1); 
    void period_direct0(int val);
    void period_direct1(int val);
    void period_direct2(int val);
    void period_direct3(int val);
    void period_direct4(int val);
    void period_direct5(int val);
    void period_direct6(int val);
    void period_direct7(int val);

    //Methods to change the PWM frequency (Hz)
    void freq(int freq, int ch0 = -1, int ch1 = -1, int ch2 = -1, int ch3 = -1, int ch4 = -1, int ch5 = -1, int ch6 = -1, int ch7 = -1);
    void freq0(int freq);
    void freq1(int freq);
    void freq2(int freq);
    void freq3(int freq);
    void freq4(int freq);
    void freq5(int freq);
    void freq6(int freq);
    void freq7(int freq);

    //Methods to enable (a) specific PWM channel(s)
    void ena(int ch0 = -1, int ch1 = -1, int ch2 = -1, int ch3 = -1, int ch4 = -1, int ch5 = -1, int ch6 = -1, int ch7 = -1);
    void ena0();
    void ena1();
    void ena2();
    void ena3();
    void ena4();
    void ena5();
    void ena6();
    void ena7();
    void enaAll();      //enables all PWM channels

    //Methods to disable (a) specific PWM channel(s)
    void dis(int ch0 = -1, int ch1 = -1, int ch2 = -1, int ch3 = -1, int ch4 = -1, int ch5 = -1, int ch6 = -1, int ch7 = -1);
    void dis0();
    void dis1();
    void dis2();
    void dis3();
    void dis4();
    void dis5();
    void dis6();
    void dis7();
    void disAll();      //disables all PWM channels
    

    //Get Methods

    //Get methods to read the current period. The period in this case is the register value not the period in seconds (1/f) !!
    int get_period();           //Method to get the "default" period which is set through PWM Setup
    int get_period(int pwm_ch);
    int get_period0();
    int get_period1();
    int get_period2();
    int get_period3();
    int get_period4();
    int get_period5();
    int get_period6();
    int get_period7();

    //Get methods to read the current period in seconds. ATTENTION(!) Make sure to specify the number of decimal places when using serial.print
    double get_period_sec(int pwm_ch);
    double get_period_sec0();
    double get_period_sec1();
    double get_period_sec2();
    double get_period_sec3();
    double get_period_sec4();
    double get_period_sec5();
    double get_period_sec6();
    double get_period_sec7();

    //Get Methods to read the current duty cycle. The duty cycle in this case is the regitser value not the duty cyle in %!
    int get_duty(int pwm_ch);
    int get_duty0();
    int get_duty1();
    int get_duty2();
    int get_duty3();
    int get_duty4();
    int get_duty5();
    int get_duty6();
    int get_duty7();

    //Get methods to read the current duty cycle in percent
    int get_duty_pct(int pwm_ch);
    int get_duty_pct0();
    int get_duty_pct1();
    int get_duty_pct2();
    int get_duty_pct3();
    int get_duty_pct4();
    int get_duty_pct5();
    int get_duty_pct6();
    int get_duty_pct7();

    //Get Methods to read the current frequency
    int get_freq();           //read the frequency specified through the setup method
    int get_freq(int pwm_ch);
    int get_freq0();
    int get_freq1();
    int get_freq2();
    int get_freq3();
    int get_freq4();
    int get_freq5();
    int get_freq6();
    int get_freq7();
    

    //Constructor
    DuePWM();
    DuePWM(int CLK);

    //Destructor
    ~DuePWM();
    
    private:

    int DUE_CLK;

    int frequency;          //temperory storage which is used at initialization
    int period;             //temperory storage which is used at initialization

    //variables to store the duty cycle. The variables hold the register values not the duty cycle in %!!
    int ch_duty[NumOfPWMChannels];
    
    // int ch0_duty;
    // int ch1_duty;
    // int ch2_duty;
    // int ch3_duty;
    // int ch4_duty;
    // int ch5_duty;
    // int ch6_duty;
    // int ch7_duty;

    //variables to store the period. The variables hold the register values not the period in seconds!
    int ch_period[NumOfPWMChannels];
    
    // int ch0_period;
    // int ch1_period;
    // int ch2_period;
    // int ch3_period;
    // int ch4_period;
    // int ch5_period;
    // int ch6_period;
    // int ch7_period;


    //Global variables
    //bool PWMch[NumOfPWMChannels];

    // bool PWMch0;
    // bool PWMch1;
    // bool PWMch2;
    // bool PWMch3;
    // bool PWMch4;
    // bool PWMch5;
    // bool PWMch6;
    // bool PWMch7;

};


#endif 