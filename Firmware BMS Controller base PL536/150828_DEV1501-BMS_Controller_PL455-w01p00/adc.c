/*********************************************************************
* File:        adc.c
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r00
*
* Created on 7 Marzo 2014, 15.35
*********************************************************************/

#include "p33fxxxx.h"
#include "adc.h"
#include "math.h"
#include "common.h"


// #define  SAMP_BUFF_SIZE	 		8	  		// Size of the input buffer per analog input
#define  NUM_CHS2SCAN		   		12			// Number of channels enabled for channel scan


unsigned int BufferADC_A[NUM_CHS2SCAN] __attribute__((space(dma),aligned(256)));
unsigned int BufferADC_B[NUM_CHS2SCAN] __attribute__((space(dma),aligned(256)));

void InitADC(void)
{

    AD1CON1bits.ADON = 0x0;			// Turn off the A/D converter
    AD1CON1bits.ADSIDL = 0x0;		// Continue module operation in Idle mode
    AD1CON1bits.ADDMABM = 0x0;		// DMA buffers are written in Scatter/Gather mode
    AD1CON1bits.AD12B = 0x1;		// 12-bit, 1-channel ADC operation
    AD1CON1bits.FORM = 0x0; 		// Data Output Format bits: Integer (DOUT = 0000 dddd dddd dddd)
    AD1CON1bits.SSRC = 0x2; 		// Sample Clock Source Select bits: Timer3 compare ends sampling and starts conversion
    AD1CON1bits.SIMSAM = 0x0; 		// Samples multiple channels individually in sequence
    AD1CON1bits.ASAM = 0x1; 		// Sampling begins immediately after last conversion. SAMP bit is auto-set
    AD1CON1bits.SAMP = 0x0; 		// No effect
    AD1CON1bits.DONE = 0x0; 		// No effect

    AD1CON2bits.VCFG = 0x2;			// Converter Voltage Reference Configuration bits: A_VDD and Vref-
    AD1CON2bits.CSCNA = 0x1; 		// Scan Input Selections for CH0+ during Sample A bit
    AD1CON2bits.CHPS = 0x0;			// No effect
    AD1CON2bits.BUFS = 0x0;			// No effect
    AD1CON2bits.SMPI = 0x8; 		// 8 ADC buffers
    AD1CON2bits.BUFM = 0x0;			// Always starts filling buffer at address 0x0
    AD1CON2bits.ALTS = 0x0;			// Always uses channel input selects for Sample A

    AD1CON3bits.ADRC = 0x0;			// Clock derived from system clock
    AD1CON3bits.SAMC = 0x00;		// No effect
    AD1CON3bits.ADCS = 0x3F;		// ADC Conversion Clock Select Bits: T_AD = 64 T_CY


    AD1CON4 = 0x0000;				// Allocates 1 word of buffer to each analog input

    //AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
    AD1CSSL = 0x0000;
//  AD1CSSLbits.CSS0=1;			// Enable AN0 for channel scan
//  AD1CSSLbits.CSS1=1;			// Enable AN1 for channel scan
    AD1CSSLbits.CSS2=1;			// Enable AN2 for channel scan
    AD1CSSLbits.CSS3=1;			// Enable AN3 for channel scan
    AD1CSSLbits.CSS4=1;			// Enable AN4 for channel scan
//  AD1CSSLbits.CSS5=1;			// Enable AN5 for channel scan
    AD1CSSLbits.CSS9=1;			// Enable AN9 for channel scan


    IFS0 &= 0xDFFF; 				// Clear the A/D interrupt flag bit
    IEC0 &= 0xDFFF; 				// Disable A/D interrupt
    AD1CON1 |= 0x8000;				// Turn on the A/D converter
}

void InitDMA0(void)
{

    DMA0CONbits.CHEN = 0x0;		// Disable DMA
    DMA0CONbits.SIZE = 0x0;		// Data Transfer Size is Word
    DMA0CONbits.DIR = 0x0;		// Read from peripheral address, write to DMA RAM address
    DMA0CONbits.HALF = 0x0;		// Initiate block transfer complete interrupt when all of the data has been moved
    DMA0CONbits.NULLW = 0x0;		// Normal operation
    DMA0CONbits.AMODE = 0x2;		// Configure DMA for Peripheral Indirect Addressing mode
    DMA0CONbits.MODE = 0x2;		// Configure DMA for Continuous, Ping-Pong modes enabled

    DMA0REQbits.FORCE = 0x0; 		// Automatic DMA transfer initiation by DMA request
    DMA0REQbits.IRQSEL = 0x0D;		// ADC1 convert done as DMA Request Source

    DMA0STA = __builtin_dmaoffset(BufferADC_A);
    DMA0STB = __builtin_dmaoffset(BufferADC_B);
    DMA0PAD = (int)&ADC1BUF0;		// Point DMA to ADC1BUF0
    DMA0CNT = 0x0007;			// 8 DMA transfers (8 buffer with 1 word) (SAMP_BUFF_SIZE*NUM_CHS2SCAN)-1;
    IPC1bits.DMA0IP= 2;                 // DMA0 Interrupt priority level=2
    IFS0bits.DMA0IF = 0;		//Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1;		//Set the DMA interrupt enable bit
    DMA0CONbits.CHEN=1;			//Enable DMA
        
}



static unsigned char DMA0Buffer = 0;
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
    float InstaCurr_LC, InstaCurr_HC, Vdd_AN;

/*
    //++++++++++++++++++++++++ SONDA LEM HASS 50A ++++++++++++++++++++++++++++++++//

    if(DMA0Buffer == 0)
    {   //BufferA
        InstaCurr = (BufferADC_A[2]) * 0.001232; //conversione in V
    }
    else
    {   //BufferB
        InstaCurr = (BufferADC_B[2]) * 0.001232; //conversione in V
    }

    
    if(InstaCurr >= 2.5)
        InstaCurr = (InstaCurr - 2.5) * (-50/0.625);
    else{
        if(InstaCurr > 0.1)
            InstaCurr = (2.5 - InstaCurr) * (50/0.625);
        else
            InstaCurr = 0;
    }
    InstaCurr = (InstaCurr * Curr_Gain ) + Curr_Offset;

    //--------------------------------------------------------------------------//
*/

    //+++++++++++++++++++++++ SONDA LEM DHAB S/06 ++++++++++++++++++++++++++++//

    if(DMA0Buffer == 0)
    {   //BufferA
        InstaCurr_HC = (BufferADC_A[2]) * 0.0012547; //conversione in V
        InstaCurr_LC = (BufferADC_A[3]) * 0.0012547; //conversione in V
        Vdd_AN = (BufferADC_A[4]) * 0.0012547;      //conversione in V
        WaterLeak_AN = (BufferADC_A[9]) * 0.0012547; //conversione in V
    }
    else
    {   //BufferB
        InstaCurr_HC = (BufferADC_B[2]) * 0.0012547; //conversione in V
        InstaCurr_LC = (BufferADC_B[3]) * 0.0012547; //conversione in V
        Vdd_AN = (BufferADC_B[4]) * 0.0012547;      //conversione in V
        WaterLeak_AN = (BufferADC_B[9]) * 0.0012547; //conversione in V
    }

    if( (InstaCurr_HC < 4.9) && (InstaCurr_LC < 4.9) )
        CrntSense_Plg = 1;
    else
        CrntSense_Plg = 0;

//    Vdd_AN = 5;  //Da forzare se non c'è ingresso analogico dedicato.

    if( CrntSense_Plg == 1 ){

        if(InstaCurr_LC >= (Vdd_AN/2))
            InstaCurr_LC = (InstaCurr_LC - (Vdd_AN/2)) * (-10*(5/Vdd_AN));
        else{
            if(InstaCurr_LC > 0.1)
                InstaCurr_LC = ((Vdd_AN/2) - InstaCurr_LC) * (10*(5/Vdd_AN));
            else
                InstaCurr_LC = 0;
        }

        if(InstaCurr_HC >= (Vdd_AN/2))
            InstaCurr_HC = (InstaCurr_HC - (Vdd_AN/2)) * (-250*(5/Vdd_AN));
        else{
            if(InstaCurr_HC > 0.1)
                InstaCurr_HC = ((Vdd_AN/2) - InstaCurr_HC) * (250*(5/Vdd_AN));
            else
                InstaCurr_HC = 0;
        }

        if( fabsf(InstaCurr_LC) > 18)
            InstaCurr = (InstaCurr_HC * Curr_Gain ) + (Curr_Offset*25);
        else
            InstaCurr = (InstaCurr_LC * Curr_Gain ) + Curr_Offset;

    }else
        InstaCurr = 0;

    //--------------------------------------------------------------------------//



    DMA0Buffer ^= 1;
    IFS0bits.DMA0IF = 0;				// Clear the DMA0 interrupt flag bit

}


