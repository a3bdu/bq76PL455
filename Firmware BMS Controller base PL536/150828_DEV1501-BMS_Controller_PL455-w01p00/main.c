/*********************************************************************
* File:        main.c
* Author:      Carlo Menegazzo
* Co-Autor     Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Mail:        carlo.menegazzo.1@gmail.com, carlo.campanale@podiumengineering.com 
* Processor:   dsPIC33FJ128GP706A
* Board:       150509_DEV_1501-BMSCONTROLLER-R02P00
* TI Chip:     BQ76PL455A-Q1
*
* Created on 28 Aug. 2015, 09.35
*********************************************************************/

/*
 TEST VERSION
 */

#include "p33fxxxx.h"
#include "math.h"
#include "common.h"
#include "init.h"
#include "timer.h"
#include "adc.h"
#include "can.h"
#include "spi.h"
#include "EEPROM_Emulation_16bit.h"
#include "rs232.h"
#include <libpic30.h>



unsigned int BALANCE_EN=0 , SLEEP=0, VMIN_BAL, DELTA, Cap_TOT, TOTAL_V=0, Dev_Highest=0, Dev_Lowest=10000, Dev_Lowest_BNK=0,
             PLUGGED_DEV=0, Max_V=3000, MAX_Iac=0, MAX_Vdc=0, MAX_Idc=0, MAX_Tmp=60, Over_V, Under_V, SOCH, SOCL, SOCT=10000, Charger_Idc=0,
             CAN_BASE_ID=0x130,  Ah_st_w, Ah_st_r, CHARGER_EN=0, SOD=0, CONTACT_EN=0, CANWatchdog, CRC_ctrl=0,
             DeltaRV = 50, DeltaRI = 20, PRECHR_TM=5, PRECHR_STAT=0, FORCE_BALANCE_EN = 0, FORCE_LEDS_EN = 0,
             nVCell_Max, nVCell_Min, VCell_Avg, Temp_Highest=0, Temp_Lowest, nTCell_Min, nTCell_MAX, TCell_Avg, nRCell_Max, nRCell_Min, RCell_Avg,
             FAULT_SPI = 0, HARD_ERROR = 0, ERROR_Flag = 0, FANS_Enable = 0, FANS_Temperature = 40, TOTAL_CELLS = 0, password = 0,
             CrntSense_Plg = 0, MAX_BAL_Tmp = 60, IMD_Resistance=0, DISCHR_TM=10, KESM_Timeout=0;

unsigned long Time=0;


float SOCH_Ah, SOCL_Ah, InstaCurr, Curr_Gain=1, Curr_Offset=0, AvgCurr, SOC=0, Ah=-100, avg=0, CHRG_Itmp, RCell_Max=0, RCell_Min=1001, WaterLeak_AN=0;

//       EEPROM Addresses
unsigned int  DEEaddr_DELTA = 8,      DEEaddr_VbMIN = 12,    DEEaddr_Ah = 18,       DEEaddr_CAN = 24,
              DEEaddr_Cap = 28,       DEEaddr_OverV = 32,    DEEaddr_UnderV = 36,   DEEaddr_CGain = 40,
              DEEaddr_COffset = 44,   DEEaddr_SOCH = 48,     DEEaddr_SOCL = 52,     DEEaddr_SOCH_Ah = 56,
              DEEaddr_SOCL_Ah = 60,   DEEaddr_SOCT = 64,     DEEaddr_SLEEP = 68,    DEEaddr_SOD = 72,
              DEEaddr_PrecTM = 76,    DEEaddr_CHRGIac = 80,  DEEaddr_CHRGVdc = 84,  DEEaddr_CHRGIdc = 88,
              DEEaddr_CHRGTmp = 92,   DEEaddr_FANsTmp = 96,  DEEaddr_BALTemp = 100, DEEaddr_DeltaRV = 104,
              DEEaddr_DeltaRI = 108,  DEEaddr_DiscTM = 112;


int main(void){
    
    Init_DevicesTi();
    set_autoAdressing();
    
    return 0;
}



//int main ( void )
//{
//    unsigned int cl;
//
//    //  Init Hardware I/O
//        PRECHARGE_RLY = OFF;
//        CONTACT_RLY_P = OFF;
//        CONTACT_RLY_N = OFF;
//        DISCHARGE_RLY = ON;
//        FAULT_OUT = OFF;
//        LED = OFF;
//        FAN_CMD = OFF;
//        POWER_LOOP = ON;
//
//    //  Init dsPIC
//        InitPin();
//        InitOsc();
//
//    //  Init SPI
//        InitSPI();
//
//    //  Init ADC
//        InitDMA0();
//        InitADC();
//
//    //  Init CAN
//        InitCAN();
//
//    //  Init EEPROM
//        DataEEInit();
//        dataEEFlags.val = 0;
//        InitEEPROM_Var();
//
//    //  Init BMS Texas Ins.
//        for (cl=0; cl<50000; cl++);     //->delay of 1.25ms
//        Reset_SPI();
//        for (cl=0; cl<50000; cl++);     //->delay of 1.25ms
//
//    //  Init RS232
//	Init_rs232();
//
//    //  Init Timers
//        InitTimer1();   //->Vehicle Routine
//        InitTimer2();   //->SPI Process
//        InitTimer3();   //->ADC
//        InitTimer4();   //->IMD PWM
//        InitTimer5();   //->RS232
//        InitTimer6();   //->CAN KESM
//
//    while(1);
//
//    return 0;
//}


float SOD_cont=0;
unsigned int Timer_1Sec=0, Timer_2Sec=0, SOCT_cont=0, CAN_Watchdog=0, BAL_cont=0, 
             Empty_Cycle=0, BAL_Dev_Lowest=0, BAL_Temp_Highest = 0, SLEEP_cont=0, TCellAvg_cont=0;
int cont=0, dev=1, can_c=0x0;
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt( void ) {

    int i,k,j;
    unsigned int data=0x02;
   /*+++++++++++++++++++++++++++++ TEXAS BMS START +++++++++++++++++++++++++++++++++++++++++*/
    
    switch(cont){

        case(1):            //WATCHDOC CHECK AND SLEEP MODE

            ClrWdt();   // CLEAR HARDWARE WATCHDOG
            if(SLEEP == 1){
//                if( Dev[dev].IORead >= 4 ){
//                    Write_SPI(0x3f, ALERT_STATUS, 0x04);
//
//                    if(SLEEP_cont < 1000)
//                        SLEEP_cont++;
//                    else
//                        cont = 0;
//                }
//                else{
                    Sleep_SET();
                    SLEEP_cont=0;
//                }

                for(k=0; k<6; k++)
                    Dev[dev].Vcell[k] = 0;           
            }

        break;

        case(2):    //READ REGISTER IO_CONTROL: AUX - GIPI_OUT - GPIO_IN - SLEEP - TS2 - TS1
            // REQ_Read_SPI(dev, IO_CONTROL, 0x01);
        break;

        case(4):    //READ REGISTER IO_CONTROL: AUX - GIPI_OUT - GPIO_IN - SLEEP - TS2 - TS1
            //CRC_ctrl += Read_SPI(dev, IO_CONTROL, 0x01);
            if(SLEEP == 1)
                cont=28; // se SLEEP salta ciclo normale lettura
        break;

        //DEVICES CONTROL
        case(10):   //lettura dei registri da 0x00 a 0x12 => penso sia per aggiornare i registri   ->dovrebbe essere registro CMD
        //    REQ_Read_SPI(dev, DEVICE_STATUS, 0x12);   //LETTURA fino a Temp2
        break;

        case(12):
            if(Dev[dev].Status==0)
                cont = 45;
            //lettura dei registri da 0x00 a 0x12
            //CRC_ctrl += Read_SPI(dev, DEVICE_STATUS, 0x12);
            CRC_ctrl += Read_SPI(dev, CMD, 0x0F);
        break;

        case(14):
                //lettura di 4 registri => fatto per aggiornare i registri.
            //REQ_Read_SPI(dev, ALERT_STATUS, 0x04);   //LETTURA fino a CUV fault
        break;

        case(16):
                //lettura delle alert
            //CRC_ctrl += Read_SPI(dev, ALERT_STATUS, 0x04);
        break;

        //PER ORA NON ATTIVO IL BALANCING
//        case(18):
//            if(BAL_Temp_Highest < 20)
//                BAL_Temp_Highest = 20;
//            if ( FORCE_BALANCE_EN == 0){
//                if( ( BAL_Temp_Highest < MAX_BAL_Tmp ) && ( SLEEP == 0 ) ){
//                    //+++++++++++++++++++++++++ BALANCE START ++++++++++++++++++++++++++++++++//
//                    if( BALANCE_EN ){  //CAN Enable
//                        if( (!(CHECK_BIT(Dev[dev].Status , 1))) && ( BAL_cont > (BAL_Temp_Highest*5) ) ){  //Bilanciamento non in corso
//                            for (i=0; i < Dev[dev].CellsNum; i++){
//                                if( ((Dev[dev].Vcell[i] - DELTA) > BAL_Dev_Lowest) && (Dev[dev].Vcell[i] > VMIN_BAL)  ){  //Bilanciamento
//                                    if( (Dev_Lowest_BNK==dev) && (Dev[dev].Lowest == i ) ){
//                                            Nop();
//                                    }else{
//                                        Dev[dev].BalCtrl = (Dev[dev].BalCtrl | (0x01<<i));
//                                        write_to_Device(REGISTER_ADDRESSING, 2, dev, CBENBL, &Dev[dev].BalCtrl);
//                                    }
//                                }
//                            }
//                        }
//                    }
//                    //-------------------------- BALANCE END ------------------------------//
//                }else
//                    BALANCE_EN = 0;
//            }else{
//                if( (!(CHECK_BIT(Dev[dev].Status , 1))) && (BAL_cont > 100) ){
//                    for (i=0; i < Dev[dev].CellsNum; i++){
//                        Dev[dev].BalCtrl = (Dev[dev].BalCtrl | (0x01<<i));
//                        write_to_Device(REGISTER_ADDRESSING, 2, dev, CBENBL, &Dev[dev].BalCtrl);
//                    }
//                }
//            }
//
//        break;

        case(24):
//            if((CHECK_BIT(Dev[dev].Status , 1))){  //bilanciamento in corso : in caso attiva GPIO_OUT con OPEN DRAIN e connette sensori temperatura
//                Write_SPI(dev,IO_CONTROL,0x43);
//                BAL_cont=0;
//            }else{                                 //bilanciamento non in corso
//                Write_SPI(dev,IO_CONTROL,0x03);
//                BAL_cont++;
//            }
        break;

        case(25):
//            if( FORCE_LEDS_EN ) //bilanciamento in corso    forza GPIO_OUT con OPEN DRAIN
//                Write_SPI(dev,IO_CONTROL,0x40);
        break;


        case(26):
            data=0x20;
            read_from_Device(REGISTER_ADDRESSING,1,dev, CBENBL, &data);
        break;

        case(28):
            CRC_ctrl += Read_SPI(dev, CBENBL, 0x02);   //LEGGE DATI SUL BALANCING DEI SINGOLI DEVICES
            cont=37; // Se NON SLEEP salta ultima lettura ALERT
        break;
//
//        case(30):
//            REQ_Read_SPI(dev, IO_CONTROL, 0x01);
//        break;
//
//        case(32):
//            CRC_ctrl += Read_SPI(dev, IO_CONTROL, 0x01);
//        break;
//
//        case(34):
//            REQ_Read_SPI(dev, ALERT_STATUS, 0x04);   //LETTURA fino a CUV fault
//        break;
//
//        case(36):
//            CRC_ctrl += Read_SPI(dev, ALERT_STATUS, 0x04);
//            if (SLEEP==1)
//                Write_SPI(0x3f, ALERT_STATUS, 0x00);  //SLEEP Broadcast ALERT RESET
//        break;

    }
   /*----------------------------------- SWITCH END -----------------------------------*/


/*++++++++++++++++++++++++++++++++ CONTROL TIMERS START ++++++++++++++++++++++++++++++++*/
    cont++;
    if(cont > 50)
    {
     ////////////////////
        cont = 0;
        dev++;
        
     ////////////////////
    }

//    if(dev > PLUGGED_DEV)    //Fine ciclo devices+++++++++++++++++++++++++++++++++++++++++++++++++++
//    {
//
//      //  if(SLEEP==0) //Comando sleep in rs232.c
//      //      Write_SPI(0x3f, ADC_CONVERT, 1); //start adc convert TI
//
//        dev = 1;
//        can_c = 0;
//        TOTAL_V = 0;
//        Empty_Cycle = 1;
//        TOTAL_CELLS = 0;
//        VCell_Avg = 0;
//        RCell_Avg = 0;
//        TCell_Avg = 0;
//
//        for(i=1; i <= PLUGGED_DEV; i++){
//            TOTAL_V += Dev[i].Vtot;
//            if(Dev_Lowest > Dev[i].Vmin){
//                Dev_Lowest = Dev[i].Vmin;
//                BAL_Dev_Lowest = Dev_Lowest;
//                Dev_Lowest_BNK = i;
//                nVCell_Min = ( (i-1) * 6 ) + Dev[i].Lowest;
//            }
//            if(Dev_Highest < Dev[i].Vmax){
//                Dev_Highest = Dev[i].Vmax;
//                nVCell_Max = ( (i-1) * 6 ) + Dev[i].Highest;
//            }
//
//            VCell_Avg += Dev[i].Vtot;
//
//            if( Dev[i].Tcell[0] > 100 ){
//                if(Temp_Highest < (Dev[i].Tcell[0]-100) ){
//                    BAL_Temp_Highest = Temp_Highest;
//                    Temp_Highest = (Dev[i].Tcell[0]-100);
//                    nTCell_MAX = i;
//                }
//                if(Temp_Highest < (Dev[i].Tcell[1]-100) ){
//                    BAL_Temp_Highest = Temp_Highest;
//                    Temp_Highest = (Dev[i].Tcell[1]-100);
//                    nTCell_MAX = i+100;
//                }
//                if ( HARDWARE_V >= 3 ){
//                    if(Temp_Highest < (Dev[i].Tcell[2]-100) ){
//                        BAL_Temp_Highest = Temp_Highest;
//                        Temp_Highest = (Dev[i].Tcell[2]-100);
//                        nTCell_MAX = i+200;
//                    }
//                }
//                if(Temp_Lowest > (Dev[i].Tcell[0]-100) && Dev[i].Tcell[0] > 100 ){
//                    Temp_Lowest = (Dev[i].Tcell[0]-100);
//                    nTCell_Min = i;
//                }
//                if(Temp_Lowest > (Dev[i].Tcell[1]-100) && Dev[i].Tcell[1] > 100){
//                    Temp_Lowest = (Dev[i].Tcell[1]-100);
//                    nTCell_Min = i+100;
//                }
//                if ( HARDWARE_V >= 3 ){
//                    if(Temp_Lowest > (Dev[i].Tcell[2]-100) && Dev[i].Tcell[2] > 100){
//                        Temp_Lowest = (Dev[i].Tcell[2]-100);
//                        nTCell_Min = i+200;
//                    }
//                }
//            }
//
//            if(nTCell_MAX > 100)
//                nTCell_MAX = (nTCell_MAX-100)*2;
//            if(nTCell_Min > 100)
//                nTCell_Min = (nTCell_Min-100)*2;
//            if ( HARDWARE_V >= 3 ){
//                if(nTCell_MAX > 200)
//                    nTCell_MAX = (nTCell_MAX-100)*3;
//                if(nTCell_Min > 200)
//                    nTCell_Min = (nTCell_Min-100)*3;
//            }
//
//            if( Dev[i].Tcell[0] > 100 ){
//                TCell_Avg += (Dev[i].Tcell[0]-100);
//                TCellAvg_cont++;
//            }
//            if( Dev[i].Tcell[1] > 100 ){
//                TCell_Avg += (Dev[i].Tcell[1]-100);
//                TCellAvg_cont++;
//            }
//            if ( HARDWARE_V >= 3 && Dev[i].Tcell[2] > 100 ){
//                TCell_Avg += (Dev[i].Tcell[2]-100);
//                TCellAvg_cont++;
//            }
//
//            if( RCell_Max < Dev[i].Rmax ){
//                RCell_Max = Dev[i].Rmax;
//                nRCell_Max = ( (i-1) * 6 ) + Dev[i].R_Highest;
//            }
//            if( Dev[i].Rmin > 0.5 ){
//                if( RCell_Min > Dev[i].Rmin ){
//                    RCell_Min = Dev[i].Rmin;
//                    nRCell_Min = ( (i-1) * 6 ) + Dev[i].R_Lowest;
//                }
//            }
//
//            RCell_Avg += (unsigned int) (Dev[i].Rtot);
//
//            TOTAL_CELLS += Dev[i].CellsNum;
//
//            if( CRC_ctrl > 50000 )
//                CRC_ctrl = 0;
//
//            Dev[i].Vmin = 10000;
//            Dev[i].Rmin = 1000;
//            Dev[i].Vmax = 0;
//            Dev[i].Rmax = 0;
//            Dev[i].Rtot = 0;
//            if( Dev[i].Vcell[1] < 10 )
//                FAULT_SPI ++;  //////////////////////////////////////////////////////////////////////////////
//            for(k=0; k<6; k++)
//                Dev[i].Vcell[k] = 0;
//        }
//
//        if( (PLUGGED_DEV > 0) && ( TOTAL_CELLS > 0 ) ){
//            VCell_Avg = (VCell_Avg / (TOTAL_CELLS) );
//            VCell_Avg = VCell_Avg *100;
//
//            RCell_Avg = (RCell_Avg / (TOTAL_CELLS) );
//
//            if (TCellAvg_cont > 0){
//                TCell_Avg = (unsigned int) ((TCell_Avg / TCellAvg_cont )+0.5);
//                TCellAvg_cont = 0;
//            }
//
//
//        }
//
//    } //Fine ciclo devices-----------------------------------------------------------------------

  /*------------------------------- CONTROL TIMERS END -------------------------------*/





  /*++++++++++++++++++++++++++++++++ SOC COMPUTATION START ++++++++++++++++++++++++++++++++*/
//    if(CrntSense_Plg == 1)
//        avg += InstaCurr;
//    else
//        avg = 0;
//
//    if(Timer_1Sec == 1000)
//    {
//        if( Ah == -100 ) // Init Ah counter
//            Ah = (Ah_st_r*0.01);
//        AvgCurr = avg / 1000;
//
//        if(fabsf(AvgCurr) < CURRENT_CUT)
//            AvgCurr = 0;
//
//        avg = AvgCurr / 3600;  // [avg / (3600*1000)] => (avg / 3600000) [Ah]  Per avere media ORARIA
//
//        if( CELL_CHEMISTRY == 0)
//            Ah += avg;
//        if( CELL_CHEMISTRY == 1){
//            if( fabsf(AvgCurr) > 250 )
//                Ah = (Dev_Lowest * 0.001 * -0.361) + 1.249;
//            else
//                if( fabsf(AvgCurr) > 150 )
//                    Ah = (Dev_Lowest * 0.001 * -0.375) + 1.338;
//                else
//                    if( fabsf(AvgCurr) > 75 )
//                        Ah = (Dev_Lowest * 0.001 * -0.375) + 1.383;
//                    else
//                        if( fabsf(AvgCurr) > 20 )
//                            Ah = (Dev_Lowest * 0.001 * -0.375) + 1.413;
//                        else
//                            Ah = (Dev_Lowest * 0.001 * -0.385) + 1.485;
//        }
//        if( CELL_CHEMISTRY == 1)
//            Ah = (Cap_TOT*0.01) - Ah;
//
//
//        if( Ah > (Cap_TOT*0.01) )  //H-CAP EEPROM primi cicli carica
//            Ah = (Cap_TOT*0.01);
//        if( Ah < 0 )   //L-CAP EEPROM primi cicli carica
//            Ah = 0;
//
//        if( (fabsf(AvgCurr) < CURRENT_CUT) && (Dev_Highest >= SOCH)  ){
//            if( SOCT_cont == SOCT ){
//                Ah = (SOCH_Ah*0.1);
//                SOCT_cont = 0;
//            }else
//                SOCT_cont++;
//        }
//        if( (fabsf(AvgCurr) < CURRENT_CUT) && (Dev_Lowest <= SOCL)  ){
//            if( SOCT_cont == SOCT ){
//                Ah = (SOCL_Ah*0.1);
//                SOCT_cont = 0;
//            }else
//                SOCT_cont++;
//        }
//
//        Ah_st_w =(unsigned int) (Ah*100);
//        DataEEWrite(Ah_st_w, DEEaddr_Ah);
//        SOC = ( (Ah*100) / Cap_TOT ) * 100;  //SOC  [%Ah] : ( Ah / (Cap_TOT*1000) ) *100  || Ah=xx.x  Cap_Tot=xx.xx
//        if(avg < 0)
//            SOD_cont += (avg * -1);
//        if(SOD_cont >= 1){
//            SOD++;
//            DataEEWrite(SOD, DEEaddr_SOD);
//            SOD_cont = 0;
//        }
//
//
//        for(j=1; j <= PLUGGED_DEV; j++){
//            if( Dev[j].ReadCnt > 0 ){
//                if( Dev[j].Tcell_raw[0] < (200*Dev[j].ReadCnt) )
//                    Dev[j].Tcell[0] = (unsigned int) (Dev[j].Tcell_raw[0] / Dev[j].ReadCnt) +100;
//                else
//                    Dev[j].Tcell[0] = 100;
//
//                if( Dev[j].Tcell_raw[1] < (200*Dev[j].ReadCnt) )
//                    Dev[j].Tcell[1] = (unsigned int) (Dev[j].Tcell_raw[1] / Dev[j].ReadCnt) +100;
//                else
//                    Dev[j].Tcell[1] = 100;
//                if ( HARDWARE_V >= 3 ){
//                    if( Dev[j].Tcell_raw[2] < (200*Dev[j].ReadCnt) )
//                        Dev[j].Tcell[2] = (unsigned int) (Dev[j].Tcell_raw[2] / Dev[j].ReadCnt) +100;
//                    else
//                        Dev[j].Tcell[2] = 100;
//                }
//            }
//            Dev[j].Tcell_raw[0] = 0;
//            Dev[j].Tcell_raw[1] = 0;
//            if ( HARDWARE_V >= 3 )
//                Dev[j].Tcell_raw[2] = 0;
//            Dev[j].ReadCnt = 0;
//        }
//
//        avg = 0;
//        Timer_1Sec = 0;
//        Time++;
//        Dev_Lowest = 10000;
//        Dev_Highest = 0;
//        Temp_Highest = 0;
//        Temp_Lowest = 10000;
//        Empty_Cycle = 0;
//
//    }
    Timer_1Sec++;

  /*----------------------------- SOC COMPUTATION END -----------------------------*/
  /*-------------------------------- TEXAS BMS END --------------------------------*/




    /* reset Timer 2 interrupt flag */
    IFS0bits.T2IF = 0;
}
