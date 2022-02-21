#ifndef _SCOMM_h
#define _SCOMM_h

//Function Prototypes

//Transmit
void USART_init(uint32_t);
void USART_Transmit(uint8_t);
void USART_Transmit_String(char[]);
void USART_Transmit_Num(double);

//Receive
char USART_Receive();
int USART_Receive_String(char* str, int);
void USART_Receive_String_Echo(char*);
int RESET_STRING(char*, int );

#endif