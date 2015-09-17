/*********************************************************************
* File:        timer.c
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r00
*
* Created on 3 Marzo 2014, 15.35
*********************************************************************/

#include "p33fxxxx.h"


/*---------------------------------------------------------------------
  Function Name: InitTimer1
  Description:   Initialize Timer1 for 1 millisecond intervals
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------
  TIMER1 is used to generate a Real Clock
  Period = [(PR) + 1] • TCY • (TMR Prescale Value)

  In this case it has been chosen:
    FCY = 40M     PS = 1    PR = 39990
    PWM Period =  [62499+1] • 1/40M • 64 = 100 ms
-----------------------------------------------------------------------*/
void InitTimer1( void )
{

    T1CON = 0;              // Timer reset

    T1CONbits.TCS = 0;		// Select internal instruction cycle clock - era 0
    T1CONbits.TGATE = 0;	// Disable Gated Timer mode
    T1CONbits.TCKPS = 0b10;	// Select 1:64 Prescaler
    T1CONbits.TSYNC = 1;	// era 0

    TMR1 = 0x00; 			// Clear timer register
    PR1 = 62499;	 		// Load the period value

    IFS0bits.T1IF = 0;      // Reset Timer1 interrupt flag
    IPC0bits.T1IP = 3;      // Timer1 Interrupt priority level=5 (0: disabled, 1:low -> 7:high)
    IEC0bits.T1IE = 1;      // Enable Timer1 interrupt

    T1CONbits.TON = 1;      // Enable Timer1 and start the counter

}


/*---------------------------------------------------------------------
  Function Name: InitTimer2
  Description:   Initialize Timer2 for SPI
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------
  Period = [(PR) + 1] • TCY • (TMR Prescale Value)

  In this case it has been chosen:
    FCY = 40M     PS = 64    PR = 65535
    Period =  [624+1] • 1/40M • 64 = 1 ms
-----------------------------------------------------------------------*/
void InitTimer2( void )
{

    T2CON = 0;              // Timer reset

    T2CONbits.TCS = 0;		// Select internal instruction cycle clock
    T2CONbits.TGATE = 0;	// Disable Gated Timer mode
    T2CONbits.TCKPS = 0b10;	// Select 1:64 Prescaler

    TMR2 = 0x00; 			// Clear timer register
    PR2 = 624;	 		// Load the period value

    IFS0bits.T2IF = 0;      // Reset Timer1 interrupt flag
    IPC1bits.T2IP = 4;      // Timer2 Interrupt priority level=1 (0: disabled, 1:low -> 7:high)
    IEC0bits.T2IE = 1;      // Enable Timer1 interrupt

    T2CONbits.TON = 1;      // Enable Timer1 and start the counter

}


/*---------------------------------------------------------------------
  Function Name: InitTimer3
  Description:   Initialize Timer3 as ADC COnversion Trigger
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------
  TIMER3 is used to generate the trigger for ADC conversione
  Period = [(PR) + 1] • TCY • (TMR Prescale Value)

  In this case it has been chosen:
    FCY = 40M     PS = 8    PR = 9990
    Period =  [9999+1] • 1/40M • 8 = 2 ms
-----------------------------------------------------------------------*/
void InitTimer3(void)
{
    // Initialize and enable Timer 3 for ADC conversions

            //	T3CONbits.TON = 0x0; 			// Stop Timer
            //	T3CONbits.TSIDL = 0x0; 			// Continue timer operation in Idle mode
            //	T3CONbits.TGATE = 0x0; 			// Gated time accumulation disabled
            //	T3CONbits.TCKPS = 0x1;	 		// Select 1:8 Prescaler
            //	T3CONbits.TCS = 0x0; 			// Internal clock (FOSC/2)
            T3CON = 0x0010;					// Equivalent to previous commented and indented instructions
    TMR3 = 0x0000; 					// Clear timer register
    PR3 = 9999; 					// Load the period value
    IFS0 &= 0xFEFF;		 			// Clear Timer 3 Interrupt Flag
    IPC2bits.T3IP = 2;              // Timer3 Interrupt priority level=4 (0: disabled, 1:low -> 7:high)
    IEC0 |= 0x0100; 				// Enable Timer 3 interrupt
    T3CON |= 0x8000;				// Start Timer 3

}

/*---------------------------------------------------------------------
  Function Name: InitTimer4
  Description:   Initialize Timer4 for IMD PWM
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------
  Period = [(PR) + 1] • TCY • (TMR Prescale Value)

  In this case it has been chosen:
    FCY = 40M     PS = 64    PR = 65535
    Period =  [624+1] • 1/40M • 64 = 1 ms
-----------------------------------------------------------------------*/
void InitTimer4( void )
{

    T4CON = 0;              // Timer reset

    T4CONbits.TCS = 0;		// Select internal instruction cycle clock
    T4CONbits.TGATE = 0;	// Disable Gated Timer mode
    T4CONbits.TCKPS = 0b10;	// Select 1:64 Prescaler

    TMR4 = 0x00; 			// Clear timer register
    PR4 = 624;	 		// Load the period value

    IFS1bits.T4IF = 0;      // Reset Timer1 interrupt flag
    IPC6bits.T4IP = 3;      // Timer4 Interrupt priority level (0: disabled, 1:low -> 7:high)
    IEC1bits.T4IE = 1;      // Enable Timer1 interrupt

    T4CONbits.TON = 1;      // Enable Timer1 and start the counter

}


/*---------------------------------------------------------------------
  Function Name: InitTimer5
  Description:   Initialize Timer5 for RS232
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------
  Period = [(PR) + 1] • TCY • (TMR Prescale Value)

  In this case it has been chosen:
    FCY = 40M     PS = 64    PR = 9990
    Period =  [62499+1] *• 1/40M *• 256 = 400 ms
-----------------------------------------------------------------------*/
void InitTimer5(void)
{
    T5CON = 0;
    // Initialize and enable Timer2
    T5CONbits.TON = 0; 		// Disable Timer
    T5CONbits.TCS = 0; 		// Select internal instruction cycle clock
    T5CONbits.TGATE = 0; 	  // Disable Gated Timer mode
    T5CONbits.TCKPS = 0b11;	  // Select 1:256 Prescaler
    TMR5 = 0x00; 			// Clear timer register
    PR5 = 65499;			// Load the period value
    IPC7bits.T5IP = 5;  	// Set Timer 5 Interrupt Priority Level (0: disabled, 1:low -> 7:high)
    IFS1bits.T5IF = 0; 		// Clear Timer 5 Interrupt Flag
    IEC1bits.T5IE = 1; 		// Enable Timer 5 interrupt
    T5CONbits.TON = 1; 		// Start Timer

}

/*---------------------------------------------------------------------
  Function Name: InitTimer6
  Description:   Initialize Timer6 for CAN KESM
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------
  Period = [(PR) + 1] • TCY • (TMR Prescale Value)

  In this case it has been chosen:
    FCY = 40M     PS = 64    PR = 65535
    Period =  [624+1] • 1/40M • 64 = 1 ms
-----------------------------------------------------------------------*/
void InitTimer6( void )
{

    T6CON = 0;              // Timer reset

    T6CONbits.TCS = 0;		// Select internal instruction cycle clock
    T6CONbits.TGATE = 0;	// Disable Gated Timer mode
    T6CONbits.TCKPS = 0b10;	// Select 1:64 Prescaler

    TMR6 = 0x00; 			// Clear timer register
    PR6 = 624;	 		// Load the period value

    IFS2bits.T6IF = 0;      // Reset Timer1 interrupt flag
    IPC11bits.T6IP = 3;      // Timer6 Interrupt priority level (0: disabled, 1:low -> 7:high)
    IEC2bits.T6IE = 1;      // Enable Timer1 interrupt

    T6CONbits.TON = 1;      // Enable Timer1 and start the counter

}



void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
{
    IFS0 &= 0xFEFF;		 			// Clear Timer 3 Interrupt Flag
}

