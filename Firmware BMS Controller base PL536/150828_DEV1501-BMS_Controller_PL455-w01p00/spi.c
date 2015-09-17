/*********************************************************************
* File:        spi.c
* Author:      Carlo Menegazoz, Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r00
*
* Created on 3 Marzo 2014, 15.35
* Modified on 14 September 2015
*********************************************************************/

#include "p33fxxxx.h"
#include "common.h"
#include "spi.h"
#include "timer.h"
#include <libpic30.h>                  /* For possible use with C function Call block (delay_ms or delay_us functions might be used by few peripherals) */
#include <libq.h>
#include "can.h"
#include <math.h>


#define REGISTER_ADDRESSING ADDR_SIZE_8bit

/**
 * \var int StatoSPI
 * \brief This variable manages the operation that has to been taken into the proper interrupt. Every state sets the SPI in the correct operational mode.
 */
int StatoSPI=0;
int cicli_init=0, cicli_dma=0;
unsigned int SPI1_Buff16[length] __attribute__((space(dma)));           //DMA SPI 5 for TX
unsigned int data_rx[length] __attribute__((space(dma)));               //DMA SPI 4 for RX
/**
 * \var unsigned int data_req[14] __attribute__((space(dma)));
 * \brief This array collect the message that has to be send to the SPI peripheral:
 * \Description        Read the protocol specifics about the frame response
 */
unsigned int data_req[18] __attribute__((space(dma)));
DevData Dev[MAX_DEVICE];
unsigned int VettoreInit[250];
int read_size;
int write_size;
unsigned int  Gcrc8=0;
unsigned int SPI_WRITING=0;




/**
 * \fn void InitSPI(void)
 * \brief Description:SPI Initialization. Parameter definitions in dsPIC33J registers. Principal characteristics are: 75kbps ; SPI-2
 */
void InitSPI(void)
{

      // Set-up SPI 2 peripheral
      SPI2CON1 = 0x220; //0x220 -> 75kbit/s 0b0001000100000
      SPI2CON2 = 0x00;
      SPI2STAT = 0x8000;

      // Configure DMA for SPI Rx
      DMA4CON = 0x0001;
      DMA4REQ = 0x21;
      DMA4PAD = (volatile unsigned int) &SPI2BUF;
      IPC11bits.DMA4IP= 4;    // DMA4 Interrupt priority level=4
      _SPI2IE = 0;
      _DMA4IF = 0;
      _DMA4IE = 1;

      // Configure DMA for SPI Tx
      DMA5CON = 0x2001;
      DMA5REQ = 0x21;
      DMA5PAD = (volatile unsigned int) &SPI2BUF;
      IPC15bits.DMA5IP= 3;    // DMA4 Interrupt priority level=3
      _SPI2IE = 0;
      _DMA5IF = 0;
      _DMA5IE = 0;

}

/**
 * \fn void write_to_Device(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned char *data)
 * \brief Composition of the message. Not for broadcast messages.
 * @param ADDR_SIZE     ADDR_SIZE_8bit                          0
 *                      ADDR_SIZE_16bit                         1
 * @param len           Data size
 * @param dev_addr      device address.
 * @param reg_addr      register address of the device
 * @param data          pointer to the data
 * @return              pointer of the message, the total size is length+ (dev_addr) + (reg_addr) + crc = length+4
 */
void write_to_Device(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned int* data)//DISABILITATO L'INTERRUPT
{
    SPI_WRITING = 1;
    StatoSPI=1;//STATO: programmazione/scrittura

    unsigned int *temp_buf;
    int i;

    temp_buf=write_to_Dev(REQ_TYPE_SINGLE_DEV_WRITE_NO_RESPONSE,ADDR_SIZE,len,dev_addr,reg_addr,data);
    write_size = 3+ADDR_SIZE+len+2;

    for(i=0;i<write_size;i++)
        SPI1_Buff16[i]=*(temp_buf+i);

    for(i=0;i<write_size;i++){
        printf("%x ",SPI1_Buff16[i]);
    }
    printf("\n");
    //_DMA4IF = 1;                       /* Force Interrupt */
}

/**
 * \fn void read_to_Dev(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned char *data)
 * \brief Composition of the message. Not for broadcast messages.
 * @param ADDR_SIZE     ADDR_SIZE_8bit                          0
 *                      ADDR_SIZE_16bit                         1
 * @param len           #of byte it is will to receive
 * @param dev_addr      device address.
 * @param reg_addr      start register address to be red
 * @param data          pointer to the data
 * @return              pointer of the message, the total size is length+ (dev_addr) + (reg_addr) + crc = length+4
 */
void read_from_Device(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned int *data)//DISABILITATO L'INTERRUPT
{
    StatoSPI = 2; //STATO: richiesta info

    unsigned int *temp_buf;
    int i;

    temp_buf=write_to_Dev(REQ_TYPE_SINGLE_DEV_WRITE_RESPONSE,ADDR_SIZE,len,dev_addr,reg_addr,data);
    write_size = 3+ADDR_SIZE+1+2;    //FRAME+ADDR+REG(+1 if 16bits addressing)+data+crcMSB+crcLSB
    read_size = 0;  //it is a do while... before reads after checks the condition. Warn: the real dimension of the answer is in the first byte received and it is between 00 and 0F (the value is (0#+1) )
    for(i=0;i<write_size;i++)
        data_req[i]=*(temp_buf+i);

    for(i=0;i<write_size;i++){
        printf("%x ",data_req[i]);
    }
    printf("\n");

    //_DMA4IF = 1;                       /* Force Interrupt */

}

/**
 * \fn void write_to_Group(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned char *data)
 * \brief Composition of the message. Not for broadcast messages.
 * @param ADDR_SIZE     ADDR_SIZE_8bit                          0
 *                      ADDR_SIZE_16bit                         1
 * @param len           Data size
 * @param dev_addr      device address.
 * @param reg_addr      register address of the device
 * @param data          pointer to the data
 * @return              pointer of the message, the total size is length+ (dev_addr) + (reg_addr) + crc = length+4
 */
void write_to_Group(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned int *data)
{
    SPI_WRITING = 1;
    StatoSPI=1;//STATO: programmazione/scrittura

    unsigned int *temp_buf;
    int i;

    temp_buf=write_to_Gr(REQ_TYPE_GROUP_WRITE_NORESPONSE,ADDR_SIZE,len,dev_addr,reg_addr,data);
    write_size = 3+ADDR_SIZE+len+2;

    for(i=0;i<write_size;i++)
        SPI1_Buff16[i]=*(temp_buf+i);


    _DMA4IF = 1;                       /* Force Interrupt */
}

/**
 * \fn void read_to_Group(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned char *data)
 * \brief Composition of the message. Not for broadcast messages.
 * @param ADDR_SIZE     ADDR_SIZE_8bit                          0
 *                      ADDR_SIZE_16bit                         1
 * @param len           #of byte it is will to receive
 * @param dev_addr      device address.
 * @param reg_addr      start register address to be red
 * @param data          pointer to the data
 * @return              pointer of the message, the total size is (dev_addr) + (reg_addr) + data + crc = 6 or 7
 */
void read_from_Group(unsigned char ADDR_SIZE, unsigned char len, unsigned int dev_addr, unsigned int reg_addr, unsigned int *data)
{
    StatoSPI = 2; //STATO: richiesta info

    unsigned int *temp_buf;
    int i;

    temp_buf=write_to_Gr(REQ_TYPE_GROUP_WRITE_RESPONSE,ADDR_SIZE,len,dev_addr,reg_addr,data);
    write_size = 3+ADDR_SIZE+1+2;    //FRAME+ADDR+REG(+1 if 16bits addressing)+data+crcMSB+crcLSB
    read_size = 0;  //it is a do while... before reads after checks the condition. Warn: the real dimension of the answer is in the first byte received and it is between 00 and 0F (the value is (0#+1) )
    for(i=0;i<write_size;i++)
        data_req[i]=*(temp_buf+i);


    _DMA4IF = 1;                       /* Force Interrupt */

}

/**
 * \fn void write_Broadcast(unsigned char ADDR_SIZE, unsigned char len, unsigned int reg_addr, unsigned char *data)
 * \brief Composition of the message. Not for broadcast messages.
 * @param ADDR_SIZE     ADDR_SIZE_8bit                          0
 *                      ADDR_SIZE_16bit                         1
 * @param len           Data size
 * @param dev_addr      device address.
 * @param reg_addr      register address of the device
 * @param data          pointer to the data
 * @return              pointer of the message, the total size is length+ (dev_addr) + (reg_addr) + crc = length+4
 */
void write_Broadcast(unsigned char ADDR_SIZE, unsigned char len, unsigned int reg_addr, unsigned int *data)//DISABILITATO L'INTERRUPT
{
    SPI_WRITING = 1;
    StatoSPI=1;//STATO: programmazione/scrittura

    unsigned int *temp_buf;
    int i;

    temp_buf=write_to_Broad(REQ_TYPE_BROADCAST_WRITE_NORESPONSE,ADDR_SIZE,len,reg_addr,data);
    write_size = 3+ADDR_SIZE+len+2;

    for(i=0;i<write_size;i++)
        SPI1_Buff16[i]=*(temp_buf+i);

    for(i=0;i<write_size;i++){
        printf("%x ",SPI1_Buff16[i]);
    }
    printf("\n");
    //_DMA4IF = 1;                       /* Force Interrupt */
}

/**
 * \fn void read_Broadcast(unsigned char ADDR_SIZE, unsigned char len, unsigned int reg_addr, unsigned char *data)
 * \brief Composition of the message. Not for broadcast messages.
 * @param ADDR_SIZE     ADDR_SIZE_8bit                          0
 *                      ADDR_SIZE_16bit                         1
 * @param len           #of byte it is will to receive
 * @param dev_addr      device address.
 * @param reg_addr      start register address to be red
 * @param data          pointer to the data
 * @return              pointer of the message, the total size is (dev_addr) + (reg_addr) + data + crc = 6 or 7
 */
void read_Broadcast(unsigned char ADDR_SIZE, unsigned char len,  unsigned int reg_addr, unsigned int *data)//DISABILITATO L'INTERRUPT
{
    StatoSPI = 2; //STATO: richiesta info

    unsigned int *temp_buf;
    int i;

    temp_buf=write_to_Broad(REQ_TYPE_BROADCAST_WRITE_RESPONSE,ADDR_SIZE,len,reg_addr,data);
    write_size = 3+ADDR_SIZE+1+2;    //FRAME+ADDR+REG(+1 if 16bits addressing)+data+crcMSB+crcLSB
    read_size = 0;  //it is a do while... before reads after checks the condition. Warn: the real dimension of the answer is in the first byte received and it is between 00 and 0F (the value is (0#+1) )
    for(i=0;i<write_size;i++)
        data_req[i]=*(temp_buf+i);


   // _DMA4IF = 1;                       /* Force Interrupt */

}

void Check_Dev()
{
    unsigned int i,k;
    unsigned int data=0x01;

    PLUGGED_DEV=0;

//  Write_SPI(0x3f, ADC_CONVERT, 1);

    for(i=0x01; i<=MAX_DEVICE; i++){
        for(k=0; k<6000; k++){
            switch(k){
                case 1000:
                    read_from_Device(REGISTER_ADDRESSING,1,i, ADDR, &data);
                break;
                case 5000:
                    if( (data_rx[0]) > 0 ){
                        if( i - PLUGGED_DEV == 1 )
                            PLUGGED_DEV = i;
                        else
                            i--;
                    }
                break;
            }
        }
    }
    for(i=0x01; i<=PLUGGED_DEV; i++){
         if(Dev[i].CellsNum >= NcellMIN){
             for(k=0; k<20; k++){
                 switch(k){
                     //case 0:
                          //Write_SPI(i, SHDW_CTRL, 0x35);         //Shadow CTRL write     <-- riguarda Group3
                     //break;
                     case 0:
                          write_to_Device(REGISTER_ADDRESSING, 1, i, NCHAN, &Dev[i].CellsNum );   //Number of cells in series     <- define
                     break;
                 }
             }
         }
     }
}

void Clear_Faults_Broadcast()
{
    unsigned int *data;
    *data=0xFFFF;
    write_Broadcast(REGISTER_ADDRESSING,1,FAULT_SUM,data);
}

void Clear_Faults_Device(unsigned Dev_Addr)
{
    unsigned int *data;
    *data=0xFFFFFFFF;
    write_to_Device(REGISTER_ADDRESSING,4,Dev_Addr,FAULT_SUM,data);
}

/**
 * \fn void init_Ti(unsigned int DevAdr, unsigned int RegAdr, unsigned int Data)
 * \brief Description:Initialization of a TI device.
 * @param  DevAdr   Addressing of the initialization procedure
 * @param  RegAdr   Register selection in the device bq76PL455A-Q1
 * @param  Data     Data value
 */
void init_Ti(unsigned int DevAdr, unsigned int RegAdr, unsigned int Data)
{

    unsigned int *temp_buf;
    int i;

    //Messagge only for a single device
    if(DevAdr<0xFF){
        temp_buf=write_to_Dev(REQ_TYPE_SINGLE_DEV_WRITE_NO_RESPONSE,REGISTER_ADDRESSING,1,DevAdr,RegAdr,&Data);
    
        for(i=0;i<(6+REGISTER_ADDRESSING);i++){
            VettoreInit[cicli_init+i]=*(temp_buf+i);
        }
        StatoSPI = 3;
        cicli_init+=(6+REGISTER_ADDRESSING);
        
    }
    else{ //Broadcast message
        temp_buf=write_to_Broad(REQ_TYPE_BROADCAST_WRITE_NORESPONSE,REGISTER_ADDRESSING,1,RegAdr,&Data);
        
        for(i=0;i<(5+REGISTER_ADDRESSING);i++){
            VettoreInit[cicli_init+i]=*(temp_buf+i);
        }
        StatoSPI = 3;
        cicli_init+=(5+REGISTER_ADDRESSING);
    }

    for(i=0;i<(5+REGISTER_ADDRESSING);i++){
        printf("%x ",VettoreInit[cicli_init+i]);
    }
    printf("\n");
}

void Reset_SPI()
{
    int i;
    unsigned int cl;
    cicli_init = 0;

    for(i=0; i<700; i++){
        switch(i){
            case 100:
                init_Ti(0xFF, DEV_CTRL, 0x80);          //Reset All Devices
                 for (cl=0; cl<20000; cl++);
            break;
            case 200:
                //Write_SPI(0x3f, IO_CONTROL, 0x43);     ---> Not available
                 for (cl=0; cl<20000; cl++);
            break;
            case 300:
                Init_DevicesTi();
                 for (cl=0; cl<20000; cl++);
            break;
            case 500:
                Check_Dev();
                for (cl=0; cl<20000; cl++);
            break;
        }
    }
}

//0xFF broadcast address
void Init_DevicesTi()//DISABILITATO L'INTERRUPT
{
    init_Ti(0xFF, DEV_CTRL, 0x80);          //Reset All Devices
    init_Ti(0xFF, DEV_CTRL, 0x04);          //Set auto adressing
    init_Ti(0xFF, DEV_CTRL, 0x80);          //Reset All Devices
    init_Ti(0xFF, DEV_CTRL, 0x04);          //Set auto adressing
    init_Ti(0xFF, DEVCONFIG, 0b00011001);      //Indicates source of ALERT signal: AR=1; PARITY=NAN ECC_ERR=0 FORCE=0 TSD=0 SLEEP=0 OT2=0 OT1=0

    init_Ti(0xFF, AM_CHAN, 0x00);
    init_Ti(0xFF, AM_CHAN, 0b10000100);


    init_Ti(0xFF, CHANNELS,0);      //set of minimum number of cells:6
    init_Ti(0xFF, CHANNELS+1,0b00111111);

    init_Ti(0xFF, CHANNELS+2,0x00);    //set no AUX temperature sensors, set both internal sensors
    init_Ti(0xFF, CHANNELS+3,0b11000000);

    

    init_Ti(0xFF, CBCONFIG , 0x18);           //Configures the CB control FETs maximum on time : 4s


     init_Ti(0xFF, CELL_OV, (((unsigned int) ((Over_V - 2000)*0.02))>>6) );        //Overvoltage set point : 3,6V || 0x00 = 2 V; each binary increment adds 50 mV until 0x3c = 5 V.
     init_Ti(0xFF, CELL_OV+1, (((unsigned int) ((Over_V - 2000)*0.02))<<2) );        //Overvoltage set point : 3,6V || 0x00 = 2 V; each binary increment adds 50 mV until 0x3c = 5 V.


    init_Ti(0xFF, CELL_UV, (((unsigned int) ((Under_V - 700)*0.01))>>6) );        //Undervoltage setpoint : 2,5V || 0x00 = 0.7 V; each binary increment adds 100 mV until 0x1a = 3.3 V.
    init_Ti(0xFF, CELL_UV+1, (((unsigned int) ((Under_V - 700)*0.01))<<2) );        //Undervoltage setpoint : 2,5V || 0x00 = 0.7 V; each binary increment adds 100 mV until 0x1a = 3.3 V.

    init_Ti(0xFF, STATUS, 0xFF);


    StatoSPI = 3;
    //_DMA4IF = 1;

}
     
void Wake_up_SET()
{

    /*+++++++++++++++++++++++++++++++++ SLEEP START ++++++++++++++++++++++++++++++++++++*/
    unsigned int i=0;
    unsigned int data=0x20;
    SLEEP = 1;
    for(i=0; i<60000; i++){
        if(i==1000)
            data=0x20;
            write_Broadcast(REGISTER_ADDRESSING,1, DEV_CTRL, &data);  //SLEEP Broadcast SET

        if(i==60000)
             write_Broadcast(REGISTER_ADDRESSING,1, DEV_CTRL, &data);   //SLEEP Broadcast SET

    }

    /*----------------------------------- SLEEP END -----------------------------------*/
}

void Sleep_SET()
{
    /*+++++++++++++++++++++++++++++++++ SLEEP START ++++++++++++++++++++++++++++++++++++*/
      unsigned int i=0;
      unsigned int data=0x40;
    SLEEP = 1;
    for(i=0; i<60000; i++){
        if(i==1000)
             write_Broadcast(REGISTER_ADDRESSING,1, DEV_CTRL, &data);  //SLEEP Broadcast SET

        if(i==60000)
              write_Broadcast(REGISTER_ADDRESSING,1, DEV_CTRL, &data);  //SLEEP Broadcast SET
        
    }

    /*----------------------------------- SLEEP END -----------------------------------*/
 }

int Read_SPI( int DevAdr, int RegAdr, int Size )
{
     unsigned int crctemp=0,z,i,k;
     unsigned int crc_Rx, CRC_fault;
     long int V_temp[32];   //NCellMAX=16
     long double T_temp[16];    //NTemperatureMAX=8



     crc_Rx = data_rx[Size-2];

     if(checkCRC(crc_Rx,&data_rx,Size-1)){
         Dev[DevAdr].id=DevAdr;
         if(RegAdr == CHANNELS){


             //+++++++++++++ VOLTAGE +++++++++++++++++++++++++//
             
             Dev[DevAdr].Vtot = 0;
             Dev[DevAdr].CellsNum = 0;
             for (i=0; i<NcellMAX; i++){
                Dev[DevAdr].Vcell[i]=V_temp[i+2];

                Dev[DevAdr].Vtot += (Dev[DevAdr].Vcell[i] / 100);

                if(Dev[DevAdr].Vcell[i] > 500 ){
                    if(Dev[DevAdr].Vmin > Dev[DevAdr].Vcell[i]){
                        Dev[DevAdr].Vmin = Dev[DevAdr].Vcell[i];
                        Dev[DevAdr].Lowest = i;
                    }
                    if(Dev[DevAdr].Vmax < Dev[DevAdr].Vcell[i]){
                        Dev[DevAdr].Vmax = Dev[DevAdr].Vcell[i];
                        Dev[DevAdr].Highest = i;
                    }
                    Dev[DevAdr].CellsNum++;
                }
                else
                    Dev[DevAdr].Vcell[i] = 0;
             }

             /**********TEMP*******************/
//             for (i=0; i<12; i++){
//                 V_temp[i] = data_rx[i+2]*0x100 + data_rx[i+3];
//                 V_temp[i] = (V_temp[i] * 0x186a) / 0x3fff;
//             }
         }
     }else{
         Gcrc8 = 0;
         return CRC_fault = 1;
     }
      
}



// int Read_SPI( int DevAdr,   int RegAdr,   int Size ){
//     unsigned int crctemp=0,z,i,k;
//     unsigned int crc_Rx, CRC_fault;
//     long int V_temp[12];
//     long double T_temp[6];
//
//     for (z = 0; z < Size; z++){
//        crctemp = Gcrc8 ^ data_rx[z];
//        Gcrc8 = CrcTable[crctemp];
//     }
//
//     crc_Rx = data_rx[Size];
//
//     if(Gcrc8 == crc_Rx){
//         Dev[DevAdr].id=DevAdr;
//         if(RegAdr == DEVICE_STATUS){
//             Dev[DevAdr].ReadCnt++;
//             Dev[DevAdr].Status=data_rx[0];
//             Dev[DevAdr].Gpai[0]=data_rx[1];
//             Dev[DevAdr].Gpai[1]=data_rx[2];
//
//             //+++++++++++++ VOLTAGE +++++++++++++++++++++++++//
//             for (i=0; i<12; i+=2){
//                 V_temp[i] = data_rx[i+3]*0x100 + data_rx[i+4];
//                 V_temp[i] = (V_temp[i] * 0x186a) / 0x3fff;
//             }
//             Dev[DevAdr].Vtot = 0;
//             Dev[DevAdr].CellsNum = 0;
//             for (i=0; i<6; i++){
//                Dev[DevAdr].Vcell[i]=V_temp[i*2];
//
//                Dev[DevAdr].Vtot += (Dev[DevAdr].Vcell[i] / 100);
//
//                if(Dev[DevAdr].Vcell[i] > 500 ){
//                    if(Dev[DevAdr].Vmin > Dev[DevAdr].Vcell[i]){
//                        Dev[DevAdr].Vmin = Dev[DevAdr].Vcell[i];
//                        Dev[DevAdr].Lowest = i;
//                    }
//                    if(Dev[DevAdr].Vmax < Dev[DevAdr].Vcell[i]){
//                        Dev[DevAdr].Vmax = Dev[DevAdr].Vcell[i];
//                        Dev[DevAdr].Highest = i;
//                    }
//                    Dev[DevAdr].CellsNum++;
//                }
//                else
//                    Dev[DevAdr].Vcell[i] = 0;
//             }

//     /*++++++++++++++++++++++ TEMPERATURE START ++++++++++++++++++++++++++++++*/
//             float Tmp_DeltaT=0;
//             unsigned int Tmp_DevTemp=0;
//
//             if ( HARDWARE_V == 2 ){
//                 for (i=0; i<4; i+=2){
//                     T_temp[i] = data_rx[i+15]*0x100 + data_rx[i+16];
//                     T_temp[i] = (T_temp[i] + 2) / 0x8116;
//                     T_temp[i] = 0x71c / T_temp[i];
//                     T_temp[i] = (T_temp[i] - 0x5be) - 0x71c;
//                 }
//             }
//
//             if ( HARDWARE_V >= 3 ){
//
//                 T_temp[0] = data_rx[17]*0x100 + data_rx[18];
//                 T_temp[0] = (T_temp[0] + 2) / 0x8116;
//                 T_temp[0] = 0x71c / T_temp[0];
//                 T_temp[0] = (T_temp[0] - 0x5be) - 0x71c;
//
//                 T_temp[2] = data_rx[15]*0x100 + data_rx[16];
//                 T_temp[2] = (T_temp[2] + 2) / 0x8116;
//                 T_temp[2] = 0x71c / T_temp[2];
//                 T_temp[2] = (T_temp[2] - 0x5be) - 0x71c;
//
                    //NON IMPLEMENTABILI
//                 // +++++++ GPAI ACQUISITION AS TEMPERATURE #3 +++++
//                 T_temp[4] = data_rx[1]*0x100 + data_rx[2];
//                 T_temp[4] = (T_temp[4] + 2) / 0x8116;
//                 T_temp[4] = 0x71c / T_temp[4];
//                 T_temp[4] = (T_temp[4] - 0x5be) - 0x71c;
//
//             }
//
//             for(k=0; k<3; k++)
//                 for (i=0; i<34 ; i++ )
//                     if( (T_temp[k*2] >= Temp_Table[i]*10000) && (T_temp[k*2] < Temp_Table[i+1]*10000) ){
//                         Tmp_DevTemp = (125 - (i*5));
//                         Tmp_DeltaT = (Temp_Table[i+1]*10000) - (Temp_Table[i]*10000);
//                         Dev[DevAdr].Tcell_raw[k] +=  Tmp_DevTemp - ( (unsigned int) ( ( 5*( T_temp[k*2] - Temp_Table[i]*10000 ) ) / Tmp_DeltaT ) );
//                     }
//
//     /*------------------------- TEMPERATURE END --------------------------------*/
//
//             Dev[DevAdr].BalCtrl = 0;
//
//    /*+++++++++++++++++++++++++++ Res Calc. START ++++++++++++++++++++++++++++++++*/
//             if(Dev[DevAdr].Vdcell[0] == 0){
//                 for(i=0; i<6; i++){
//                     Dev[DevAdr].Vdcell[i] = Dev[DevAdr].Vcell[i];
//                     Dev[DevAdr].CurrDcell = InstaCurr;
//                 }
//             }
//             float Rcell_tpm;
//             if( (fabsf(Dev[DevAdr].Vdcell[0] - Dev[DevAdr].Vcell[0]) > DeltaRV) &&  (fabsf(InstaCurr - Dev[DevAdr].CurrDcell) > DeltaRI) ){
//                 for(i=0; i<6; i++){
//                    Rcell_tpm = ( fabsf(Dev[DevAdr].Vdcell[i] - Dev[DevAdr].Vcell[i]) / fabsf(InstaCurr - Dev[DevAdr].CurrDcell) );
//                    if( (Rcell_tpm > 0) && (Rcell_tpm < 20) )
//                        Dev[DevAdr].Rcell[i] = Rcell_tpm;
//                    Dev[DevAdr].Vdcell[i] = Dev[DevAdr].Vcell[i];
//                    if(Dev[DevAdr].Rmax < Dev[DevAdr].Rcell[i]){
//                        Dev[DevAdr].Rmax = Dev[DevAdr].Rcell[i];
//                        Dev[DevAdr].R_Highest = i;
//                    }
//                    if(Dev[DevAdr].Rmin > Dev[DevAdr].Rcell[i]){
//                        Dev[DevAdr].Rmin = Dev[DevAdr].Rcell[i];
//                        Dev[DevAdr].R_Lowest = i;
//                    }
//                    Dev[DevAdr].Rtot +=  Dev[DevAdr].Rcell[i] ;
//                 }
//                 Dev[DevAdr].CurrDcell = InstaCurr;
//             }
//    /*--------------------------- Res Calc. END ------------------------------------*/
//         }
//
//         if(RegAdr == ALERT_STATUS){
//             Dev[DevAdr].Alarm = data_rx[0];
//             Dev[DevAdr].Fault = data_rx[1];
//             Dev[DevAdr].COV = data_rx[2];
//             Dev[DevAdr].CUV = data_rx[3];
//         }
//
//         if(RegAdr == IO_CONTROL)
//             Dev[DevAdr].IORead = data_rx[0];
//
//         if(RegAdr == CB_CTRL)
//             Dev[DevAdr].BalCtrlRead = data_rx[0];
//
//         Dev[DevAdr].SingleRead = data_rx[0];
//
//         return CRC_fault = 0;
//
//     }else{
//         Gcrc8 = 0;
//         return CRC_fault = 1;
//     }

//}




//
//
//void Write_SPI(unsigned int DevAdr, unsigned int RegAdr, unsigned int Data){
//
//    SPI_WRITING = 1;
//
//    int mystring[3];
//    unsigned int crc8=0, crctemp=0, z;
//
//    StatoSPI = 1;
//
//    mystring[0] = (((DevAdr)<<1)|0x01);
//    mystring[1] = RegAdr;
//    mystring[2] = Data;
//
//    for (z = 0; z < 3; z++){
//        crctemp = crc8 ^ mystring[z];
//        crc8 = CrcTable[crctemp];
//    }
//
//    SPI1_Buff16[0]= mystring[0];
//    SPI1_Buff16[1]= mystring[1];
//    SPI1_Buff16[2]= mystring[2];
//    SPI1_Buff16[3]= crc8;
//
//    _DMA4IF = 1;                       /* Force Interrupt */
//}


/**
 * \fn void REQ_Read_SPI(  int DevAdr,   int RegAdr,   int Size)
 * \brief Read a register from a device. It is required to write the number of byte to read.
 * @param DevAdr    Addressing of the initialization procedure
 * @param  RegAdr   Register selection in the device bq76PL455A-Q1
 * @param Size      Data dimension (number of bytes)
 */
//void REQ_Read_SPI(  int DevAdr,   int RegAdr,   int Size){
//    unsigned int mystring[3];
//    unsigned int crctemp=0,z;
//
//    read_size = Size;
//
//    StatoSPI = 2; //Scrittura richiesta info
//
//    mystring[0] = DevAdr;
//    mystring[1] = RegAdr;
//    mystring[2] = Size;
//
//
//    data_req[0]= mystring[0];
//    data_req[1]= mystring[1];
//    data_req[2]= mystring[2];
//
//    Gcrc8=0;
//
//    for (z = 0; z < 3; z++){
//        crctemp = Gcrc8 ^ mystring[z];
//        Gcrc8 = CrcTable[crctemp];
//    }
//
//    _DMA4IF = 1;                       /* Force Interrupt */
//
//}


 void __attribute__((__interrupt__,__auto_psv__)) _DMA5Interrupt(void)
{
    _DMA5IF = 0;
    IFS3bits.DMA5IF = 0;
}


 /* CS SENZA MOS DOPO ISO -> CS_H = 1 e CS_L = 0
    CS CON MOS DOPO ISO   -> CS_H = 0 e CS_L = 1 */
void __attribute__((__interrupt__,__auto_psv__)) _DMA4Interrupt(void) // DMA4 SPI
{
    _DMA4IF = 0;

    unsigned int static tmp __attribute__((space(dma),aligned(2)));
    int spi_temp;
    unsigned int cl=0;

    switch (StatoSPI)
    {
       case 0:                             /* Idle */
        break;

       case 1:                             /* Start a new SPI Sequence */  /* WRITE SEQUENCE */
            SPI2STATbits.SPIROV = 0;
            spi_temp = SPI2BUF;

            /* Toggle pin */
            PORTGbits.RG9 = CS_H;                /* switch RG9 to 1 */
            __delay32(4074);                  /* Delay  */
            PORTGbits.RG9 = CS_L;                /* switch RG9 to 0 */
            __delay32(4000);                  /* Delay  */

            /* Write only Sequence */
            DMA5CNT = 3;                       /* DMA (SPI) Tx */ //Num byte + 1
            DMA5STA = (unsigned int) &SPI1_Buff16[0];
            DMA4CNT = 3;                       /* DMA (SPI) Rx */
            DMA4STA = (unsigned int) &tmp;
            DMA5CON &= 0xFFEF;                 /* Use DMA Tx Post-Incrementation */
            DMA4CON |= 0x10;                   /* Do not use DMA Rx Post-Incrementation */
            DMA4CON &= 0xF7FF;                 /* Disable Null data Write */
            DMA5CONbits.CHEN = 1;              /* Enable DMA Tx */
            DMA4CONbits.CHEN = 1;              /* Enable DMA Rx */
            DMA5REQbits.FORCE = 1;             /* Force Start DMA Tx */

            for (cl=0; cl<4000; cl++);
            PORTGbits.RG9 = CS_H;               /* switch RG9 to 1 */

            SPI_WRITING = 0;

        break;

        case 2:  //READ TX                              /*READ SEQUENCE*/

    
            /* Toggle pin */
            PORTGbits.RG9 = CS_H;            /* switch RG9 to 1 */
            __delay32(5074);                 /* Delay  */
            PORTGbits.RG9 = CS_L;            /* switch RG9 to 0 */
            __delay32(5000);

            SPI2STATbits.SPIROV = 0;         /* SPIROV: Receive Overflow Flag bit */
            spi_temp = SPI2BUF;

            /* Write only Sequence */
            DMA5CNT = 2;                     /* DMA (SPI) Tx */
            DMA5STA = (unsigned int) &data_req[0];
            DMA4CNT = 2;                     /* DMA (SPI) Rx */
            DMA4STA = (unsigned int) &tmp;
            DMA5CON &= 0xFFEF;               /* Use DMA Tx Post-Incrementation */
            DMA4CON |= 0x10;                 /* Do not use DMA Rx Post-Incrementation */
            DMA4CON &= 0xF7FF;               /* Disable Null data Write */
            DMA5CONbits.CHEN = 1;            /* Enable DMA Tx */
            DMA4CONbits.CHEN = 1;            /* Enable DMA Rx */
            DMA5REQbits.FORCE = 1;           /* Force Start DMA Tx */

            for (cl=0; cl<5000; cl++);

            /*Receive the answer*/
            SPI2STATbits.SPIROV = 0;
            spi_temp = SPI2BUF;
            DMA4STA = (unsigned int) &data_rx[0];
            DMA4CNT = 0;           /* DMA (SPI) Rx: the value is retrieved from the first byte of the response(#of successive bytes-1 + 2 crc bytes) */
            DMA4CON &= 0xFFEF;               /* Use DMA Rx Post-Incrementation */
            DMA4CON |= 0x800;                /* Null data Write in addition do Peripheral to RAM write */

            DMA4CONbits.CHEN = 1;            /* Enable DMA Rx */
            SPI2BUF = 0;                     /* Start DMA sequence */
            for (cl=0; cl<1500; cl++);
            PORTGbits.RG9 = CS_H;               /* switch RG9 to 1 */
            /* Toggle pin */
            PORTGbits.RG9 = CS_H;            /* switch RG9 to 1 */
            __delay32(5074);                 /* Delay  */
            PORTGbits.RG9 = CS_L;            /* switch RG9 to 0 */
            __delay32(5000);


            SPI2STATbits.SPIROV = 0;
            spi_temp = SPI2BUF;

            DMA4STA = (unsigned int) &data_rx[1];
            DMA4CNT = data_rx[0]+2;           /* DMA (SPI) Rx: the value is retrieved from the first byte of the response(#of successive bytes-1 + 2 crc bytes) */
            DMA4CON &= 0xFFEF;               /* Use DMA Rx Post-Incrementation */
            DMA4CON |= 0x800;                /* Null data Write in addition do Peripheral to RAM write */
            DMA4CONbits.CHEN = 1;            /* Enable DMA Rx */
            SPI2BUF = 0;                     /* Start DMA sequence */

            
            for (cl=0; cl<18000; cl++);
            
            PORTGbits.RG9 = CS_H;               /* switch RG9 to 1 */


        break;

        
        case 3:      /* Start a new SPI Sequence */     /*INITIALIZATION SEQUENCE*/

            for(cicli_dma=0; cicli_dma <= cicli_init+1; cicli_dma+=4){

                PORTGbits.RG9 = CS_H;              /* switch RG9 to 1 */
                /* Delay */
                __delay32(4074);                 /* Delay  */
                /* Toggle pin */
                PORTGbits.RG9 = CS_L;             /* switch RG9 to 0 */
                __delay32(4000);

                SPI1_Buff16[0] = VettoreInit[cicli_dma];
                SPI1_Buff16[1] = VettoreInit[cicli_dma+1];
                SPI1_Buff16[2] = VettoreInit[cicli_dma+2];
                SPI1_Buff16[3] = VettoreInit[cicli_dma+3];

                SPI2STATbits.SPIROV = 0;
                /* Write only Sequence */
                DMA5CNT = write_size;                       /* DMA (SPI) Tx */
                DMA5STA = (unsigned  int) &SPI1_Buff16[0];
                DMA4CNT = write_size;                       /* DMA (SPI) Rx */
                DMA4STA = (unsigned  int) &tmp;
                DMA5CON &= 0xFFEF;                 /* Use DMA Tx Post-Incrementation */
                DMA4CON |= 0x10;                   /* Do not use DMA Rx Post-Incrementation */
                DMA4CON &= 0xF7FF;                 /* Disable Null data Write */
                DMA5CONbits.CHEN = 1;              /* Enable DMA Tx */
                DMA4CONbits.CHEN = 1;              /* Enable DMA Rx */
                DMA5REQbits.FORCE = 1;             /* Force Start DMA Tx */

                for (cl=0; cl<12000; cl++);
                PORTGbits.RG9 = CS_H;               /* switch RG9 to 1 */

                for (cl=0; cl<20000; cl++);

         }
        
        break;


        default:                            /* Sequence finished */
            StatoSPI = 0;               /* Should never happend */
        break;

    }                                    /* End of switch case sequence*/

 
    IFS2bits.DMA4IF = 0;				// Clear the DMA0 interrupt flag bit

}        /* End of interrupt */


//AUTO ADRESSING PROCEDURE
/**
 * \fn void set_autoAdressing(void)
 * \brief Summary of Steps for Auto-Addressing
 *           1. Make sure all boards are awake and ready to receive the AUTO-ADDRESS ENABLE command.
 *           2. Turn on the downstream communications drivers on all devices in the chain.
 *           3. Place all devices into auto-address learn mode.
 *           4. Send out new addresses to all possible bq76PL455A-Q1 device addresses, in incremental order,
 *               starting at 0.
 *           5. Read back the value stored in the Device Address register from each newly addressed device, starting
 *               at address 0 and proceeding sequentially. The last bq76PL455A-Q1 device to successfully respond is
 *               the last device in the serial chain.
 *           6. Turn off the high-side communications receiver on the last (top-most) device in the chain.
 *           7. Turn off the single-ended transmitter on all except the lowest device in the chain.
 *  @return void
 */
void set_autoAdressing(void)
{
    unsigned int *data;
    unsigned char i;
    unsigned int j;
    unsigned int *answer;

    /**
     * F2 10 10E0 3F35 Configure communications as per the previous description
                F2 = General Broadcast Write Without Response 2 bytes (8-bit register
                        addressing)
                10 = Register Address 16 (Communication Configuration register)
                10E0 = Data
                3F35 = CRC
     */

    *data=0x10E0;
    printf("1-step:\n");
    write_Broadcast(REGISTER_ADDRESSING,2,COMCONFIG,data);
    

    /**
     * F1 0E 10 545F Configure the bq76PL455A-Q1 device to use auto-addressing to select address
                F1 = General Broadcast Write Without Response 1 byte (8-bit register
                        addressing)
                0E = Register Address 14 (Device Configuration)
                10 = Data (set the ADDR_SEL bit)
                545F = CRC

     * F1 0C 08 5535 Configure the bq76PL455A-Q1 device to enter auto-address mode
                F1 = General Broadcast Write Without Response 1 byte (8-bit register
                        addressing)
                0C = Register Address 12 (Device Control)
                08 = Data (set the AUTO_ADDRESS bit)
                5535 = CRC
     */

     *data=0x10;
     printf("2-step: -1\n");
     write_Broadcast(REGISTER_ADDRESSING,1,DEVCONFIG,data);
     
     *data=0x08;
     printf("2-step: -2\n");
     write_Broadcast(REGISTER_ADDRESSING,1,DEV_CTRL,data);
     
     /**
      * F1 0A 00 5753 Configure device 1 to address 0
                F1 = Broadcast Write Without Response 1 byte (8-bit register addressing)
                0A = Register Address 10 (Device Address register)
                00 = Data (address = 00)
                5753 = CRC
      */
    printf("3-step: \n");

     for(i=0;i<16;i++){
        *data=i;
        write_Broadcast(REGISTER_ADDRESSING,1,ADDR,data);
     }

     /**
      * Map the list of devices
      */
     printf("4-step: \n");
     Check_Dev();

     /**
      * Clear all faults
      */
     printf("5-step: \n");
     Clear_Faults_Broadcast();

}








