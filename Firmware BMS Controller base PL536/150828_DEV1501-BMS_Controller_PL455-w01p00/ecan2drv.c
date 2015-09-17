//VMU - Carlo Campanale 07/04/2012

#include "p33fxxxx.h"
#include "ecan2config.h"

/*
This function configures Acceptance Filter "n"

Inputs:
n-> Filter number [0-15]
identifier-> Bit ordering is given below
Filter Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
								   |____________|_____________________|
									  SID10:0           EID17:0


Filter Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
														  |___________|
															  SID10:0
exide -> "0" for standard identifier
		 "1" for Extended identifier

bufPnt -> Message buffer to store filtered message [0-15]
maskSel -> Optinal Masking of identifier bits [0-3]

*/

void ecan2WriteRxAcptFilter(int n, long identifier, unsigned int exide, unsigned int bufPnt,unsigned int maskSel) {

unsigned long sid10_0=0, eid15_0=0, eid17_16=0;
unsigned int *sidRegAddr,*bufPntRegAddr,*maskSelRegAddr, *fltEnRegAddr;


	C2CTRL1bits.WIN=1;

	// Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN register for a given filter number "n"
	sidRegAddr = (unsigned int *)(&C2RXF0SID + (n << 1));
	bufPntRegAddr = (unsigned int *)(&C2BUFPNT1 + (n >> 2));
	maskSelRegAddr = (unsigned int *)(&C2FMSKSEL1 + (n >> 3));
	fltEnRegAddr = (unsigned int *)(&C2FEN1);


	// Bit-filed manupulation to write to Filter identifier register
	if(exide==1) { 	// Filter Extended Identifier
		eid15_0 = (identifier & 0xFFFF);
		eid17_16= (identifier>>16) & 0x3;
		sid10_0 = (identifier>>18) & 0x7FF;

		*sidRegAddr=(((sid10_0)<<5) + 0x8) + eid17_16;	// Write to CiRXFnSID Register
	    *(sidRegAddr+1)= eid15_0;					// Write to CiRXFnEID Register

	}else{			// Filter Standard Identifier
		sid10_0 = (identifier & 0x7FF);
		*sidRegAddr=(sid10_0)<<5;					// Write to CiRXFnSID Register
		*(sidRegAddr+1)=0;							// Write to CiRXFnEID Register
	}

   *bufPntRegAddr = (*bufPntRegAddr) & (0xFFFF - (0xF << (4 *(n & 3)))); // clear nibble
   *bufPntRegAddr = ((bufPnt << (4 *(n & 3))) | (*bufPntRegAddr));       // Write to C1BUFPNTn Register

   *maskSelRegAddr = (*maskSelRegAddr) & (0xFFFF - (0x3 << ((n & 7) * 2))); // clear 2 bits
   *maskSelRegAddr = ((maskSel << (2 * (n & 7))) | (*maskSelRegAddr));      // Write to C1FMSKSELn Register

   *fltEnRegAddr = ((0x1 << n) | (*fltEnRegAddr)); // Write to C1FEN1 Register

	C2CTRL1bits.WIN=0;

}


/*
This function configures Acceptance Filter Mask "m"

Inputs:
m-> Mask number [0-2]
identifier-> Bit ordering is given below
Filter Mask Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
								        |____________|_____________________|
									        SID10:0           EID17:0


Filter Mask Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
														       |___________|
															      SID10:0

mide ->  "0"  Match either standard or extended address message if filters match
         "1"  Match only message types that correpond to 'exide' bit in filter

*/

void ecan2WriteRxAcptMask(int m, long identifier, unsigned int mide,unsigned int exide){

unsigned long sid10_0=0, eid15_0=0, eid17_16=0;
unsigned int *maskRegAddr;


	C2CTRL1bits.WIN=1;

	// Obtain the Address of CiRXMmSID register for given Mask number "m"
	maskRegAddr = (unsigned int *)(&C2RXM0SID + (m << 1));

	// Bit-filed manupulation to write to Filter Mask register
	if(exide==1)
	 { 	// Filter Extended Identifier
		eid15_0 = (identifier & 0xFFFF);
		eid17_16= (identifier>>16) & 0x3;
		sid10_0 = (identifier>>18) & 0x7FF;

		if(mide==1)
			*maskRegAddr=((sid10_0)<<5) + 0x0008 + eid17_16;	// Write to CiRXMnSID Register
		else
			*maskRegAddr=((sid10_0)<<5) + eid17_16;	// Write to CiRXMnSID Register
	    *(maskRegAddr+1)= eid15_0;					// Write to CiRXMnEID Register

	}
	else
	{			// Filter Standard Identifier
		sid10_0 = (identifier & 0x7FF);
		if(mide==1)
			*maskRegAddr=((sid10_0)<<5) + 0x0008;					// Write to CiRXMnSID Register
		else
			*maskRegAddr=(sid10_0)<<5;					// Write to CiRXMnSID Register

		*(maskRegAddr+1)=0;							// Write to CiRXMnEID Register
	}


	C2CTRL1bits.WIN=0;

}


/* ECAN Transmit Message Buffer Configuration

Inputs:
buf	-> Transmit Buffer Number

txIdentifier ->

Extended Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
								     |____________|_____________________|
									        SID10:0           EID17:0



Standard Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
														    |___________|
															      SID10:0

Standard Message Format:
											Word0 : 0b000f ffff ffff ffff
													     |____________|||___
 									        				SID10:0   SRR   IDE

											Word1 : 0b0000 0000 0000 0000
														   |____________|
															  EID17:6

											Word2 : 0b0000 00f0 0000 ffff
													  |_____||	  	 |__|
													  EID5:0 RTR   	  DLC



				Extended Message Format:
											Word0 : 0b000f ffff ffff ffff
													     |____________|||___
 									        				SID10:0   SRR   IDE

											Word1 : 0b0000 ffff ffff ffff
														   |____________|
															  EID17:6

											Word2 : 0bffff fff0 0000 ffff
													  |_____||	  	 |__|
													  EID5:0 RTR   	  DLC

ide -> "0"  Message will transmit standard identifier
	   "1"  Message will transmit extended identifier



remoteTransmit -> "0" Message transmitted is a normal message
				  "1" Message transmitted is a remote message

				Standard Message Format:
											Word0 : 0b000f ffff ffff ff1f
													     |____________|||___
 									        				SID10:0   SRR   IDE

											Word1 : 0b0000 0000 0000 0000
														   |____________|
															  EID17:6

											Word2 : 0b0000 0010 0000 ffff
													  |_____||	  	 |__|
													  EID5:0 RTR   	  DLC



				Extended Message Format:
											Word0 : 0b000f ffff ffff ff1f
													     |____________|||___
 									        				SID10:0   SRR   IDE

											Word1 : 0b0000 ffff ffff ffff
														   |____________|
															  EID17:6

											Word2 : 0bffff ff10 0000 ffff
													  |_____||	  	 |__|
													  EID5:0 RTR   	  DLC

*/



void ecan2WriteTxMsgBufId(unsigned int buf, long txIdentifier, unsigned int ide, unsigned int remoteTransmit){

unsigned long word0=0, word1=0, word2=0;
unsigned long sid10_0=0, eid5_0=0, eid17_6=0;



if(ide)
	{
		eid5_0  = (txIdentifier & 0x3F);
		eid17_6 = (txIdentifier>>6) & 0xFFF;
		sid10_0 = (txIdentifier>>18) & 0x7FF;
		word1 = eid17_6;
	}
	else
	{
		sid10_0 = (txIdentifier & 0x7FF);
	}


	if(remoteTransmit==1) { 	// Transmit Remote Frame

		word0 = ((sid10_0 << 2) | ide | 0x2);
		word2 = ((eid5_0 << 10)| 0x0200);}

	else {

		word0 = ((sid10_0 << 2) | ide);
		word2 = (eid5_0 << 10);
	     }

// Obtain the Address of Transmit Buffer in DMA RAM for a given Transmit Buffer number

if(ide)
	ecan2msgBuf[buf][0] = (word0 | 0x0002);
else
	ecan2msgBuf[buf][0] = word0;

	ecan2msgBuf[buf][1] = word1;
	ecan2msgBuf[buf][2] = word2;
}


/* ECAN Transmit Data

Inputs :
buf -> Transmit Buffer Number

dataLength -> Length of Data in Bytes to be transmitted

data1/data2/data3/data4 ->  Transmit Data Bytes

*/
/*
void ecan2WriteTxMsgBufData(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4){

	ecan2msgBuf[buf][2] = ((ecan2msgBuf[buf][2] & 0xFFF0) + dataLength) ;

	ecan2msgBuf[buf][3] = data1;
	ecan2msgBuf[buf][4] = data2;
	ecan2msgBuf[buf][5] = data3;
	ecan2msgBuf[buf][6] = data4;

}
*/



void ecan2WriteTxMsgBufData(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4){

	ecan2msgBuf[buf][2] = ((ecan2msgBuf[buf][2] & 0xFFF0) + dataLength) ;


		// Inverting the byte because in CAN data are BIG ENDIAN
		ecan2msgBuf[buf][3] = ((data1 & 0x00FF) << 8) | ((data1 & 0xFF00) >> 8);
		ecan2msgBuf[buf][4] = ((data2 & 0x00FF) << 8) | ((data2 & 0xFF00) >> 8);
		ecan2msgBuf[buf][5] = ((data3 & 0x00FF) << 8) | ((data3 & 0xFF00) >> 8);
		ecan2msgBuf[buf][6] = ((data4 & 0x00FF) << 8) | ((data4 & 0xFF00) >> 8);

}

void ecan2WriteTxMsgBufData_Bit(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned  int data3, unsigned  int data4, unsigned  int data5, unsigned  int data6, unsigned  int data7, unsigned  int data8)
{

	ecan2msgBuf[buf][2] = ((ecan2msgBuf[buf][2] & 0xFFF0) + dataLength) ;

        ecan2msgBuf[buf][3] = (data2 << 8) | (data1);
        ecan2msgBuf[buf][4] = (data4 << 8) | (data3);
        ecan2msgBuf[buf][5] = (data6 << 8) | (data5);
        ecan2msgBuf[buf][6] = (data8 << 8) | (data7);

}


//------------------------------------------------------------------------------
// Disable RX Acceptance Filter
// void ecan1DisableRXFilter(int n)
//------------------------------------------------------------------------------
/*
n -> Filter number [0-15]
*/

void ecan2DisableRXFilter(int n)
{
unsigned int *fltEnRegAddr;
   C2CTRL1bits.WIN=1;
   fltEnRegAddr = (unsigned int *)(&C2FEN1);
  *fltEnRegAddr = (*fltEnRegAddr) & (0xFFFF - (0x1 << n));
   C2CTRL1bits.WIN=0;

}

void clearRxFlags2(unsigned char buffer_number)
{
	if((C2RXFUL1bits.RXFUL2) && (buffer_number==2))
		/* clear flag */
		C2RXFUL1bits.RXFUL2=0;
	/* check to see if buffer is full */
	else if((C2RXFUL1bits.RXFUL3) && (buffer_number==3))
		/* clear flag */
		C2RXFUL1bits.RXFUL3=0;
	else;

}
