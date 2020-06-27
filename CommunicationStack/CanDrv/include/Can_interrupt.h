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
#ifndef CAN_INTERRUPT_H_
#define CAN_INTERRUPT_H_
#include "Std_Types.h"


#define ENTER_CAN_CONTROLLER_INTERRUPT_DISABLE(CONTROLLER)				do{CanControllerInterruptDisable[CONTROLLER]++;}while(0)


#define EXIT_CAN_CONTROLLER_INTERRUPT_DISABLE(CONTROLLER)				do{CanControllerInterruptDisable[CONTROLLER]--;}while(0)


#define IN_CAN_CONTROLLER_INTERRUPT_DISABLE()							(CanControllerInterruptDisable[Controller] != CAN_DISABLE_INTERRUPT_NOT_CALLED)


#define OUT_CAN_CONTROLLER_INTERRUPT_DISABLE()							(CanControllerInterruptDisable[Controller] == CAN_DISABLE_INTERRUPT_NOT_CALLED)


FUNC(void,CAN_CODE) CAN_IRQHandler(VAR(uint8, AUTOMATIC) Controller);

void Can0_InterruptHandler(void);
void Can1_InterruptHandler(void);

#endif