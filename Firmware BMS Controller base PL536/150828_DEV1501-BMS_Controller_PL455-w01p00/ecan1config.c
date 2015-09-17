/*********************************************************************
* File:        ecan1config.c
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r00
*
* Created on 7 Marzo 2014, 15.35
*********************************************************************/

#include "p33fxxxx.h"
#include "ecan1config.h"
#include "common.h"

/* Dma Initialization for ECAN1 Transmission */
void dma1init(void){
	 DMACS0=0;
         DMA1CON=0x2020;
	 DMA1PAD=0x0442;	/* ECAN 1 (C1TXD) */
 	 DMA1CNT=7;
	 DMA1REQ=0x0046;	/* ECAN 1 Transmit */
	 DMA1STA = __builtin_dmaoffset(&ecan1msgBuf);
         IPC3bits.DMA1IP= 2;    // DMA1 Interrupt priority level=2
	 DMA1CONbits.CHEN=1;
}

/* Dma Initialization for ECAN1 Reception */
void dma2init(void){
	 DMACS0=0;
         DMA2CON=0x0020;
	 DMA2PAD=0x0440;	/* ECAN 1 (C1RXD) */
 	 DMA2CNT=7;
	 DMA2REQ=0x0022;	/* ECAN 1 Receive */
	 DMA2STA = __builtin_dmaoffset(&ecan1msgBuf);
         IPC6bits.DMA2IP= 2;    // DMA2 Interrupt priority level=1
	 DMA2CONbits.CHEN=1;
}


void ecan1ClkInit(void){
	/* FCAN is selected to be FCY */
	/* FCAN = FCY = 40MHz */
	C1CTRL1bits.CANCKS = 0x1;

	/*
	Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 + Phase Segment 2)=20*TQ
//	Phase Segment 1 = 8TQ
//	Phase Segment 2 = 6Tq
//	Propagation Delay = 5Tq
//	Sync Segment = 1TQ
//	CiCFG1<BRP> =(FCAN /(2 ×N×FBAUD))– 1
	*/

	/* Synchronization Jump Width set to 4 TQ */
	C1CFG1bits.SJW = 0x3;
	/* Baud Rate Prescaler */
	C1CFG1bits.BRP = BRP_VAL;


	/* Phase Segment 1 time is 8 TQ */
	C1CFG2bits.SEG1PH=0x7;
	/* Phase Segment 2 time is set to be programmable */
	C1CFG2bits.SEG2PHTS = 0x1;
	/* Phase Segment 2 time is 6 TQ */
	C1CFG2bits.SEG2PH = 0x5;
	/* Propagation Segment time is 5 TQ */
	C1CFG2bits.PRSEG = 0x4;
	/* Bus line is sampled three times at the sample point */
	C1CFG2bits.SAM = 0x1;
}

void ecan1Init(void){


	/* Request Configuration Mode */
	C1CTRL1bits.REQOP=4;
	while(C1CTRL1bits.OPMODE!=4);

	ecan1ClkInit();

	C1FCTRLbits.DMABS=0b000;		/* 4 CAN Message Buffers in DMA RAM */



	/*	Filter Configuration

	ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide, unsigned int bufPnt, unsigned int maskSel)

	n = 0 to 15 -> Filter number

	identifier -> SID <10:0> : EID <17:0>

	exide = 0 -> Match messages with standard identifier addresses
	exide = 1 -> Match messages with extended identifier addresses

	bufPnt = 0 to 14  -> RX Buffer 0 to 14
	bufPnt = 15 -> RX FIFO Buffer

	maskSel = 0	->	Acceptance Mask 0 register contains mask
	maskSel = 1	->	Acceptance Mask 1 register contains mask
	maskSel = 2	->	Acceptance Mask 2 register contains mask
	maskSel = 3	->	No Mask Selection

*/

	ecan1WriteRxAcptFilter(0,BMS_CAN_INPUT,0,2,0);
	ecan1WriteRxAcptFilter(1,BMS_CAN_INPUT,0,3,0);





/*	Mask Configuration

	ecan1WriteRxAcptMask(int m, long identifierMask, unsigned int mide, unsigned int exide)

	m = 0 to 2 -> Mask Number

	identifier -> SID <10:0> : EID <17:0>

	mide = 0 -> Match either standard or extended address message if filters match
	mide = 1 -> Match only message types that correpond to 'exide' bit in filter

	exide = 0 -> Match messages with standard identifier addresses
	exide = 1 -> Match messages with extended identifier addresses

*/
	ecan1WriteRxAcptMask(0,0x7ff,0,0); // 0x000 per far passare tutti i pacchetti, 0x7ff per far passare solo i valori inseriti in ecan1WriteRxAcptFilter()

	/* clear window bit to access ECAN control registers */
	C1CTRL1bits.WIN=0;

/* Enter Normal Mode */
	C1CTRL1bits.REQOP=0;
	while(C1CTRL1bits.OPMODE!=0);

/* ECAN transmit/receive message control */

	C1RXFUL1=C1RXFUL2=C1RXOVF1=C1RXOVF2=0x0000;
	C1TR01CONbits.TXEN0=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C1TR01CONbits.TXEN1=1;			/* ECAN1, Buffer 1 is a Transmit Buffer */
	C1TR23CONbits.TXEN2=0;			/* ECAN1, Buffer 2 is a Receive Buffer */
	C1TR23CONbits.TXEN3=0;			/* ECAN1, Buffer 3 is a Receive Buffer */
	C1TR01CONbits.TX0PRI=0b11; 		/* Message Buffer 0 Priority Level */
	C1TR01CONbits.TX1PRI=0b11; 		/* Message Buffer 0 Priority Level */

	/* configure the device to interrupt on the receive buffer full flag */
	/* clear the buffer full flags */
	C1RXFUL1=0;
	C1INTFbits.RBIF=0;

}



