/*********************************************************************
* File:        can.c
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r00
*
* Created on 7 Marzo 2014, 15.35
*********************************************************************/

#include "p33fxxxx.h"
#include "can.h"
#include "ecan1config.h"
#include "ecan2config.h"
#include "common.h"
#include "EEPROM_Emulation_16bit.h"
#include "KESM_Parameters.h"



// Define ECAN Message Buffers
ECAN1MSGBUF ecan1msgBuf __attribute__((space(dma),aligned(ECAN1_MSG_BUF_LENGTH*16)));
ECAN2MSGBUF ecan2msgBuf __attribute__((space(dma),aligned(ECAN2_MSG_BUF_LENGTH*16)));

// CAN Messages in RAM
mID canRx1Message;
mID canRx2Message;


void InitCAN(void){

    // Init ECAN1
    ecan1Init();
    dma1init();	// for ECAN1 Transmit
    dma2init(); // for ECAN1 Receive
    IEC2bits.C1IE = 1;
    C1INTEbits.TBIE = 0;
    C1INTEbits.RBIE = 1;

    // Init ECAN2
    ecan2Init();
    dma6init();	// for ECAN2 Transmit
    dma7init(); // for ECAN2 Receive
    IEC3bits.C2IE = 1;
    C2INTEbits.TBIE = 0;
    C2INTEbits.RBIE = 1;


 }


void CANProcess1(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4)
{


	
}

void CANProcess2(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4)
{

}



void CANProcessB(unsigned int id, unsigned int byte0, unsigned int byte1, unsigned int byte2, unsigned int byte3, unsigned int byte4, unsigned int byte5, unsigned int byte6, unsigned int byte7)
{



}

void CANSendPacket1(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4)
{


    // buf=0, standard id, normal message
    ecan1WriteTxMsgBufId(0,id,0,0);
    // buf=0, length=8 byte
    ecan1WriteTxMsgBufData(0,8,data1,data2,data3,data4);
    // send buffer 0
    C1TR01CONbits.TXREQ0=1;



}

void CANSendPacket1_Bit(unsigned int id, unsigned int dlc, unsigned short int data1, unsigned short int data2, unsigned short int data3, unsigned short int data4, unsigned short int data5, unsigned short int data6, unsigned short int data7, unsigned short int data8)
{


    // buf=0, standard id, normal message
    ecan1WriteTxMsgBufId(0,id,0,0);
    // buf=0, length=8 byte
    ecan1WriteTxMsgBufData_Bit(0,dlc, data1,data2,data3,data4,data5,data6,data7,data8);
    // send buffer 0
    C1TR01CONbits.TXREQ0=1;



}

void CANSendPacket2(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4)
{


    // buf=0, standard id, normal message
    ecan2WriteTxMsgBufId(0,id,0,0);
    // buf=0, length=8 byte
    ecan2WriteTxMsgBufData(0,8,data1,data2,data3,data4);
    // send buffer 0
    C2TR01CONbits.TXREQ0=1;



}

void CANSendPacket2_Bit(unsigned int id, unsigned int dlc, unsigned short int data1, unsigned short int data2, unsigned short int data3, unsigned short int data4, unsigned short int data5, unsigned short int data6, unsigned short int data7, unsigned short int data8)
{


    // buf=0, standard id, normal message
    ecan2WriteTxMsgBufId(0,id,0,0);
    // buf=0, length=8 byte
    ecan2WriteTxMsgBufData_Bit(0,dlc, data1,data2,data3,data4,data5,data6,data7,data8);
    // send buffer 0
    C2TR01CONbits.TXREQ0=1;



}



void rxECAN1(mID *message)
{
    unsigned int ide=0, rtr=0, buffer;
    unsigned long id=0;

    unsigned int data1, data2, data3, data4;
    unsigned int byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7;

    /* Standard Message Format: */
    /* Word0 : 0bUUUx xxxx xxxx xxxx */
    /* |____________||| */
    /* SID10:0 SRR IDE (bit 0) */
    /* Word1 : 0bUUUU xxxx xxxx xxxx */
    /* |_____________| */
    /* EID17:6 */
    /* Word2 : 0bxxxx xxx0 UUU0 xxxx */
    /* |_____|| |__| */
    /* EID5:0 RTR DLC */
    /* Word3-Word6: Data bytes */
    /* Word7: Filter hit code bits */
    /* Remote Transmission Request Bit for standard frames */
    /* SRR-> "0 " Normal Message */
    /* "1" Message will request remote transmission */
    /* Substitute Remote Request Bit for extended frames */
    /* SRR-> Should always be set to ‘1’ as per CAN specification */
    /* Extended Identifier Bit */
    /* IDE-> "0" Message will transmit standard identifier */
    /* "1" Message will transmit extended identifier */
    /* Remote Transmission Request Bit for extended frames */
    /* RTR-> "0" Message transmitted is a normal message */
    //	"1" Message transmitted is a remote message
    /* Don't care for standard frames */
    /* read word 0 to see the message type */

    ide=ecan1msgBuf[message->buffer][0] & 0x0001;


    /* check to see what type of message it is */
    /* message is standard identifier */
    if(ide==0)
            {
            message->id=(ecan1msgBuf[message->buffer][0] & 0x1FFC) >> 2;
            message->frame_type=CAN_FRAME_STD;
            rtr=ecan1msgBuf[message->buffer][0] & 0x0002;
    }
    /* mesage is extended identifier */
    else
    {
            id=ecan1msgBuf[message->buffer][0] & 0x1FFC;
            message->id=id << 16;
            id=ecan1msgBuf[message->buffer][1] & 0x0FFF;
            message->id=message->id+(id << 6);
            id=(ecan1msgBuf[message->buffer][2] & 0xFC00) >> 10;
            message->id=message->id+id;
            message->frame_type=CAN_FRAME_EXT;
            rtr=ecan1msgBuf[message->buffer][2] & 0x0200;
    }
    /* check to see what type of message it is */
    /* RTR message */
    if(rtr==1)
    {
    message->message_type=CAN_MSG_RTR;
    }
    /* normal message */
    else
    {

            message->message_type=CAN_MSG_DATA;
            message->data[0]=(unsigned char)ecan1msgBuf[message->buffer][3];
            message->data[1]=(unsigned char)((ecan1msgBuf[message->buffer][3] & 0xFF00) >> 8);
            message->data[2]=(unsigned char)ecan1msgBuf[message->buffer][4];
            message->data[3]=(unsigned char)((ecan1msgBuf[message->buffer][4] & 0xFF00) >> 8);
            message->data[4]=(unsigned char)ecan1msgBuf[message->buffer][5];
            message->data[5]=(unsigned char)((ecan1msgBuf[message->buffer][5] & 0xFF00) >> 8);
            message->data[6]=(unsigned char)ecan1msgBuf[message->buffer][6];
            message->data[7]=(unsigned char)((ecan1msgBuf[message->buffer][6] & 0xFF00) >> 8);
            message->data_length=(unsigned char)(ecan1msgBuf[message->buffer][2] & 0x000F);


    /*
            // Inverting the byte because in CAN there are BIG ENDIAN data
            message->message_type=CAN_MSG_DATA;
            data1 = message->data[0]=(ecan1msgBuf[message->buffer][3] & 0x00FF) << 8;
            data1 = data1 | (message->data[1]=((ecan1msgBuf[message->buffer][3] & 0xFF00) >> 8));
            data2 = message->data[2]=(ecan1msgBuf[message->buffer][4] & 0x00FF) << 8;
            data2 = data2 | (message->data[3]=((ecan1msgBuf[message->buffer][4] & 0xFF00) >> 8));
            data3 = message->data[4]=(ecan1msgBuf[message->buffer][5] & 0x00FF) << 8;
            data3 = data3 | (message->data[5]=((ecan1msgBuf[message->buffer][5] & 0xFF00) >> 8));
            data4 = message->data[6]=(ecan1msgBuf[message->buffer][6] & 0x00FF) << 8;
            data4 = data4 | (message->data[7]=((ecan1msgBuf[message->buffer][6] & 0xFF00) >> 8));
            message->data_length=(ecan1msgBuf[message->buffer][2] & 0x000F);
    */
    /*
            data1 = ecan1msgBuf[message->buffer][3];
            data2 = ecan1msgBuf[message->buffer][4];
            data3 = ecan1msgBuf[message->buffer][5];
            data4 = ecan1msgBuf[message->buffer][6];
    */
            buffer = message->buffer;

            // Inverting the byte because in CAN there are BIG ENDIAN data
            data1 = (ecan1msgBuf[buffer][3] & 0x00FF) << 8;
            data1 = (data1 | ((ecan1msgBuf[buffer][3] & 0xFF00) >> 8));
            data2 = (ecan1msgBuf[buffer][4] & 0x00FF) << 8;
            data2 = (data2 | ((ecan1msgBuf[buffer][4] & 0xFF00) >> 8));
            data3 = (ecan1msgBuf[buffer][5] & 0x00FF) << 8;
            data3 = (data3 | ((ecan1msgBuf[buffer][5] & 0xFF00) >> 8));
            data4 = (ecan1msgBuf[buffer][6] & 0x00FF) << 8;
            data4 = (data4 | ((ecan1msgBuf[buffer][6] & 0xFF00) >> 8));

            byte0 = message->data[0];
            byte1 = message->data[1];
            byte2 = message->data[2];
            byte3 = message->data[3];
            byte4 = message->data[4];
            byte5 = message->data[5];
            byte6 = message->data[6];
            byte7 = message->data[7];

    }

            id=message->id;



            CANProcess1(id, data1, data2, data3, data4);

            CANProcessB(id, byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7);

            clearRxFlags1(message->buffer);


}

void rxECAN2(mID *message)
{
unsigned int ide=0, rtr=0, buffer;
	unsigned long id=0;

	unsigned int data1, data2, data3, data4;
	unsigned int byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7;

	/* Standard Message Format: */
	/* Word0 : 0bUUUx xxxx xxxx xxxx */
	/* |____________||| */
	/* SID10:0 SRR IDE (bit 0) */
	/* Word1 : 0bUUUU xxxx xxxx xxxx */
	/* |_____________| */
	/* EID17:6 */
	/* Word2 : 0bxxxx xxx0 UUU0 xxxx */
	/* |_____|| |__| */
	/* EID5:0 RTR DLC */
	/* Word3-Word6: Data bytes */
	/* Word7: Filter hit code bits */
	/* Remote Transmission Request Bit for standard frames */
	/* SRR-> "0 " Normal Message */
	/* "1" Message will request remote transmission */
	/* Substitute Remote Request Bit for extended frames */
	/* SRR-> Should always be set to ?1? as per CAN specification */
	/* Extended Identifier Bit */
	/* IDE-> "0" Message will transmit standard identifier */
	/* "1" Message will transmit extended identifier */
	/* Remote Transmission Request Bit for extended frames */
	/* RTR-> "0" Message transmitted is a normal message */
	/* "1" Message transmitted is a remote message */
	/* Don't care for standard frames */
	/* read word 0 to see the message type */

	ide=ecan2msgBuf[message->buffer][0] & 0x0001;


	/* check to see what type of message it is */
	/* message is standard identifier */
	if(ide==0)
		{
		message->id=(ecan2msgBuf[message->buffer][0] & 0x1FFC) >> 2;
		message->frame_type=CAN_FRAME_STD;
		rtr=ecan2msgBuf[message->buffer][0] & 0x0002;
	}
	/* mesage is extended identifier */
	else
	{
		id=ecan2msgBuf[message->buffer][0] & 0x1FFC;
		message->id=id << 16;
		id=ecan2msgBuf[message->buffer][1] & 0x0FFF;
		message->id=message->id+(id << 6);
		id=(ecan2msgBuf[message->buffer][2] & 0xFC00) >> 10;
		message->id=message->id+id;
		message->frame_type=CAN_FRAME_EXT;
		rtr=ecan2msgBuf[message->buffer][2] & 0x0200;
	}
	/* check to see what type of message it is */
	/* RTR message */
	if(rtr==1)
	{
	message->message_type=CAN_MSG_RTR;
	}

	/* normal message */
	else
	{

		message->message_type=CAN_MSG_DATA;
		message->data[0]=(unsigned char)ecan2msgBuf[message->buffer][3];
		message->data[1]=(unsigned char)((ecan2msgBuf[message->buffer][3] & 0xFF00) >> 8);
		message->data[2]=(unsigned char)ecan2msgBuf[message->buffer][4];
		message->data[3]=(unsigned char)((ecan2msgBuf[message->buffer][4] & 0xFF00) >> 8);
		message->data[4]=(unsigned char)ecan2msgBuf[message->buffer][5];
		message->data[5]=(unsigned char)((ecan2msgBuf[message->buffer][5] & 0xFF00) >> 8);
		message->data[6]=(unsigned char)ecan2msgBuf[message->buffer][6];
		message->data[7]=(unsigned char)((ecan2msgBuf[message->buffer][6] & 0xFF00) >> 8);
		message->data_length=(unsigned char)(ecan2msgBuf[message->buffer][2] & 0x000F);

/*
		// Inverting the byte because in CAN there are BIG ENDIAN data
		message->message_type=CAN_MSG_DATA;
		data1 = message->data[0]=(ecan2msgBuf[message->buffer][3] & 0x00FF) << 8;
		data1 = data1 | (message->data[1]=((ecan2msgBuf[message->buffer][3] & 0xFF00) >> 8));
		data2 = message->data[2]=(ecan2msgBuf[message->buffer][4] & 0x00FF) << 8;
		data2 = data2 | (message->data[3]=((ecan2msgBuf[message->buffer][4] & 0xFF00) >> 8));
		data3 = message->data[4]=(ecan2msgBuf[message->buffer][5] & 0x00FF) << 8;
		data3 = data3 | (message->data[5]=((ecan2msgBuf[message->buffer][5] & 0xFF00) >> 8));
		data4 = message->data[6]=(ecan2msgBuf[message->buffer][6] & 0x00FF) << 8;
		data4 = data4 | (message->data[7]=((ecan2msgBuf[message->buffer][6] & 0xFF00) >> 8));
		message->data_length=(ecan2msgBuf[message->buffer][2] & 0x000F);
*/
/*
		data1 = ecan2msgBuf[message->buffer][3];
		data2 = ecan2msgBuf[message->buffer][4];
		data3 = ecan2msgBuf[message->buffer][5];
		data4 = ecan2msgBuf[message->buffer][6];
*/
		buffer = message->buffer;

		// Inverting the byte because in CAN there are BIG ENDIAN data
		data1 = (ecan2msgBuf[buffer][3] & 0x00FF) << 8;
		data1 = (data1 | ((ecan2msgBuf[buffer][3] & 0xFF00) >> 8));
		data2 = (ecan2msgBuf[buffer][4] & 0x00FF) << 8;
		data2 = (data2 | ((ecan2msgBuf[buffer][4] & 0xFF00) >> 8));
		data3 = (ecan2msgBuf[buffer][5] & 0x00FF) << 8;
		data3 = (data3 | ((ecan2msgBuf[buffer][5] & 0xFF00) >> 8));
		data4 = (ecan2msgBuf[buffer][6] & 0x00FF) << 8;
		data4 = (data4 | ((ecan2msgBuf[buffer][6] & 0xFF00) >> 8));


		byte0 = message->data[0];
		byte1 = message->data[1];
		byte2 = message->data[2];
		byte3 = message->data[3];
		byte4 = message->data[4];
		byte5 = message->data[5];
		byte6 = message->data[6];
		byte7 = message->data[7];
	}

		id=message->id;

		CANProcess2(id, data1, data2, data3, data4);
		if(id==0x210)
			id=0x310;

		if(id==0x22c)
			id=0x32c;

		CANProcessB(id, byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7);
		clearRxFlags2(message->buffer);

}





// Interrupt Service Routine 1
/* No fast context save, and no variables stacked */
void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void)
{

    /* check to see if the interrupt is caused by receive */
    if(C1INTFbits.RBIF)
    {

            /* check to see if buffer 2 is full */
            if(C1RXFUL1bits.RXFUL2)
            {
                    /* set the buffer full flag and the buffer received flag */
                    canRx1Message.buffer_status=CAN_BUF_FULL;
                    canRx1Message.buffer=2;
            }
            /* check to see if buffer 3 is full */
            else if(C1RXFUL1bits.RXFUL3)
            {
                    /* set the buffer full flag and the buffer received flag */
                    canRx1Message.buffer_status=CAN_BUF_FULL;
                    canRx1Message.buffer=3;
            }
            else;
            /* clear flag */
            C1INTFbits.RBIF = 0;
            rxECAN1(&canRx1Message);
    }
    else if(C1INTFbits.TBIF)
    {
    //	puts_ecanTc(&canTx1Message);
            /* clear flag */
            C1INTFbits.TBIF = 0;
    }
    else;
            /* clear interrupt flag */
            IFS2bits.C1IF = 0;

}

void __attribute__((interrupt, no_auto_psv))_C2Interrupt(void)
{
	/* check to see if the interrupt is caused by receive */
	if(C2INTFbits.RBIF)
	{
		/* check to see if buffer is full */
		if(C2RXFUL1bits.RXFUL2)
		{
			/* set the buffer full flag and the buffer received flag */
			canRx2Message.buffer_status=CAN_BUF_FULL;
			canRx2Message.buffer=2;
		}
		/* check to see if buffer is full */
		else if(C2RXFUL1bits.RXFUL3)
		{
			/* set the buffer full flag and the buffer received flag */
			canRx2Message.buffer_status=CAN_BUF_FULL;
			canRx2Message.buffer=3;
		}
		else;
			/* clear flag */
			C2INTFbits.RBIF = 0;
			rxECAN2(&canRx2Message);
	}
	else if(C2INTFbits.TBIF)
	{
	//	puts_ecanTc(&canTxMessage);
		/* clear flag */
		C2INTFbits.TBIF = 0;
	}
	else;
		/* clear interrupt flag */
		IFS3bits.C2IF = 0;
}





//------------------------------------------------------------------------------
//    DMA interrupt handlers
//------------------------------------------------------------------------------

void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)
{
   IFS0bits.DMA1IF = 0;          // Clear the DMA1 Interrupt Flag;
}

void __attribute__((interrupt, no_auto_psv)) _DMA2Interrupt(void)
{
   IFS1bits.DMA2IF = 0;          // Clear the DMA2 Interrupt Flag;
}

void __attribute__((interrupt, no_auto_psv)) _DMA6Interrupt(void)
{
   IFS4bits.DMA6IF = 0;          // Clear the DMA3 Interrupt Flag;
}

void __attribute__((interrupt, no_auto_psv)) _DMA7Interrupt(void)
{
   IFS4bits.DMA7IF = 0;          // Clear the DMA4 Interrupt Flag;
}
