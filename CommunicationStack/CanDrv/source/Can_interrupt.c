/********************************************************************************************
 *                                                                                          *
 * File Name   : Can_interrupt.c                                                            *
 *                                                                                          *
 * Author      : ITI AUTOSAR Can_CanIf Team                                                 *
 *                                                                                          *
 * contacts:   : Mahmoud Gamal (mahmouddgamall93@gmail.com)                                 *
 *                                                                                          *
 *               Mohammed Ibrahim (mohammedibra96@gmail.com)                                *
 *                                                                                          *
 * Date        : 26 Jun 2020                                                                *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Version     : 4.1.1                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR  Basic Software module CAN Transport Layer           *
 *               source file Release 4.3.1                                                  *
 *                                                                                          *
 ********************************************************************************************/
#include "Can.h"
#include "Can_interrupt.h"
#include "Can_Lcfg.h"



#define CAN_CONTROLLER_ZERO_BASE_ADDRESS 0x40040000U
#define CAN_CONTROLLER_ONE_BASE_ADDRESS  0x40041000U

void Can0_InterruptHandler(void)
{
    if ( CanController[0].CanControllerBaseAddress == CAN_CONTROLLER_ZERO_BASE_ADDRESS )
    {
        CAN_IRQHandler((uint8 )CAN_CONTROLLER_ZERO);
    }
    else if ( CanController[1].CanControllerBaseAddress == CAN_CONTROLLER_ZERO_BASE_ADDRESS )
    {
        CAN_IRQHandler( CAN_CONTROLLER_ONE );
    }
    else
    {

    }
}

void Can1_InterruptHandler(void) 
{
    if ( CanController[0].CanControllerBaseAddress == CAN_CONTROLLER_ONE_BASE_ADDRESS )
    {
        CAN_IRQHandler((uint8 ) CAN_CONTROLLER_ZERO );
    }
    else if  ( (MAX_NUM_CONTROLLER > 1) && (CanController[1].CanControllerBaseAddress  == CAN_CONTROLLER_ONE_BASE_ADDRESS) )
    {
        CAN_IRQHandler( (uint8 )CAN_CONTROLLER_ONE );
    }
    else
    {

    }
}



