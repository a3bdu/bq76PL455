/* 
 * File:   spi.h
 * Author: Carlo
 *
 * Created on 5 marzo 2014, 9.39
 */
#include "rtwtypes.h"

#ifndef SPI_H
#define	SPI_H

#ifdef bq76PL455
#include "protocol_bq76PL455.h"
#endif


void InitSPI(void);
//void Write_SPI(unsigned int DevAdr, unsigned int RegAdr, unsigned int Data);
void init_Ti(unsigned int DevAdr, unsigned int RegAdr, unsigned int Data);
void Init_DevicesTi(void);
void Check_Dev(void);
//void REQ_Read_SPI(  int DevAdr,  int RegAdr,   int Size);
int Read_SPI( int DevAdr,   int RegAdr,   int Size);
void Reset_SPI(void);
void Sleep_SET(void);
void Wake_up_SET(void);
void set_autoAdressing();
#endif	/* SPI_H */

