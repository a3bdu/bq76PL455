#ifndef __ECAN1_DRV_H__
#define __ECAN1_DRV_H__

extern void ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide,unsigned int bufPnt,unsigned int maskSel);
extern void ecan1WriteRxAcptMask(int m, long identifierMask, unsigned int mide,unsigned int exide);

extern void ecan1WriteTxMsgBufId(unsigned int buf, long txIdentifier, unsigned int ide, unsigned int remoteTransmit);
extern void ecan1WriteTxMsgBufData(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);
extern void ecan1WriteTxMsgBufData_Bit(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned  int data3, unsigned  int data4, unsigned  int data5, unsigned  int data6, unsigned  int data7, unsigned  int data8);

extern void ecan1DisableRXFilter(int n);
extern void clearRxFlags1(unsigned char buffer_number);

#endif
