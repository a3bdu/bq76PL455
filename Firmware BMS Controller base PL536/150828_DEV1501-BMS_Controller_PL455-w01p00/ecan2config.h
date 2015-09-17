/* 
 * File:   ecan2config.h
 * Author: Carlo.Campanale
 *
 * Created on 17 febbraio 2015, 11.27
 */

#ifndef __ECAN2_CONFIG_H__
#define __ECAN2_CONFIG_H__

#include "ecan2drv.h"

/* CAN Baud Rate Configuration 		*/
#define FCAN  	40000000
#define BITRATE 1000000
#define NTQ 	20		// 20 Time Quanta in a Bit Time
#define BRP_VAL		((FCAN/(2*NTQ*BITRATE))-1)


/* CAN Message Buffer Configuration */
#define  ECAN2_MSG_BUF_LENGTH   32
typedef unsigned int ECAN2MSGBUF [ECAN2_MSG_BUF_LENGTH][8];
extern ECAN2MSGBUF  ecan2msgBuf __attribute__((space(dma)));

/* Function Prototype 	*/
extern void ecan2Init(void);
extern void dma6init(void);
extern void dma7init(void);

#endif
