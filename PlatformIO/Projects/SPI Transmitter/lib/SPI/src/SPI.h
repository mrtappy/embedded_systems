#ifndef _SPI_h
#define _SPI_h

//Initiate device as master
void SPI_MasterInit(void);

//Initiate devide as slave
void SPI_SlaveInit(void);

//Start SCK and data transmission
void SPI_MasterTX(uint8_t);

//Read received data from slave
uint8_t SPI_MasterRX(void);

void SlaveTransmit(uint8_t);

uint8_t SlaveReceive(void);

//declare Slave Select Ports with connected slaves
void SlaveSelect(uint8_t, uint8_t);

//Select a slave to communicate with
void InitComm(uint8_t, uint8_t);


#endif
