#ifndef _SerialProcessing_h
#define _SerialProcessing_h

#define len 20

void RESET_STRING(char*);
void READ_String(char*);
char READ_Char();
int READ_int(char*);
double READ_double (char*);
int SELECT_Time();
char SERIAL_FLUSH();
int READ_Digit();


#endif
