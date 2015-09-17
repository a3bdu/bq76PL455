/*********************************************************************
* File:        common.h
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r00
* Description: This file manages the HAL (hardware Abstract Layer) del chip dsPIC33FJ.
* Created on 3 Marzo 2014, 15.35
*********************************************************************/

#ifndef COMMON_H
#define COMMON_H
#define bq76PL455     //Define the correct board for the correct memory mapping
#ifdef bq76PL455
#include "protocol_bq76PL455.h"
#endif


#define HARDWARE_V 3

#define CELL_CHEMISTRY 1 //0:LiPo - 1:LiC_1500

#define CHECK_BIT(var,pos) (var & (1 << pos))
#define length          24     // DMA Buffer length

#define CURRENT_CUT    0.2

#define ON  1
#define OFF 0

#define CS_H  0 //1 senza mos, 0 con mos
#define CS_L  1

/*********************************************************************
* DIGITAL I/O Defines
*********************************************************************/

/* HARDWARE OUT */
#define DISCHARGE_RLY  LATDbits.LATD8
#define PRECHARGE_RLY  LATDbits.LATD9
#define CONTACT_RLY_N  LATDbits.LATD10
#define CONTACT_RLY_P  LATDbits.LATD11
#define LED            LATGbits.LATG13
#define FAULT_OUT      LATDbits.LATD6
#define FAN_CMD        LATDbits.LATD7
#define POWER_LOOP     LATGbits.LATG12

/* HARDWARE IN */
#define INTERLOCK_IN   PORTDbits.RD0
#define SENSE_VBATT    PORTDbits.RD1 
#define SENSE_VCHRG    PORTDbits.RD2
#define SENSE_KEY      PORTDbits.RD3
#define IMD_IN         PORTDbits.RD4
#define IMD_PWM        PORTDbits.RD5


/*********************************************************************
* CAN Defines
*********************************************************************/
#define BMS_CAN_INPUT             0x100

/*********************************************************************
* Texas Ins. BMS Defines:
*********************************************************************/

#define MAX_DEVICE      43  //   ->Up to 256-Series Cells, 6-to-16 Cells per Device => 256/6=43




#ifdef bq76PL455
    #define REGISTER_ADDRESSING ADDR_SIZE_8bit
    #define NCELLs 6
    #define NcellMIN 6
    #define NcellMAX 16
    #define NTempMAX 8
#define RESET_DEV DEV_CTRL


typedef struct{

    unsigned int id;            // ADDR
    unsigned int Status;        // STATUS
    unsigned int Vcell[16];     //CHANNELS
    unsigned int Tcell[8];      //AUXn ; n=0,...,7
    unsigned int Tcell_raw[8];  //--------------------->???
    float Rcell[16];            //cells' resistence
    unsigned int Vdcell[16];    //cells' voltage
    float CurrDcell;            //--------------------->serve sensore di HALL
    unsigned int Alarm;         //--------------------->non è integrato
    unsigned int Fault[8];      //-->FAULT_COM, FAULT_SYS, FAULT_DEV, FAULT_AUX, FAULT_2UV, FAULT_2OV,
    unsigned int COV;           //-->FAULT_OV
    unsigned int CUV;           //-->FAULT_UV
    unsigned int IORead;        //-->I/O pin control   : DEVCONFIG o DEV_CTRL
    unsigned int BalCtrl;       //-->CBCONFIG 0x13 (19) Balance Configuration
    unsigned int BalCtrlRead;   //-->CBENBL 0x14?15 (20?21) Balancing Enable
    unsigned int data_length;
    unsigned int Vtot;          //calculated value about the battery pack
    unsigned int Vmin;          //statistic value, used in monituring
    unsigned int Vmax;          //statistic value, used in monituring
    float Rmax;                 //statistic value, used in monituring
    float Rtot;                 //statistic value, used in monituring
    float Rmin;                 //statistic value, used in monituring
    unsigned int SingleRead;    //------>dovrebbe essere in caso di ricezioni dati spuri, si evita di bloccare il protocollo.
    unsigned int CellsNum;      //Number of cells
    unsigned int Lowest;        //indentification of the lower tension cell
    unsigned int Highest;       //indentification of the higher tension cell
    unsigned int R_Highest;     //indentification of the lower resistance cell
    unsigned int R_Lowest;      //indentification of the higher resistance cell
    unsigned int ReadCnt;       //messagese received 

}DevData;

#endif











/*
 *  Mapping del device bq76PL536
 */
#ifdef bq76PL536
#define DEVICE_STATUS   0x00
#define GPAI_H          0x01
#define GPAI_L          0x02
#define VCELL1_H        0x03
#define VCELL1_L        0x04
#define VCELL2_H        0x05
#define VCELL2_L        0x06
#define VCELL3_H        0x07
#define VCELL3_L        0x08
#define VCELL4_H        0x09
#define VCELL4_L        0x0a
#define VCELL5_H        0x0b
#define VCELL5_L        0x0c
#define VCELL6_H        0x0d
#define VCELL6_L        0x0e
#define TEMPERATURE1_H  0x0f
#define TEMPERATURE1_L  0x10
#define TEMPERATURE2_H  0x11
#define TEMPERATURE2_L  0x12
#define ALERT_STATUS    0x20
#define FAULT_STATUS    0x21
#define COV_FAULT       0x22
#define CUV_FAULT       0x23
#define PRESULT_A       0x24
#define PRESULT_B       0x25
#define ADC_CONTROL     0x30
#define IO_CONTROL      0x31
#define CB_CTRL         0x32
#define CB_TIME         0x33
#define ADC_CONVERT     0x34
#define SHDW_CTRL       0x3a
#define ADDRESS_CONTROL 0x3b
#define RESET_DEV       0x3c
#define TEST_SELECT     0x3d
#define E_EN            0x3f
#define FUNCTION_CONFIG 0x40
#define IO_CONFIG       0x41
#define CONFIG_COV      0x42
#define CONFIG_COVT     0x43
#define CONFIG_CUV      0x44
#define CONFIG_CUVT     0x45
#define CONFIG_OT       0x46
#define CONFIG_OTT      0x47
#define USER1           0x48
#define USER2           0x49
#define USER3           0x4a
#define USER4           0x4b



typedef struct{
    
    unsigned int id;
    unsigned int Status;
    unsigned int Gpai[2];
    unsigned int Vcell[6];
    unsigned int Tcell[3];
    unsigned int Tcell_raw[3];
    float Rcell[6];
    unsigned int Vdcell[6];
    float CurrDcell;
    unsigned int Alarm;
    unsigned int Fault;
    unsigned int COV;
    unsigned int CUV;
    unsigned int IORead;
    unsigned int BalCtrl;
    unsigned int BalCtrlRead;
    unsigned int data_length;
    unsigned int Vtot;
    unsigned int Vmin;
    unsigned int Vmax;
    float Rmax;
    float Rtot;
    float Rmin;
    unsigned int SingleRead;
    unsigned int CellsNum;
    unsigned int Lowest;
    unsigned int Highest;
    unsigned int R_Highest;
    unsigned int R_Lowest;
    unsigned int ReadCnt;

}DevData;

#endif

extern float Ah, SOC, SOCH_Ah, SOCL_Ah, InstaCurr, AvgCurr, Curr_Gain, Curr_Offset, CHRG_Itmp, RCell_Max, RCell_Min, WaterLeak_AN ;

extern unsigned int  BALANCE_EN, SLEEP, VMIN_BAL, DELTA, CAN_BASE_ID, Cap_TOT,
                     Over_V, Under_V, TOTAL_V, SOCH, SOCL, SOCT, PLUGGED_DEV, HARD_ERROR, ERROR_Flag,
                     Gcrc8, Dev_Highest, Dev_Lowest, CHARGER_EN, SOD, Ah_st_w, Ah_st_r, CONTACT_EN, PRECHR_TM,
                     DeltaRV, DeltaRI, FORCE_BALANCE_EN, FORCE_LEDS_EN, CANWatchdog, CRC_ctrl, FAULT_SPI,
                     MAX_Iac, MAX_Vdc, MAX_Idc, Charger_Idc, MAX_Tmp, Temp_Highest, FANS_Enable, FANS_Temperature,
                     VCell_Avg, TCell_Avg, Temp_Lowest, RCell_Avg, password, CrntSense_Plg, MAX_BAL_Tmp, 
                     IMD_Resistance, DISCHR_TM, KESM_Timeout;

extern unsigned int  DEEaddr_CAN, DEEaddr_Cap, DEEaddr_OverV,
                     DEEaddr_UnderV, DEEaddr_DELTA, DEEaddr_VbMIN, DEEaddr_CGain, DEEaddr_COffset,
                     DEEaddr_SOCH, DEEaddr_SOCL, DEEaddr_SOCH_Ah, DEEaddr_SOCL_Ah, DEEaddr_SOCT, 
                     DEEaddr_SLEEP, DEEaddr_SOD, DEEaddr_Ah, DEEaddr_PrecTM,
                     DEEaddr_CHRGIac,   DEEaddr_CHRGVdc,   DEEaddr_CHRGIdc, DEEaddr_CHRGTmp, 
                     DEEaddr_FANsTmp,   DEEaddr_BALTemp, DEEaddr_DeltaRV, DEEaddr_DeltaRI, DEEaddr_DiscTM;

extern unsigned int  CAN_VCell_Avg, CAN_Dev_Highest, CAN_Dev_Lowest, CAN_TCell_Avg ,
                     CAN_Temp_Highest, CAN_Temp_Lowest, CAN_RCell_Avg , CAN_RCell_Max ,
                     CAN_RCell_Min, CAN_TOTAL_V;

extern unsigned int  SLEEP_cont;

extern unsigned long Time;

extern DevData Dev[];


#endif

