#ifndef _SerialProcessing_h
#define _SerialProcessing_h

#define len 20

//Resets the global and temporary variables used for the READ_String function
void RESET_STRING(char*);

//Read a string over the serial port. The end of the string is marked by pressing "enter".
void READ_String(char*);

//Read a single char form the serial port
char READ_Char();           

//Read a integer value form the serial port. 
//Uses the READ_String function(!). RESET_STRING needs to be called afterwards
int READ_int(char*);        

//Read a double value form the serial port. 
//Uses the READ_String function(!). RESET_STRING needs to be called afterwards
double READ_double (char*);

//Clear the input buffer on the arduino side
char SERIAL_FLUSH();     

//Read a single digit from the serial port
int READ_Digit(); 


#endif
