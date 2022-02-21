#ifndef MeasureStates_h
#define MeasureStates_h



//For more information on what happens during the states check out the MeasureClass (USonicMeasure)

void MeasureInit();      
void MeasureCycle();     
void StartTransmission();       //State to start a measurement. Starts transmission of an ultrasonic signal
void EndTransmission();         //State to determine the end of the ultrasonic transmission
void SeekObstacle();            //Waits for the reflection(s) of the transmitted signal
void ReadObstacle();            //Evaluates the maximum of the reflected signal (for a specific duration)
void EvalObstacle();            //Evaluates the measurement data (distance in cm and max signal "strength" in volts)
void SerialOutObstacle();       //Outputs the measurement data to the serial port

#endif