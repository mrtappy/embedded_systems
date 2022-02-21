#ifndef SystemStates_h
#define SystemStates_h

void smSystemInit();    //First state of the SystemAutomat
void SystemCycle();     //Observation state of the SystemAutomat

void HomeMenu();                    //Displays the menu options.
void FetchHomeMenu();               //Evaluates the user input based on the menu options
void ContMeasure();                 //State for continuous measurement. States checks for user input (which ends the measurement) after each measurement period
void EndMeasure();                  //This state is called if the user ends the continuous measurement. Displays the options for the next steps
void FetchEndMeasure();             //Evaluates the user input based on the options defined in the EndMeasure state
void InitIntervalMeasure();         //Initializes the interval based measurement
void FetchIntervalMeasure();        //Prompts user to input the intervall length
void IntervalMeasure();             //Evaluates the user input and stores the intervall length
void EndIntervalMeasure();          //Is called after the interval measurement is done. Display options fot next steps
void FetchEndIntervalMeasure();     //Evaluates the user input based on the options given in EndIntervalMeasure
void RumbleTest();                  //This state starts the test procedure for the rumble motors (see RumbleStates.h)
void EndProgram();                  //This state is called if the user does not want to performe any other actions
void SerialFlush();                 //This state is used to empty the serial buffer
void ResetString();                 //This state is used to clear the temporary string which is needed for the "SerialProcessing" library
void SerialReset();                 //Clears the serial buffer and resets the temporary string

#endif