#ifndef MeasureClass_h
#define MeasureClass_h

class USonicMeasure 
{
    public:

    //Methods
    double CalcSpeedOfSound();                      //Calculate the speed of sound based on the temperature value intialized by the constructor
    double CalcSpeedOfSound(double room_temp);      //Calculate the speed of sound based on the input
    void Start();                                   //Start ultrasonic transmitter with default values (At the moment this starts the PWM on an Arduino Nano)
    //void Start(int TrigPin, long TransTime);        //Start ultrasonic transmitter with custom values
    void Start(int PWMch, long TransTime);
    bool End();                                     //Ends Ultrasonic transmission after "TransTime" (given in the Start method)
    int GetStartTime();                             //Outputs the time at which the transmission (and the measurement) was started (micros() value!)
    bool ObstacleDetect();                          //Waits for high signal on DetectPin initialized by the constructor (The DetectPin reads a comparator output)
    bool ObstacleDetect(int DectPin);               //Same as "no args" version but uses a custom DetectPin
    long Duration();                                //Outputs the current measurement duration (based on transmission start)
    bool FindMax();                                 //Searches for a maximum (ADC value) for a given duration  (MaxFindMaxDuration). Uses the default channel initialized by the constructor
    bool FindMax(int AnalogChannel);                //Same as above with custom "analogRead channel"
    bool FindMax(long Duration, int AnalogChannel); //Same as above with custom duration and channel
    void EvalObstacle();                            //Evaluation of the measurement data. Calculates the distance in cm and the max value in volts
    void SerialOut();                               //Prints the evaluated data to the serial port

   
    //Set Methods to set the object attributes to custom values
    void SetEnvTemperature(double room_temp);       
    void SetTriggerPin(int TrigPin);
    void SetMaxMeasureDuration(long MaxDur);
    void SetDetectPin(int DetPin);
    void SetAnalogChannel(int AnalogCh);
    void SetMaxFindMaxDuration(long MaxFindMax);
    void SetTransmissionTime(long TransTime);
    void SetCSV_Separator(char separator);
    void SetTransmissionMeasureDelay(long delay);
    void SetMinDistance(double MinDis);
   

    //Get Methods to output(get) the values of the objects attributes
    double GetEnvTemperature();
    double GetAirSpeed();
    int GetTriggerPin();
    long GetMaxMeasureDuration();
    int GetDetectPin();
    int GetAnalogChannel();
    long GetMaxFindMaxDuration();
    long GetTransmissionTime();
    char GetCSV_Separator();
    double GetDistance();
    int GetTimeToObstacle();
    int GetMax();                       //Displays the ADC value!
    int GetNumOfObstacles();            //Displays number of obstacles based on the number of evaluated "packages" (single objects can be identified as separate "pulse packages" in the ultrasonic signal)
    long GetTransmissionMeasureDelay();
    double GetMinDistance();



    //Constructors
    USonicMeasure();
    USonicMeasure(double room_temp);
    USonicMeasure(long MaxMeasureDur);
    USonicMeasure(int TrigPin, int DetPin, int AnalogCh);


    private:
    long TransmissionTime;             //Defines the length of the ultrasonic transmission, default is 150 microseconds
    long TransmissionStart;            //Stores the time at which the transmission was started using micros(). Serves as start point of the measurement
    long TransmissionDuration;         //Stores the current transmission duration which is compared to TransmissionTime to determine the end of the transmission
    long TransmissionMeasureDelay;     //Stores the delay time (us) between end of transmission and start of actual measurement. This delay is necessary due to transmitter interfernce in the receiver
    long TimeToObstacle;               //Stores the elapsed time between measurement start and obstacle detection (= komparator high)
    long FindMaxStart;                 //Stores the start point of the FindMax method using micros()
    long FindMaxDuration;              //Stores the duration of the search for the maximum using micros(). This value is compared to MaxFindMaxDuration to determine the end of the search
    long MaxFindMaxDuration;           //Maximum duration of the searach for a max value
    long MeasureDuration;              //Stores the current duration of the entire measurement. This value is compared to the MaxMeasureDuration
    long MaxMeasureDuration;           //Defines the maximum measurement duration. Comparison to MeasureDuration determines the start of a new measurement
    
    
    int TriggerPin;                    //Defines which Arduino pin is used to trigger the Nano (start PWM @ Arduino Nanao)
    int DetectPin;                     //Defines the pin to which the comparator output is connected
    int AnalogChannel;                 //Defines the analog channel (Arduino) to which the envelope detector is connected
    int ReadADC0;                      //Stores the max value read from the ADC
    int ReadADC1;                      //Temp storage for comparison
    int NumOfObstacles;                //Stores the number of obstacles detected during one measurement period
    int MeasurePWMch;

    double AirSpeed;                   //Stores the speed of sound based on the first order approximation (only temperature dependent)
    double distance;                   //Stores the distance to an object
    double PulseHeight;                //Stores the converted ADC value in volts
    double EnvironmentTemperature;     //Stores the environment temperature

    char CSV_Separator;                //Defines the character used to separate the values in the serial output


};

#endif 