
#ifndef CAN_H
#define CAN_H

#define CAN_MSG_DATA	0x01 // message type
#define CAN_MSG_RTR     0x02 // data or RTR
#define CAN_FRAME_EXT	0x03 // Frame type
#define CAN_FRAME_STD	0x04 // extended or standard
#define CAN_BUF_FULL    1
#define CAN_BUF_EMPTY   0


/* message structure in RAM */
typedef struct{
	/* keep track of the buffer status */
	unsigned int buffer_status;
	/* RTR message or data message */
	unsigned char message_type;
	/* frame type extended or standard */
	unsigned char frame_type;
	/* buffer being used to reference the message */
	unsigned char buffer;
	/* 29 bit id max of 0x1FFF FFFF
	*  11 bit id max of 0x7FF */
	unsigned long id;
	/* message data */
	char data[8];
	/* received message data length */
	unsigned char data_length;
}mID;

// Prototype Declaration
void InitCAN(void);
void ecan1WriteMessage(void);
void ecan2WriteMessage(void);
void CANSendPacket1(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);
void CANSendPacket1_Bit(unsigned int id, unsigned int dlc, unsigned short int data1, unsigned short int data2, unsigned short int data3, unsigned short int data4, unsigned short int data5, unsigned short int data6, unsigned short int data7, unsigned short int data8);
void CANProcess1(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);
void CANProcessB(unsigned int id, unsigned int byte0, unsigned int byte1, unsigned int byte2, unsigned int byte3, unsigned int byte4, unsigned int byte5, unsigned int byte6, unsigned int byte7);
void CANSendPacket2(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);
void CANSendPacket2_Bit(unsigned int id, unsigned int dlc, unsigned short int data1, unsigned short int data2, unsigned short int data3, unsigned short int data4, unsigned short int data5, unsigned short int data6, unsigned short int data7, unsigned short int data8);
void CANProcess2(unsigned int id, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);

void rxECAN1(mID *message);
void rxECAN2(mID *message);

#endif
