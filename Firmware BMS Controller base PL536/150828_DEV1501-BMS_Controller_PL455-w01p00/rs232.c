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

#include "p33Fxxxx.h"
#include "pps.h"
#include "uart.h"
#include "spi.h"
#include "common.h"
#include "atox.h"
#include "EEPROM_Emulation_16bit.h"
#include <stdlib.h>

#define FCY 40000000
#define UART_BAUD 57600
#define BRGVAL ((FCY/UART_BAUD)/16)-1
#define PASSWORD 7530 //PASSWORD for the user interface
#define VECTOR_232 10


unsigned int line[300];
unsigned char temp_rx232[VECTOR_232];
unsigned char rx232[VECTOR_232];
unsigned int int232, hex232;
int menu=1, nav=-1, sig232;




void Init_rs232(void)
{
	unsigned int UxMODEval=0;
	unsigned int UxSTAval=0;
	unsigned int UxINTval=0;

	PPSUnLock;
	PPSLock;

	UxMODEval = UART_EN &  UART_IDLE_CON & UART_IrDA_DISABLE & UART_MODE_SIMPLEX &UART_UEN_00 & UART_DIS_WAKE &UART_DIS_LOOPBACK & UART_DIS_ABAUD &UART_BRGH_SIXTEEN & UART_NO_PAR_8BIT & UART_1STOPBIT;

	UxSTAval = UART_INT_TX_BUF_EMPTY  & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR & UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED;

	UxINTval = UART_RX_INT_EN & UART_RX_INT_PR4 & UART_TX_INT_DIS & UART_TX_INT_PR4;

	OpenUART2(UxMODEval, UxSTAval, BRGVAL);
	ConfigIntUART2(UxINTval);
        U2MODEbits.URXINV = 0;

	_U2RXIF=0;
	_U2RXIE=1;

}

int car_cont=0;
void __attribute__((interrupt,no_auto_psv)) _U2RXInterrupt(void)
{

    _U2RXIF=0;

    int i;

    if(car_cont >= VECTOR_232)
        car_cont = 0;

    temp_rx232[car_cont] = ReadUART2();

    if( temp_rx232[car_cont] == 0x1b){  //reset con ESC
        nav=0;
        car_cont=0;
        menu=0;
        for(i=0; i < VECTOR_232 ; i++)
            temp_rx232[i] = 0;

    }else{

        if( temp_rx232[car_cont] == 0x08){  //reset con backspace
            car_cont=0;
            for(i=0; i < VECTOR_232 ; i++)
                temp_rx232[i] = 0;
            putsUART2("\r\n: ");

        }else{

            if( (temp_rx232[car_cont] != 0x0d) && (car_cont < VECTOR_232) ){  // 0x0d = INVIO
                WriteUART2(temp_rx232[car_cont]);
                car_cont++;

            }else{

                int k=0;
                while(k < (VECTOR_232-1)){
                    rx232[k] = temp_rx232[k];
                    temp_rx232[k]=0;
                    k++;
                    int232 = atoi(rx232);
                    sig232 = atoi(rx232);
                    hex232 = atox(rx232);
                    if(menu==0)
                        menu=2;
                }
                car_cont=0;

            }
        }
    }   
}

unsigned int pssw_cnt=0;
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void)
{
    int dev232, regTI;
    unsigned int Offset_temp, int_temp;

    if(menu==1)  //se si sta navigando nei menù, altrimenti ins valore
        nav=int232;

    if( int232 == PASSWORD )    
        password++;

    password = 1;  //commentare quanto psw attiva

    if( password == 0 ){
        if( pssw_cnt >= 20 ){
            putsUART2("\r\n     **      Please insert Admin password      **\r\n");
            putsUART2("Support contact: carlo.campanaleATpodiumengineering.com \r\n");
            pssw_cnt = 0;
        }
        pssw_cnt++;
        if( pssw_cnt > 40)
            pssw_cnt = 0;
    }


    if( password > 0 )
    {
        switch(nav){

            case(0):
                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("********************************************************\r\n");
                putsUART2("*                  PODIUM ENGINEERING                  *\r\n");
                putsUART2("*            BMS Controller DEV_1401 r15p07            *\r\n");
                putsUART2("*                  Date: 08 Jun 2015                   *\r\n");
                putsUART2("*          Comment: VEHICLE BETA RELEASE               *\r\n");
                putsUART2("*     Mail: carlo.campanale@podiumengineering.com      *\r\n");
                putsUART2("********************************************************\r\n \r\n");
                sprintf (line, "                                 Activity Timer: %lu [s]\r\n", Time );
                putsUART2(line);
                sprintf (line, "                            SLEEP: %u \r\n", SLEEP );
                putsUART2(line);
                sprintf (line, "                            CONTACTORS: %u \r\n", CONTACT_RLY_N );
                putsUART2(line);
                sprintf (line, "                            PLUG DEV: %u \r\n", PLUGGED_DEV );
                putsUART2(line);
                sprintf (line, "                            POWER LOOP: %u \r\n", POWER_LOOP );
                putsUART2(line);
                sprintf (line, "                            INTERLOCK IN: %u \r\n", INTERLOCK_IN );
                putsUART2(line);
                sprintf (line, "                            SENSE_VBATT: %u \r\n", SENSE_VBATT );
                putsUART2(line);
                sprintf (line, "                            SENSE_VCHRG: %u \r\n", SENSE_VCHRG );
                putsUART2(line);
                sprintf (line, "                            SENSE_KEY: %u \r\n", SENSE_KEY );
                putsUART2(line);
                sprintf (line, "                            IMD_IN: %u \r\n", IMD_IN );
                putsUART2(line);
                sprintf (line, "Hard Errors: %u \r\n", HARD_ERROR );
                putsUART2(line);
                sprintf (line, "Hard Errors Code: %u \r\n", ERROR_Flag );
                putsUART2(line);
                putsUART2("Options List: \r\n\r\n");
                putsUART2(" 1) Cells Status \r\n");
                putsUART2(" 2) Balance Status \r\n");
                putsUART2(" 3) State Of Charge (SOC) \r\n");
                putsUART2(" 4) Balance Settings \r\n");
                putsUART2(" 5) BMS Settings \r\n");
                putsUART2(" 6) Charger \r\n");
                putsUART2("\r\n 9) Contactors \r\n");
                putsUART2("\r\n 0) Home Page \r\n");
                putsUART2("\r\n: ");

                menu=1;
                int232=0xffff;
            break;


            case(1):

                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("  Cells Voltage and Temperature: \r\n\r\n");
                sprintf (line, "Total Voltage: %u [V]\r\n\r\n", TOTAL_V/10 );
                putsUART2(line);
                sprintf (line, "Max Voltage: %u [mV]\r\n\r\n", Dev_Highest );
                putsUART2(line);
                sprintf (line, "Avg Voltage: %u [mV]\r\n\r\n", VCell_Avg );
                putsUART2(line);
                sprintf (line, "Min Voltage: %u [mV]\r\n\r\n", Dev_Lowest );
                putsUART2(line);
                sprintf (line, "Max Temp: %u [C]\r\n\r\n", Temp_Highest );
                putsUART2(line);
                sprintf (line, "Avg Temp: %u [C]\r\n\r\n", TCell_Avg );
                putsUART2(line);
                sprintf (line, "Min Temp: %u [C]\r\n\r\n", Temp_Lowest );
                putsUART2(line);
                sprintf (line, "Max Res.: %.2f [mOhm]\r\n\r\n", RCell_Max );
                putsUART2(line);
//                sprintf (line, "Avg Res.: %u [mOhm]\r\n\r\n", CAN_RCell_Avg );
//                putsUART2(line);
                sprintf (line, "Min Res.: %.2f [mOhm]\r\n\r\n", RCell_Min );
                putsUART2(line);

                sprintf (line, " Devices Plugged: %u  \r\n", PLUGGED_DEV);
                putsUART2(line);
                sprintf (line, " 11) Cells dev #1: %u  \r\n", Dev[1].CellsNum);
                putsUART2(line);
                sprintf (line, " 12) Cells dev #2: %u  \r\n", Dev[2].CellsNum);
                putsUART2(line);
                sprintf (line, " 13) Cells dev #3: %u  \r\n", Dev[3].CellsNum);
                putsUART2(line);
                sprintf (line, " 14) Cells dev #4: %u  \r\n", Dev[4].CellsNum);
                putsUART2(line);
                sprintf (line, " 15) Cells dev #5: %u  \r\n", Dev[5].CellsNum);
                putsUART2(line);
                sprintf (line, " 16) Cells dev #6: %u  \r\n", Dev[6].CellsNum);
                putsUART2(line);
                sprintf (line, " 17) Cells dev #7: %u  \r\n", Dev[7].CellsNum);
                putsUART2(line);
                sprintf (line, " 18) Cells dev #8: %u  \r\n", Dev[8].CellsNum);
                putsUART2(line);
                sprintf (line, " 19) Cells dev #9: %u  \r\n", Dev[9].CellsNum);
                putsUART2(line);
                sprintf (line, " 110) Cells dev #10: %u  \r\n \r\n \r\n and so on...", Dev[10].CellsNum);
                putsUART2(line);


                menu=1;
                int232=0xffff;

            break;

            case(11):

                dev232=1;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 3: %u \r\n", Dev[dev232].Tcell[2] - 100);
                    putsUART2(line);
                    sprintf (line, "Rmax: %.2f \r\n", Dev[dev232].Rmax);
                    putsUART2(line);
                    sprintf (line, "Rmin: %.2f \r\n", Dev[dev232].Rmin);
                    putsUART2(line);


                }

                menu=1;
            break;

            case(12):

                dev232=2;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 3: %u \r\n", Dev[dev232].Tcell[2] - 100);
                    putsUART2(line);
                }

                menu=1;
            break;

            case(13):

                dev232=3;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 3: %u \r\n", Dev[dev232].Tcell[2] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(14):

                dev232=4;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 3: %u \r\n", Dev[dev232].Tcell[2] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(15):

                dev232=5;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(16):

                dev232=6;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(17):

                dev232=7;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(18):

                dev232=8;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(19):

                dev232=9;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(110):

                dev232=10;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(111):

                dev232=11;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(112):

                dev232=12;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(113):

                dev232=13;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(114):

                dev232=14;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(115):

                dev232=15;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(116):

                dev232=16;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(117):

                dev232=17;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(118):

                dev232=18;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

            case(119):

                dev232=19;

                if((Dev[dev232].Vcell[0]) > 0){

                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
                    putsUART2(line);

                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
                    putsUART2(line);
                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
                    putsUART2(line);
                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
                    putsUART2(line);
                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
                    putsUART2(line);

                }
                menu=1;
            break;

//            case(120):
//
//                dev232=20;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(121):
//
//                dev232=21;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(122):
//
//                dev232=22;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(123):
//
//                dev232=23;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(124):
//
//                dev232=24;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(125):
//
//                dev232=25;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(126):
//
//                dev232=26;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(127):
//
//                dev232=27;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(128):
//
//                dev232=28;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(129):
//
//                dev232=29;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(130):
//
//                dev232=30;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(131):
//
//                dev232=31;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(132):
//
//                dev232=32;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//
//            case(133):
//
//                dev232=33;
//
//                if((Dev[dev232].Vcell[0]) > 0){
//
//                    putcUART2(0x0c);   // 0x0c = NEW PAGE
//                    sprintf (line, "  Cells Voltage Bank #%d: \r\n\r\n", dev232);
//                    putsUART2(line);
//
//                    sprintf (line, "#1: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[0], Dev[dev232].Rcell[0], (Dev[dev232].BalCtrlRead & (0x01<<0)) ? 1 : 0, (Dev[dev232].COV & (0x01<<0)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<0)) ? 1 : 0 );
//                    putsUART2(line);
//                    sprintf (line, "#2: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[1], Dev[dev232].Rcell[1], (Dev[dev232].BalCtrlRead & (0x01<<1)) ? 1 : 0, (Dev[dev232].COV & (0x01<<1)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<1)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#3: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[2], Dev[dev232].Rcell[2], (Dev[dev232].BalCtrlRead & (0x01<<2)) ? 1 : 0, (Dev[dev232].COV & (0x01<<2)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<2)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#4: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[3], Dev[dev232].Rcell[3], (Dev[dev232].BalCtrlRead & (0x01<<3)) ? 1 : 0, (Dev[dev232].COV & (0x01<<3)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<3)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#5: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n", Dev[dev232].Vcell[4], Dev[dev232].Rcell[4], (Dev[dev232].BalCtrlRead & (0x01<<4)) ? 1 : 0, (Dev[dev232].COV & (0x01<<4)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<4)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "#6: %u [mV] || R: %.2f [mOhm] ||Bl:%d||OV:%d||UV:%d|| \r\n\r\n", Dev[dev232].Vcell[5], Dev[dev232].Rcell[5], (Dev[dev232].BalCtrlRead & (0x01<<5)) ? 1 : 0, (Dev[dev232].COV & (0x01<<5)) ? 1 : 0, (Dev[dev232].CUV & (0x01<<5)) ? 1 : 0  );
//                    putsUART2(line);
//                    sprintf (line, "Temp 1: %u \r\n", Dev[dev232].Tcell[0] - 100);
//                    putsUART2(line);
//                    sprintf (line, "Temp 2: %u \r\n", Dev[dev232].Tcell[1] - 100);
//                    putsUART2(line);
//
//                }
//                menu=1;
//            break;
//



            case(2):

                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("  Balance Status: \r\n\r\n");

                sprintf (line, "SPI Fault: %u \r\n", FAULT_SPI);
                putsUART2(line);
                sprintf (line, "CRC Status: %u \r\n\r\n", CRC_ctrl);
                putsUART2(line);
                sprintf (line, "Water Leak Sns: %f [V]\r\n\r\n", WaterLeak_AN);
                putsUART2(line);
                sprintf (line, "Status Bank #1: 0x%x \r\n", Dev[1].Status);
                putsUART2(line);
                sprintf (line, "Alarm Bank #1: 0x%x \r\n", Dev[1].Alarm);
                putsUART2(line);
                sprintf (line, "Fault Bank #1: 0x%x \r\n\r\n", Dev[1].Fault);
                putsUART2(line);
                sprintf (line, "IO Read Bank #1: 0x%x \r\n", Dev[1].IORead);
                putsUART2(line);
                sprintf (line, "Sleep cont: %u \r\n\r\n", SLEEP_cont);
                putsUART2(line);
                sprintf (line, "IO Read Bank #16: 0x%x \r\n\r\n", Dev[16].IORead);
                putsUART2(line);
                sprintf (line, "Status Bank #2: 0x%x \r\n", Dev[2].Status);
                putsUART2(line);
                sprintf (line, "Alarm Bank #2: 0x%x \r\n", Dev[2].Alarm);
                putsUART2(line);
                sprintf (line, "Fault Bank #2: 0x%x \r\n\r\n", Dev[2].Fault);
                putsUART2(line);
                sprintf (line, "Status Bank #16: 0x%x \r\n", Dev[16].Status);
                putsUART2(line);
                sprintf (line, "Alarm Bank #16: 0x%x \r\n", Dev[16].Alarm);
                putsUART2(line);
                sprintf (line, "Fault Bank #16: 0x%x \r\n\r\n", Dev[16].Fault);
                putsUART2(line);
                sprintf (line, "Status Bank #25: 0x%x \r\n", Dev[25].Status);
                putsUART2(line);
                sprintf (line, "Alarm Bank #25: 0x%x \r\n", Dev[25].Alarm);
                putsUART2(line);
                sprintf (line, "Fault Bank #25: 0x%x \r\n\r\n", Dev[25].Fault);
                putsUART2(line);
                sprintf (line, "Status Bank #32: 0x%x \r\n", Dev[32].Status);
                putsUART2(line);
                sprintf (line, "Alarm Bank #32: 0x%x \r\n", Dev[32].Alarm);
                putsUART2(line);
                sprintf (line, "Fault Bank #32: 0x%x \r\n\r\n", Dev[32].Fault);
                putsUART2(line);

                menu=1;

            break;


            case(3):

                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("  State Of Charge (SOC): \r\n\r\n");

                sprintf (line, "Total Capacity: %f [Ah] \r\n\r\n", (float) Cap_TOT/100);
                putsUART2(line);
                sprintf (line, "Instant Current: %f [A] \r\n", (AvgCurr * -1));
                putsUART2(line);
                sprintf (line, "Accumulated Energy: %f [Ah] \r\n", Ah);
                putsUART2(line);
                sprintf (line, "SOC: %f %% \r\n", SOC);
                putsUART2(line);
                sprintf (line, "SOD: %u [Ah] \r\n\r\n", SOD);
                putsUART2(line);
                sprintf (line, " Current Req.: %u [100mA]\r\n", Charger_Idc);
                putsUART2(line);
                sprintf (line, " Current Req.: %f [100mA]\r\n", CHRG_Itmp);
                putsUART2(line);
                sprintf (line, "Max Voltage: %u [mV]\r\n", Dev_Highest );
                putsUART2(line);
                sprintf (line, "Max Temp: %u [C]\r\n", Temp_Highest );
                putsUART2(line);
                sprintf (line, "IMD Resistance: %u [kOhm]\r\n", IMD_Resistance );
                putsUART2(line);
                sprintf (line, "Water Leak Sensor: %f [V]\r\n", WaterLeak_AN );
                putsUART2(line);

                menu=1;

            break;


            case(4):

                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("  Balance Settings: \r\n\r\n");

                sprintf (line, " 41) Enable: %u \r\n", BALANCE_EN);
                putsUART2(line);
                sprintf (line, " 42) Sleep: %u \r\n", SLEEP);
                putsUART2(line);
                sprintf (line, " 43) Balance floor: %u [mV] \r\n", VMIN_BAL);
                putsUART2(line);
                sprintf (line, " 44) DELTA: %u [mV] \r\n", DELTA);
                putsUART2(line);
                sprintf (line, " 45) MAX Balance Temperature: %u [C] \r\n\r\n", MAX_BAL_Tmp);
                putsUART2(line);
                sprintf (line, " 46) Force Balance Enable: %u \r\n", FORCE_BALANCE_EN);
                putsUART2(line);
                sprintf (line, " 47) Force LEDs Enable: %u \r\n", FORCE_LEDS_EN);
                putsUART2(line);

                int232=0xffff;
                menu=1;

            break;

            case(41):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Balance Enable (0 OFF, 1 ON): \r\n\r\n");
                }
                if(menu==2){
                    if(int232<2)
                        BALANCE_EN = int232;
                    int232=4;
                    menu=1;
                }else
                    menu=0;

            break;

            case(42):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Sleep (0 OFF, 1 ON): \r\n\r\n");
                }
                if(menu==2){
                    if(int232<2){
                        SLEEP = int232;
                       // DataEEWrite(SLEEP, DEEaddr_SLEEP);
                        if(SLEEP==1)
                            Sleep_SET();
                    }
                    int232=4;
                    menu=1;
                }else
                    menu=0;

            break;

            case(43):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Balance floor [mV]: \r\n\r\n");
                }
                if((menu==2) && (int232 >1000) && (int232 <10000)){
                    VMIN_BAL = int232;
                    DataEEWrite(VMIN_BAL, DEEaddr_VbMIN);
                    int232=4;
                    menu=1;
                }else
                    menu=0;

            break;

            case(44):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  DELTA [mV]: \r\n\r\n");
                }
                if((menu==2) && (int232 >14) && (int232 <10000)){
                    DELTA = int232;
                    DataEEWrite(DELTA, DEEaddr_DELTA);
                    int232=4;
                    menu=1;
                }else
                    menu=0;

            break;

            case(45):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  MAX Balance Temperature [C]: \r\n\r\n");
                }
                if((menu==2) && (int232 > 20) && (int232 <200)){
                    MAX_BAL_Tmp = int232;
                    DataEEWrite(MAX_BAL_Tmp, DEEaddr_BALTemp);
                    int232=4;
                    menu=1;
                }else
                    menu=0;

            break;

            case(46):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Force Balance Enable (0 OFF, 1 ON): \r\n\r\n");
                }
                if(menu==2){
                    if(int232<2)
                        FORCE_BALANCE_EN = int232;
                    int232=4;
                    menu=1;
                }else
                    menu=0;

            break;

            case(47):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Force LEDs Enable (0 OFF, 1 ON): \r\n\r\n");
                }
                if(menu==2){
                    if(int232<2)
                        FORCE_LEDS_EN = int232;
                    int232=4;
                    menu=1;
                }else
                    menu=0;

            break;



            case(5):

                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("  BMS Settings: \r\n\r\n");

                sprintf (line, " 51) CAN Base ID: 0x%x \r\n", CAN_BASE_ID);
                putsUART2(line);
                sprintf (line, " 52) Total Capacity: %f [Ah] \r\n",(float) Cap_TOT/100);
                putsUART2(line);
                sprintf (line, " 53) Over Voltage: %u [mV] \r\n", Over_V);
                putsUART2(line);
                sprintf (line, " 54) Under Voltage: %u [mV] \r\n", Under_V);
                putsUART2(line);
                sprintf (line, " 55) Fault Reset \r\n");
                putsUART2(line);
                sprintf (line, " 56) Ah Calibration \r\n");
                putsUART2(line);
                sprintf (line, " 57) Current Gain Calibration:  %f  \r\n", Curr_Gain);
                putsUART2(line);
                sprintf (line, " 58) Current Offset Calibration:  %+f  \r\n", (Curr_Offset*-1) );
                putsUART2(line);
                sprintf (line, " 59) SOC High Voltage:  %u  [mV]\r\n", SOCH );
                putsUART2(line);
                sprintf (line, " 510) SOC High Capacity:  %f  [Ah]\r\n", SOCH_Ah*0.1 );
                putsUART2(line);
                sprintf (line, " 511) SOC Low Voltage:  %u  [mV] \r\n", SOCL );
                putsUART2(line);
                sprintf (line, " 512) SOC Low Capacity:  %f  [Ah]\r\n", SOCL_Ah*0.1 );
                putsUART2(line);
                sprintf (line, " 513) SOC Timer:  %u  [s]\r\n", SOCT );
                putsUART2(line);
                sprintf (line, " 514) Delta RV:  %u  [mV]\r\n", DeltaRV );
                putsUART2(line);
                sprintf (line, " 515) Delta RI:  %u  [A]\r\n", DeltaRI );
                putsUART2(line);
                sprintf (line, " 516) Power Loop State \r\n");
                putsUART2(line);
                sprintf (line, " 517) FANs Temperature: %u \r\n", FANS_Temperature);
                putsUART2(line);
                sprintf (line, " 517) FANs Enable: %u \r\n", FAN_CMD);
                putsUART2(line);

                int232=0xffff;
                menu=1;

            break;

            case(51):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  CAN Base ID: \r\n\r\n");
                }
                if((menu==2) && (int232 >10) && (int232 <10000)){
                    CAN_BASE_ID = hex232;
                    DataEEWrite(CAN_BASE_ID, DEEaddr_CAN);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(52):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Total Capacity [Ah x100]: \r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <1000)){
                    Cap_TOT = int232;
                    DataEEWrite(Cap_TOT, DEEaddr_Cap);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(53):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Over Voltage [mV]: \r\n\r\n");
                }
                if((menu==2) && (int232 >2000) && (int232 <10000)){
                    Over_V = int232;
                    DataEEWrite(Over_V, DEEaddr_OverV);
                    asm ("RESET");
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(54):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Under Voltage [mV]: \r\n\r\n");
                }
                if((menu==2) && (int232 >0)  && (int232 <5000)){
                    Under_V = int232;
                    DataEEWrite(Under_V, DEEaddr_UnderV);
                    asm ("RESET");
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(55):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Fault Reset (0 OFF, 1 RESET): \r\n\r\n");
                }
                if(menu==2){
                    if(int232 == 1){
                        asm ("RESET");
                    }
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(56):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Ah Value: [Ah x 10]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <1000)){
                    Ah = int232;
                    Ah *= 0.1;
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(57):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Gain Value: [x100]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <10000)){
                    Curr_Gain = int232;
                    DataEEWrite(Curr_Gain, DEEaddr_CGain);
                    Curr_Gain *= 0.01;
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(58):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Offset Value: [x100]\r\n\r\n");
                }
                if(menu==2){
                    Curr_Offset = sig232;
                    Offset_temp = (unsigned int) (sig232 + 10000);
                    DataEEWrite( Offset_temp , DEEaddr_COffset);
                    Curr_Offset *= (-0.01);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(59):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  SOC High Voltage: [mV]\r\n\r\n");
                }
                if((menu==2) && (int232 >2000) && (int232 <10000)){
                    SOCH = int232;
                    DataEEWrite(SOCH, DEEaddr_SOCH);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(510):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  SOC High Capacity: [Ah x10]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <1000)){
                    SOCH_Ah = int232;
                    int_temp = (unsigned int) SOCH_Ah;
                    DataEEWrite(int_temp, DEEaddr_SOCH_Ah);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(511):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  SOC Low Voltage: [mV]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <10000)){
                    SOCL = int232;
                    DataEEWrite(SOCL, DEEaddr_SOCL);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(512):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  SOC Low Capacity: [Ah x10]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <1000)){
                    SOCL_Ah = int232;
                    int_temp = (unsigned int) SOCL_Ah;
                    DataEEWrite(int_temp, DEEaddr_SOCL_Ah);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(513):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  SOC Timer: [s]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <100)){
                    SOCT = int232;
                    DataEEWrite(SOCT, DEEaddr_SOCT);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(514):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Delta RV: [mV]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <1000)){
                    DeltaRV = int232;
                    DataEEWrite(DeltaRV, DEEaddr_DeltaRV);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(515):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Delta RI: [A]\r\n\r\n");
                }
                if((menu==2) && (int232 >0) && (int232 <1000)){
                    DeltaRI = int232;
                    DataEEWrite(DeltaRI, DEEaddr_DeltaRI);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(516):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Power Loop: \r\n\r\n");
                }
                if((menu==2) && (int232 >=0) && (int232 <=1)){
                    POWER_LOOP = int232;
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(517):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  FANs Temperature: [C]\r\n\r\n");
                }
                if((menu==2) && (int232 >10) && (int232 <100)){
                    FANS_Temperature = int232;
                    DataEEWrite(FANS_Temperature, DEEaddr_FANsTmp);
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;

            case(518):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  FANs Enable: \r\n\r\n");
                }
                if((menu==2)){
                    FAN_CMD = int232;
                    int232=5;
                    menu=1;
                }else
                    menu=0;

            break;



            case(6):

                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("  Charger Settings: \r\n\r\n");



                sprintf (line, " 61) Brusa Charger ENABLE: %u \r\n", CHARGER_EN);
                putsUART2(line);
                sprintf (line, " 62) Max Inlet AC Current: %u [100mA]\r\n", MAX_Iac);
                putsUART2(line);
                sprintf (line, " 63) Max DC OUT Voltage: %u [100mV]\r\n", MAX_Vdc);
                putsUART2(line);
                sprintf (line, " 64) Max DC OUT Current: %u [100mA]\r\n", MAX_Idc);
                putsUART2(line);
                sprintf (line, " 65) Max Char. Temperature: %u [°C]\r\n", MAX_Tmp);
                putsUART2(line);


                int232=0xffff;
                menu=1;

            break;

            case(61):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Brusa Charger ENABLE (0 OFF, 1 ON): \r\n\r\n");
                }
                if(menu==2){
                    if(int232<200)
                        CHARGER_EN = int232;
                    int232=6;
                    menu=1;
                }else
                    menu=0;

            break;

            case(62):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Max Inlet AC Current: \r\n\r\n");
                }
                if(menu==2){
                    if(int232>0){
                        MAX_Iac = int232;
                        DataEEWrite(MAX_Iac, DEEaddr_CHRGIac);
                    }
                    int232=6;
                    menu=1;
                }else
                    menu=0;

            break;

            case(63):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Max DC OUT Voltage: \r\n\r\n");
                }
                if(menu==2){
                    if(int232>0){
                        MAX_Vdc = int232;
                        DataEEWrite(MAX_Vdc, DEEaddr_CHRGVdc);
                    }
                    int232=6;
                    menu=1;
                }else
                    menu=0;

            break;

            case(64):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Max DC OUT Current: \r\n\r\n");
                }
                if(menu==2){
                    if(int232>0){
                        MAX_Idc = int232;
                        DataEEWrite(MAX_Idc, DEEaddr_CHRGIdc);
                    }int232=6;
                    menu=1;
                }else
                    menu=0;

            break;

            case(65):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Max Charg. Temperature: \r\n\r\n");
                }
                if(menu==2){
                    if(int232>0){
                        MAX_Tmp = int232;
                        DataEEWrite(MAX_Tmp, DEEaddr_CHRGTmp);
                    }int232=6;
                    menu=1;
                }else
                    menu=0;

            break;



            case(9):

                putcUART2(0x0c);   // 0x0c = NEW PAGE
                putsUART2("  Contactors Settings: \r\n\r\n");

                sprintf (line, " 91) Enable: %u \r\n", CONTACT_EN);
                putsUART2(line);
                sprintf (line, " 92) Precharge Time: %u [s]\r\n", PRECHR_TM);
                putsUART2(line);
                sprintf (line, " 93) Discharge Relay: %u \r\n", DISCHARGE_RLY);
                putsUART2(line);
                sprintf (line, " 94) Discharge Time: %u [s]\r\n", DISCHR_TM);
                putsUART2(line);

                int232=0xffff;
                menu=1;

            break;

            case(91):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Contactors Enable (0 OFF, 1 ON): \r\n\r\n");
                }
                if(menu==2){
                    if(int232 == 1)
                        CONTACT_EN = 1;
                    if(int232 == 0)
                        CONTACT_EN = 0;
                    int232=9;
                    menu=1;
                }else
                    menu=0;

            break;

            case(92):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Precharge Time [s]: \r\n\r\n");
                }
                if(menu==2){
                    if(int232<10){
                        PRECHR_TM = int232;
                        DataEEWrite(PRECHR_TM, DEEaddr_PrecTM);
                    }
                    int232=9;
                    menu=1;
                }else
                    menu=0;

            break;

            case(93):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Discharge Relay: \r\n\r\n");
                }
                if(menu==2){
                    if(int232<10){
                        DISCHARGE_RLY = int232;
                    }
                    int232=9;
                    menu=1;
                }else
                    menu=0;

            break;
            
            case(94):

                if(menu==1){
                    putcUART2(0x0c);   // 0x0c = NEW PAGE
                    putsUART2("  Discharge Time [s]: \r\n\r\n");
                }
                if(menu==2){
                    if(int232<100){
                        DISCHR_TM = int232;
                        DataEEWrite(DISCHR_TM, DEEaddr_DiscTM);
                    }
                    int232=9;
                    menu=1;
                }else
                    menu=0;

            break;




        } // ----------------------------- SWITCH END -----------------------------
    }// ------------------------------- PASSWORD END --------------------------

    IFS1bits.T5IF=0;	 			// Clear Timer 5 Interrupt Flag
}

