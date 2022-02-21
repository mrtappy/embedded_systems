#ifndef RumbleStates_h
#define RumnbleStates_h


//Rumble State Machine prototypes
void RumbleInit();     //First state for RumbleAutomat
void RumbleCycle();    //Observation state for Rumble Automat

//The MotorControlSEC states control the intensity, enabling and disabling of the rumble motors based on the measured distances.
//SEC0 determines the sector (distance range) of the first obstalce that is detected during a measurement.
//For example: If the first obstacle iswithin sector 1 (default: in the distance range of 150 cm and 250 cm)
//the rumble motor for sector 0 is disabled.
//In that casee the next state will be MotorControlSEC2 since sector 1 has already been used.  
void MotorControlSEC0();        
void MotorControlSEC1();
void MotorControlSEC2();
void MotorControlSEC3();
void MotorControlSEC4();

//The RumbleTestSEC states set the intensity of the rumble motor for the specified sector to 100%.
//The intention of these states is to test if the motors are working properly.
void RumbleTestSEC0();
void RumbleTestSEC1();
void RumbleTestSEC2();
void RumbleTestSEC3();

//--------------------------------

#endif  