
//VMU - Carlo Campanale 07/04/2012

#include "p33fxxxx.h"
#include "ecan2config.h"

/* Dma Initialization for ECAN2 Transmission */

void dma6init(void){
	DMACS0=0;
	DMA6CON=0x2020;
	DMA6PAD=(int)&C2TXD;		/* ECAN 2 (C2TXD) */
	DMA6CNT=0x0007;
	DMA6REQ=0x0047; 	/* ECAN 2 Transmit */
	DMA6STA= __builtin_dmaoffset(&ecan2msgBuf);
	DMA6CONbits.CHEN=1;
}


/* Dma Initialization for ECAN2 Reception */
void dma7init(void){
	 DMACS0=0;
         DMA7CON=0x0020;
	 DMA7PAD=(int)&C2RXD;	/* ECAN 2 (C2RXD) */
 	 DMA7CNT=0x0007;
	 DMA7REQ=0x0037;	/* ECAN 2 Receive */
	 DMA7STA=__builtin_dmaoffset(&ecan2msgBuf);
	 DMA7CONbits.CHEN=1;
}

void ecan2ClkInit(void){

	/* FCAN is selected to be FCY */
	/* FCAN = FCY = 40MHz */
	C2CTRL1bits.CANCKS = 0x1;

	/*
	Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 + Phase Segment 2)=20*TQ
	Phase Segment 1 = 8TQ
	Phase Segment 2 = 6Tq
	Propagation Delay = 5Tq
	Sync Segment = 1TQ
	CiCFG1<BRP> =(FCAN /(2 ×N×FBAUD))? 1
	*/

	/* Synchronization Jump Width set to 4 TQ */
	C2CFG1bits.SJW = 0x3;
	/* Baud Rate Prescaler */
	C2CFG1bits.BRP = BRP_VAL;


	/* Phase Segment 1 time is 8 TQ */
	C2CFG2bits.SEG1PH=0x7;
	/* Phase Segment 2 time is set to be programmable */
	C2CFG2bits.SEG2PHTS = 0x1;
	/* Phase Segment 2 time is 6 TQ */
	C2CFG2bits.SEG2PH = 0x5;
	/* Propagation Segment time is 5 TQ */
	C2CFG2bits.PRSEG = 0x4;
	/* Bus line is sampled three times at the sample point */
	C2CFG2bits.SAM = 0x1;

}



void ecan2Init(void){

	/* Request Configuration Mode */

	C2CTRL1bits.REQOP=4;
	while(C2CTRL1bits.OPMODE!=4);

	ecan2ClkInit();

	C2FCTRLbits.DMABS=0b011;		/* 12 CAN Message Buffers in DMA RAM */


	/*	Filter Configuration

	ecan2WriteRxAcptFilter(int n, long identifier, unsigned int exide,unsigned int bufPnt,unsigned int maskSel)

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

 	ecan2WriteRxAcptFilter(0,0x010,0,2,0);
	ecan2WriteRxAcptFilter(1,0x010,0,3,0);


	/*	Mask Configuration

	ecan2WriteRxAcptMask(int m, long identifierMask, unsigned int mide, unsigned int exide)

	m = 0 to 2 -> Mask Number

	identifier -> SID <10:0> : EID <17:0>

	mide = 0 -> Match either standard or extended address message if filters match
	mide = 1 -> Match only message types that correpond to 'exide' bit in filter

	exide = 0 -> Match messages with standard identifier addresses
	exide = 1 -> Match messages with extended identifier addresses

	*/

	ecan2WriteRxAcptMask(0,0x000,0,0);


	/* clear window bit to access ECAN control registers */
	C2CTRL1bits.WIN=0;


	/* Enter Normal Mode */

	C2CTRL1bits.REQOP=0;
	while(C2CTRL1bits.OPMODE!=0);

	/* ECAN transmit/receive message control */

	C2RXFUL1=C2RXFUL2=C2RXOVF1=C2RXOVF2=0x0000;
	C2TR01CONbits.TXEN0=1;			/* ECAN2, Buffer 0 is a Transmit Buffer */
	C2TR01CONbits.TXEN1=1;			/* ECAN2, Buffer 1 is a Transmit Buffer */
	C2TR23CONbits.TXEN2=0;			/* ECAN2, Buffer 2 is a Receive Buffer */
	C2TR23CONbits.TXEN3=0;			/* ECAN2, Buffer 3 is a Receive Buffer */


	C2TR01CONbits.TX0PRI=0b11; 		/* Message Buffer 0 Priority Level */
	C2TR01CONbits.TX1PRI=0b11; 		/* Message Buffer 1 Priority Level */




	/* configure the device to interrupt on the receive buffer full flag */
	/* clear the buffer full flags */
	C2RXFUL1=0;
	C2INTFbits.RBIF=0;

}
