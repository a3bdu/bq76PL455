
/*********************************************************************
* File:        init.c
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r00
*
* Created on 3 Marzo 2014, 15.35
*********************************************************************/

#include "p33fxxxx.h"
#include "init.h"
#include "EEPROM_Emulation_16bit.h"
#include "common.h"

//_FOSCSEL(FNOSC_PRIPLL & IESO_OFF); // Primary oscillator (XT, HS, EC) con PLL
//_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_XT); // Clock Switching e Fail Safe Clock Monitor disabilitati
//// Funzione del pin OSC2: OSC2 è usato dal quarzo
//// Primary Oscillator Mode: Oscillatore media velocità XT
//_FWDT(FWDTEN_OFF); // Watchdog Timer disabled

#include <xc.h>

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash  Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Mode (Primary Oscillator (XT, HS, EC) w/ PLL)
#pragma config IESO = OFF               // Internal External Switch Over Mode (Start-up device with user-selected oscillator source)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Source (XT Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT   
#pragma config WDTPOST = PS1024          // Watchdog Timer Postscaler (1:1024)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = ON             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR1           // POR Timer Value (disabled)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)



void InitOsc()
{

    //Fin = 8Mhz |||| Fosc= Fin { (PLLDIV+2) / [ (PLLPRE+2) * 2*(PLLPOST+1) ] } = 80Mhz |||| Fcy = Fosc/2 = 40MHz
    _PLLPRE = 1; // Prescaler del PLL, (il valore da settare al registro è inferire di 2 rispetto al valore di divisione)
    _PLLDIV = 58; // Divisore del PLL, (il valore da settare al registro è inferire di 2 rispetto al valore di divisione)
    _PLLPOST = 0; // Postscaler del PLL
    while(!OSCCONbits.LOCK); // Attendi che il PLL sia agganciato

}




void InitPin( void )
{

    //	PIN DIRECTION CONFIG (0 Output, 1 Input)
    TRISBbits.TRISB0 =  0x0;			//Set RB0 as  ()
    TRISBbits.TRISB1 =  0x1;			//Set RB1 as Input () Vref-
    TRISBbits.TRISB2 =  0x1;			//Set RB2 as Input () AN2
    TRISBbits.TRISB3 =  0x1;			//Set RB3 as Input () AN3
    TRISBbits.TRISB4 =  0x1;			//Set RB4 as Input () AN4
    TRISBbits.TRISB5 =  0x1;			//Set RB5 as Input () AN5
    TRISBbits.TRISB6 =  0x1;			//Set RB6 as Input () PGEC
    TRISBbits.TRISB7 =  0x1;			//Set RB7 as Input () PGED
    TRISBbits.TRISB8 =  0x0;			//Set RB8 as  ()
    TRISBbits.TRISB9 =  0x1;			//Set RB9 as Input () AN9
    TRISBbits.TRISB10 = 0x0;			//Set RB10 as  ()
    TRISBbits.TRISB11 = 0x0;			//Set RB11 as  ()
    TRISBbits.TRISB12 = 0x0;			//Set RB12 as  ()
    TRISBbits.TRISB13 = 0x0;			//Set RB13 as  ()
    TRISBbits.TRISB14 = 0x0;			//Set RB14 as  ()
    TRISBbits.TRISB15 = 0x0;			//Set RB15 as ()

    TRISCbits.TRISC1 =  0x0;			//Set RC1 as  ()
    TRISCbits.TRISC2 =  0x0;			//Set RC2 as  ()
    TRISCbits.TRISC12 = 0x1;			//Set RC12 as Input () OSC1
    TRISCbits.TRISC13 = 0x0;			//Set RC13 as  ()
    TRISCbits.TRISC14 = 0x0;			//Set RC14 as  ()
    TRISCbits.TRISC15 = 0x1;			//Set RC15 as Input () OSC2

    TRISDbits.TRISD0 =  0x1;			//Set RD0 as Input ()
    TRISDbits.TRISD1 =  0x1;			//Set RD1 as Input ()
    TRISDbits.TRISD2 =  0x1;			//Set RD2 as Input () I/O
    TRISDbits.TRISD3 =  0x1;			//Set RD3 as Input () I/O
    TRISDbits.TRISD4 =  0x1;			//Set RD4 as Input () I/O
    TRISDbits.TRISD5 =  0x1;			//Set RD5 as Input () I/O
    TRISDbits.TRISD6 =  0x0;			//Set RD6 as Output () I/O
    TRISDbits.TRISD7 =  0x0;			//Set RD7 as Output () I/O
    TRISDbits.TRISD8 =  0x0;			//Set RD8 as Output () DOUT
    TRISDbits.TRISD9 =  0x0;			//Set RD9 as Output () DOUT
    TRISDbits.TRISD10 = 0x0;			//Set RD10 as Output () DOUT
    TRISDbits.TRISD11 = 0x0;			//Set RD11 as Output () DOUT

    TRISFbits.TRISF0 =  0x1;			//Set RF0 as Input ()  C1RX     ->ECAN
    TRISFbits.TRISF1 =  0x0;			//Set RF1 as Output () C1TX     ->ECAN
    TRISFbits.TRISF2 =  0x0;			//Set RF2 as  ()
    TRISFbits.TRISF3 =  0x0;			//Set RF3 as  ()
    TRISFbits.TRISF4 =  0x1;			//Set RF4 as Input () U2RX      ->UART2
    TRISFbits.TRISF5 =  0x0;			//Set RF5 as Output () U2TX     ->UART2
    TRISFbits.TRISF6 =  0x0;			//Set RF6 as  ()

    TRISGbits.TRISG0 =  0x1;			//Set RG0 as Input () C2RX      ->ECAN2
    TRISGbits.TRISG1 =  0x0;			//Set RG1 as Output () C2TX     ->ECAN2
    TRISGbits.TRISG2 =  0x1;			//Set RG2 as Input () SCL1 I2C
    TRISGbits.TRISG3 =  0x1;			//Set RG3 as Input () SDA1 I2C
    TRISGbits.TRISG6 =  0x0;			//Set RG6 as Output () SCK2 SPI
    TRISGbits.TRISG7 =  0x1;			//Set RG7 as Input () SDI2 SPI  ->SPI2
    TRISGbits.TRISG8 =  0x0;			//Set RG8 as Output () SDO2 SPI ->SPI2
    TRISGbits.TRISG9 =  0x0;			//Set RG9 as Output () SCS2 SPI ->dovrebbe essere SS2
    TRISGbits.TRISG12 = 0x0;			//Set RG12 as Output () LATCH_3.3v
    TRISGbits.TRISG13 = 0x0;			//Set RG13 as Output () LED3
    TRISGbits.TRISG14 = 0x0;			//Set RG14 as Input () I/O
    TRISGbits.TRISG15 = 0x0;			//Set RG15 as ()

    //	OUTPUT TYPE CONFIG
    ODCDbits.ODCD0 = 0x0;			//Set RD0 as Digital
    ODCDbits.ODCD1 = 0x0;			//Set RD1 as Digital
    ODCDbits.ODCD2 = 0x0;			//Set RD2 as Digital
    ODCDbits.ODCD3 = 0x0;			//Set RD3 as Digital
    ODCDbits.ODCD4 = 0x0;			//Set RD4 as Digital
    ODCDbits.ODCD5 = 0x0;			//Set RD5 as Digital
    ODCDbits.ODCD6 = 0x0;			//Set RD6 as Digital
    ODCDbits.ODCD7 = 0x0;			//Set RD7 as Digital
    ODCDbits.ODCD8 = 0x0;			//Set RD8 as Digital
    ODCDbits.ODCD9 = 0x0;			//Set RD9 as Digital
    ODCDbits.ODCD10 = 0x0;			//Set RD10 as Digital
    ODCDbits.ODCD11 = 0x0;			//Set RD11 as Digital

    ODCFbits.ODCF0 = 0x0;			//Set RF0 as Digital
    ODCFbits.ODCF1 = 0x0;			//Set RF1 as Digital
    ODCFbits.ODCF2 = 0x0;			//Set RF2 as Digital
    ODCFbits.ODCF3 = 0x0;			//Set RF3 as Digital
    ODCFbits.ODCF4 = 0x0;			//Set RF4 as Digital
    ODCFbits.ODCF5 = 0x0;			//Set RF5 as Digital

    ODCGbits.ODCG0 = 0x0;			//Set RG0 as Digital
    ODCGbits.ODCG1 = 0x0;			//Set RG1 as Digital
    ODCGbits.ODCG2 = 0x0;			//Set RG2 as Digital
    ODCGbits.ODCG3 = 0x0;			//Set RG3 as Digital
    ODCGbits.ODCG6 = 0x0;			//Set RG6 as Digital
    ODCGbits.ODCG7 = 0x0;			//Set RG7 as Digital
    ODCGbits.ODCG8 = 0x0;			//Set RG8 as Digital
    ODCGbits.ODCG9 = 0x0;			//Set RG9 as Digital
    ODCGbits.ODCG12 = 0x0;			//Set RG12 as Digital
    ODCGbits.ODCG13 = 0x0;			//Set RG13 as Digital
    ODCGbits.ODCG14 = 0x0;			//Set RG14 as Digital
    ODCGbits.ODCG15 = 0x0;			//Set RG15 as Digital

    //	ANALOG 1 INPUT CONFIG (0 Analogic, 1 Digital)
    AD1PCFGLbits.PCFG0 = 0x1; 		//Set AN0 as Digital Input
    AD1PCFGLbits.PCFG1 = 0x1; 		//Set AN1 as Digital Input
    AD1PCFGLbits.PCFG2 = 0x0; 		//Set AN2 as Analog Input
    AD1PCFGLbits.PCFG3 = 0x0; 		//Set AN3 as Analog Input
    AD1PCFGLbits.PCFG4 = 0x0; 		//Set AN4 as Analog Input
    AD1PCFGLbits.PCFG5 = 0x0; 		//Set AN5 as Analog Input
    AD1PCFGLbits.PCFG9 = 0x0; 		//Set AN9 as Analog Input
    AD1PCFGLbits.PCFG10 = 0x1; 		//Set AN10 as Digital Input
    AD1PCFGLbits.PCFG11 = 0x1; 		//Set AN11 as Digital Input
    AD1PCFGLbits.PCFG12 = 0x1; 		//Set AN12 as Digital Input
    AD1PCFGLbits.PCFG13 = 0x1; 		//Set AN13 as Digital Input
    AD1PCFGLbits.PCFG14 = 0x1; 		//Set AN14 as Digital Input
    AD1PCFGLbits.PCFG15 = 0x1; 		//Set AN15 as Digital Input
    AD1PCFGHbits.PCFG16 = 0x1; 		//Set AN16 as Digital Input
    AD1PCFGHbits.PCFG17 = 0x1; 		//Set AN17 as Digital Input
    
    AD2PCFGL=0xFFFF;     //ANALOG 2 INPUT CONFIG - DISABLED: ALL DIGITAL INPUT


    // INTERRUPT INITIALIZATION

    IFS0 = 0x00;
    IFS1 = 0x00;
    IFS2 = 0x00;
    IFS3 = 0x00;
    IFS4 = 0x00;
    IEC0 = 0x00;
    IEC1 = 0x00;
    IEC2 = 0x00;
    IEC3 = 0x00;
    IEC4 = 0x00;

}




void InitEEPROM_Var(){

            //  Init EEPROM Variables
        Cap_TOT = DataEERead(DEEaddr_Cap);
        if( Cap_TOT > 65500 )  //RESET EEPROM prima programmazione
            Cap_TOT = 100;

        Under_V = DataEERead(DEEaddr_UnderV);
        if( Under_V > 65500 )  //RESET EEPROM prima programmazione
            Under_V = 1500;

        Over_V = DataEERead(DEEaddr_OverV);
        if( Over_V > 65500 )  //RESET EEPROM prima programmazione
            Over_V = 4500;

        VMIN_BAL = DataEERead(DEEaddr_VbMIN);
        if( VMIN_BAL > 65500 )  //RESET EEPROM prima programmazione
            VMIN_BAL = 2500;

        DELTA = DataEERead(DEEaddr_DELTA);
        if( DELTA > 65500 )  //RESET EEPROM prima programmazione
            DELTA = 100;

        CAN_BASE_ID = DataEERead(DEEaddr_CAN);
        if( CAN_BASE_ID > 65500 )  //RESET EEPROM prima programmazione
            CAN_BASE_ID = 0x130;   //BENCH 0x130 - VEHICLE 0x310

        Curr_Gain = DataEERead(DEEaddr_CGain);
        if( Curr_Gain > 65500 )  //RESET EEPROM prima programmazione
            Curr_Gain = 1;
        else
            Curr_Gain *= 0.01;

        Curr_Offset = DataEERead(DEEaddr_COffset);
        if( Curr_Offset > 65500 )  //RESET EEPROM prima programmazione
            Curr_Offset = 0;
        else{
            Curr_Offset -= 10000;
            Curr_Offset *= (-0.01);
        }

        SOCH = DataEERead(DEEaddr_SOCH);
        if( SOCH > 65500 )  //RESET EEPROM prima programmazione
            SOCH = 4200;

        SOCL = DataEERead(DEEaddr_SOCL);
        if( SOCL > 65500 )  //RESET EEPROM prima programmazione
            SOCL = 2500;

        SOCH_Ah = DataEERead(DEEaddr_SOCH_Ah);
        if( SOCH_Ah > 65500 )  //RESET EEPROM prima programmazione
            SOCH_Ah = 80;

        SOCL_Ah = DataEERead(DEEaddr_SOCL_Ah);
        if( SOCL_Ah > 65500 )  //RESET EEPROM prima programmazione
            SOCL_Ah = 10;

        SOCT = DataEERead(DEEaddr_SOCT);
        if( SOCT > 65500 )  //RESET EEPROM prima programmazione
            SOCT = 10;

        SLEEP = DataEERead(DEEaddr_SLEEP);
        if( SLEEP > 65500 )  //RESET EEPROM prima programmazione
            SLEEP = 0;

        Ah_st_r = DataEERead(DEEaddr_Ah);
        if( Ah_st_r > 65500 )  //RESET EEPROM prima programmazione
                Ah_st_r = 0;

        SOD = DataEERead(DEEaddr_SOD);                                          //Sleep of Death
        if( SOD > 65500 )  //RESET EEPROM prima programmazione
            SOD = 0;

        PRECHR_TM = DataEERead(DEEaddr_PrecTM);
        if( PRECHR_TM > 65500 )  //RESET EEPROM prima programmazione
            PRECHR_TM = 3;

        MAX_Iac = DataEERead(DEEaddr_CHRGIac);
        if( MAX_Iac > 65500 )  //RESET EEPROM prima programmazione
            MAX_Iac = 160;

        MAX_Vdc = DataEERead(DEEaddr_CHRGVdc);
        if( MAX_Vdc > 65500 )  //RESET EEPROM prima programmazione
            MAX_Vdc = 500;

        MAX_Idc = DataEERead(DEEaddr_CHRGIdc);
        if( MAX_Idc > 65500 )  //RESET EEPROM prima programmazione
            MAX_Idc = 0;

        MAX_Tmp = DataEERead(DEEaddr_CHRGTmp);
        if( MAX_Tmp > 65500 )  //RESET EEPROM prima programmazione
            MAX_Tmp = 50;

        FANS_Temperature = DataEERead(DEEaddr_FANsTmp);
        if( FANS_Temperature > 65500 )  //RESET EEPROM prima programmazione
            FANS_Temperature = 40;

        MAX_BAL_Tmp = DataEERead(DEEaddr_BALTemp);
        if( MAX_BAL_Tmp > 65500 )  //RESET EEPROM prima programmazione
            MAX_BAL_Tmp = 60;
        
        DeltaRV = DataEERead(DEEaddr_DeltaRV);
        if( DeltaRV > 65500 )  //RESET EEPROM prima programmazione
            DeltaRV = 50;
        
        DeltaRI = DataEERead(DEEaddr_DeltaRI);
        if( DeltaRI > 65500 )  //RESET EEPROM prima programmazione
            DeltaRI = 20;

        DISCHR_TM = DataEERead(DEEaddr_DiscTM);
        if( DISCHR_TM > 65500 )  //RESET EEPROM prima programmazione
            DISCHR_TM = 10;

}