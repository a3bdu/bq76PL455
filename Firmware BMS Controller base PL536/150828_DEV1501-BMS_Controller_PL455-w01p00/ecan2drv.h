/* 
 * File:   ecan2drv.h
 * Author: Carlo.Campanale
 *
 * Created on 17 febbraio 2015, 11.27
 */

#ifndef __ECAN2_DRV_H__
#define __ECAN2_DRV_H__ 

extern void ecan2WriteRxAcptFilter(int n, long identifier, unsigned int exide,unsigned int bufPnt,unsigned int maskSel);
extern void ecan2WriteRxAcptMask(int m, long identifierMask, unsigned int mide,unsigned int exide);

extern void ecan2WriteTxMsgBufId(unsigned int buf, long txIdentifier, unsigned int ide, unsigned int remoteTransmit);
extern void ecan2WriteTxMsgBufData(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);
extern void ecan2WriteTxMsgBufData_Bit(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned  int data3, unsigned  int data4, unsigned  int data5, unsigned  int data6, unsigned  int data7, unsigned  int data8);

extern void ecan2DisableRXFilter(int n);
extern void clearRxFlags2(unsigned char buffer_number);

#endif
