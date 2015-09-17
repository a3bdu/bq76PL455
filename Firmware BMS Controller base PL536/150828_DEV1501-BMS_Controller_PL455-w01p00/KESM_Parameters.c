/*********************************************************************
* File:        KESM_Parameters.c
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       A13_KESM-BMSController-r01
*
* Created on 3 July 2015, 11.35
*********************************************************************/

#include "p33fxxxx.h"
#include "common.h"
#include "timer.h"
#include "KESM_Parameters.h"
#include <libpic30.h>            
#include <libq.h>
#include "can.h"
#include <math.h>
#include "EEPROM_Emulation_16bit.h"



unsigned int KESM_Status_cnt=0, KESM_ResetP = 0;
unsigned int KESM_Status = 1;  // 1=ok, 2=errore
void KESM_Parameter_WRITE( unsigned int CMD_id, unsigned int size, unsigned int index, unsigned int value )
{

    unsigned int Curr_Gain_tmp, Curr_Offset_tmp;

    if( CMD_id == 1 ){  //1st step
        
        if( KESM_Status_cnt != 0 )
            KESM_Status = 2;
        
        CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, 0x02, KESM_Status, 0, 0, 0, 0, 0 );
        
        KESM_Status_cnt = 1;
        KESM_Timeout = 1; //Start timer for timout control
        
    }
    
    if( CMD_id == 2 ){  //2nd stemp

        KESM_Timeout = 1; //Start timer for timout control

        if( KESM_Status_cnt != 1 )
            KESM_Status = 2;

        switch( index ){
            case ( 0 ):        /* Balance DELTA */
                DELTA = value;
                DataEEWrite(DELTA, DEEaddr_DELTA);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DELTA >> 8), (DELTA & 0xff), 0, 0, 0 );
                break;
            case ( 1 ):        /* Max Balance Temperature */
                MAX_BAL_Tmp = value;
                DataEEWrite(MAX_BAL_Tmp, DEEaddr_BALTemp);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_BAL_Tmp >> 8), (MAX_BAL_Tmp & 0xff), 0, 0, 0 );
                break;
            case ( 2 ):        /* Total Capacity */
                Cap_TOT = value;
                DataEEWrite(Cap_TOT, DEEaddr_Cap);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Cap_TOT >> 8), (Cap_TOT & 0xff), 0, 0, 0 );
                break;
            case ( 3 ):        /* Over Voltage */
                Over_V = value;
                DataEEWrite(Over_V, DEEaddr_OverV);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Over_V >> 8), (Over_V & 0xff), 1, 0, 0 );
                KESM_ResetP = 1;
                break;
            case ( 4 ):        /* Under Voltage */
                Under_V = value;
                DataEEWrite(Under_V, DEEaddr_UnderV);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Under_V >> 8), (Under_V & 0xff), 1, 0, 0 );
                KESM_ResetP = 1;
                break;
            case ( 5 ):        /* Current Sense Gain */
                Curr_Gain_tmp = value;
                DataEEWrite(Curr_Gain_tmp, DEEaddr_CGain);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Curr_Gain_tmp >> 8), (Curr_Gain_tmp & 0xff), 0, 0, 0 );
                Curr_Gain = Curr_Gain_tmp * 0.01;
                break;
            case ( 6 ):        /* Current Sense Offset */   
                Curr_Offset_tmp = value;
                DataEEWrite( Curr_Offset_tmp , DEEaddr_COffset);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Curr_Offset_tmp >> 8), (Curr_Offset_tmp & 0xff), 0, 0, 0 );
                Curr_Offset_tmp -= 100;
                Curr_Offset = Curr_Offset_tmp * 0.01;
                break;
            case ( 7 ):        /* Resistance Delta R-V */
                DeltaRV = value;
                DataEEWrite(DeltaRV, DEEaddr_DeltaRV);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DeltaRV >> 8), (DeltaRV & 0xff), 0, 0, 0 );
                break;
            case ( 8 ):        /* Resistance Delta R-I */
                DeltaRI = value;
                DataEEWrite(DeltaRI, DEEaddr_DeltaRI);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DeltaRI >> 8), (DeltaRI & 0xff), 0, 0, 0 );
                break;
            case ( 9 ):        /* Temperature FANs Enable */
                FANS_Temperature = value;
                DataEEWrite(FANS_Temperature, DEEaddr_FANsTmp);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (FANS_Temperature >> 8), (FANS_Temperature & 0xff), 0, 0, 0 );
                break;
            case ( 10 ):        /* BRUSA Inlet AC Current */
                MAX_Iac = value;
                DataEEWrite(MAX_Iac, DEEaddr_CHRGIac);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_Iac >> 8), (MAX_Iac & 0xff), 0, 0, 0 );
                break;
            case ( 11 ):        /* BRUSA MAX DC Out Voltage */
                MAX_Vdc = value;
                DataEEWrite(MAX_Vdc, DEEaddr_CHRGVdc);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_Vdc >> 8), (MAX_Vdc & 0xff), 0, 0, 0 );
                break;
            case ( 12 ):        /* BRUSA MAX DC Out Current */
                MAX_Idc = value;
                DataEEWrite(MAX_Idc, DEEaddr_CHRGIdc);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_Idc >> 8), (MAX_Idc & 0xff), 0, 0, 0 );
                break;
            case ( 13 ):        /* HV DC PreCharge Time */
                PRECHR_TM = value;
                DataEEWrite(PRECHR_TM, DEEaddr_PrecTM);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (PRECHR_TM >> 8), (PRECHR_TM & 0xff), 0, 0, 0 );
                break;
            case ( 14 ):        /* HV DC DisCharge Time */
                DISCHR_TM = value;
                DataEEWrite(DISCHR_TM, DEEaddr_DiscTM);
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DISCHR_TM >> 8), (DISCHR_TM & 0xff), 0, 0, 0 );
                break;
        }
        
        KESM_Status_cnt = 2;
        
    }
        
    
    if( CMD_id == 0 ){ //3rd step
        
        if( KESM_Status_cnt != 2 )
            KESM_Status = 2;
        
        CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, 0x02, KESM_Status, KESM_ResetP, 0, 0, 0, 0 );
        
        if( KESM_ResetP == 1 )
            asm ("RESET");
        
        KESM_ResetP = 0;
        KESM_Status_cnt = 0;
        KESM_Status = 1;
        KESM_Timeout = 0;
        
    }
    
        
}


void KESM_Parameter_READ( unsigned int CMD_id, unsigned int size, unsigned int index )
{

    unsigned int Curr_Gain_tmp, Curr_Offset_tmp;

    if( CMD_id == 1 ){  //1st step

        if( KESM_Status_cnt != 0 )
            KESM_Status = 2;

        CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, 0x01, KESM_Status, 0, 0, 0, 0, 0 );

        KESM_Status_cnt = 1;
        KESM_Timeout = 1; //Start timer for timout control

    }

    if( CMD_id == 3 ){ //2nd stemp

        KESM_Timeout = 1; //Start timer for timout control

        if( KESM_Status_cnt != 1 )
            KESM_Status = 2;

        switch( index ){

            case ( 0 ):        /* Balance DELTA */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DELTA >> 8), (DELTA & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 1 ):        /* Max Balance Temperature */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_BAL_Tmp >> 8), (MAX_BAL_Tmp & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 2 ):        /* Total Capacity */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Cap_TOT >> 8), (Cap_TOT & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 3 ):        /* Over Voltage */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Over_V >> 8), (Over_V & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 4 ):        /* Under Voltage */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Under_V >> 8), (Under_V & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 5 ):        /* Current Sense Gain */
                Curr_Gain_tmp = Curr_Gain * 100;
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Curr_Gain_tmp >> 8), (Curr_Gain_tmp & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 6 ):        /* Current Sense Offset */
                Curr_Offset_tmp = Curr_Offset + 100;
                Curr_Offset_tmp *= 100;
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (Curr_Offset_tmp >> 8), (Curr_Offset_tmp & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 7 ):        /* Resistance Delta R-V */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DeltaRV >> 8), (DeltaRV & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 8 ):        /* Resistance Delta R-I */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DeltaRI >> 8), (DeltaRI & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 9 ):        /* Temperature FANs Enable */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (FANS_Temperature >> 8), (FANS_Temperature & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 10 ):        /* BRUSA Inlet AC Current */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_Iac >> 8), (MAX_Iac & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 11 ):        /* BRUSA MAX DC Out Voltage */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_Vdc >> 8), (MAX_Vdc & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 12 ):        /* BRUSA MAX DC Out Current */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (MAX_Idc >> 8), (MAX_Idc & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 13 ):        /* HV DC PreCharge Time */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (PRECHR_TM >> 8), (PRECHR_TM & 0xff), KESM_Status, 0, 0 );
                break;
            case ( 14 ):        /* HV DC DisCharge Time */
                CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, size, index, (DISCHR_TM >> 8), (DISCHR_TM & 0xff), KESM_Status, 0, 0 );
                break;
        }

        KESM_Status_cnt = 2;

    }


    if( CMD_id == 0 ){ //3rd step

        if( KESM_Status_cnt != 2 )
            KESM_Status = 2;

        CANSendPacket1_Bit(KESM_PARAMETER_BMS, 8, CMD_id, 0x01, KESM_Status, 0, 0, 0, 0, 0 );

        KESM_Status_cnt = 0;
        KESM_Status = 1;
        KESM_Timeout = 0;

    }


}
