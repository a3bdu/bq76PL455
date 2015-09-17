#ifndef __ECAN1_CONFIG_H__
#define __ECAN1_CONFIG_H__

#include "ecan1drv.h"

/* CAN Baud Rate Configuration 		*/
#define FCAN  	40000000
#define BITRATE 1000000
#define NTQ 	20		// 20 Time Quanta in a Bit Time
#define BRP_VAL		((FCAN/(2*NTQ*BITRATE))-1)

/* CAN Message Buffer Configuration */
#define  ECAN1_MSG_BUF_LENGTH 	32
typedef unsigned int ECAN1MSGBUF [ECAN1_MSG_BUF_LENGTH][8];
extern ECAN1MSGBUF  ecan1msgBuf __attribute__((space(dma)));


/* Function Prototype 	*/
extern void ecan1Init(void);
extern void dma1init(void);
extern void dma2init(void);

#endif
