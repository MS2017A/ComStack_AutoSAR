/********************************************************************************************
 *                                                                                          *
 * File Name   : Can.h                                                                      *
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

/* -------------------------------- Arctic Core ------------------------------
 * Arctic Core - the open source AUTOSAR platform http://arccore.com
 *
 * Copyright (C) 2009  ArcCore AB <contact@arccore.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * -------------------------------- Arctic Core ------------------------------*/
//////////////////////////////////////////////////////////

#ifndef INCLUDES_CAN_H_
#define INCLUDES_CAN_H_
#include "Std_Types.h"
#include "ComStack_Types.h"

/*[SWS_Can_00435] ⌈The Can.h file shall include Can_GeneralTypes.h.*/
#include "Can_GeneralTypes.h"
#include "Can_Cfg.h"





#define CAN_CONTROLLER_ZERO                   (0U)
#define CAN_CONTROLLER_ONE                    (1U)


#define CAN_DISABLE_INTERRUPT_NOT_CALLED     (0U)

#define IS_VALID_HO(HO) ( (HO) < MAX_NUM_OF_HO )




FUNC(void,CAN_CODE) Can_Init(P2VAR(Can_ConfigType,CAN_CODE,AUTOMATIC)  Config);
FUNC(void,CAN_CODE) Can_DeInit(void);
FUNC(Std_ReturnType,CAN_CODE) Can_SetBaudrate(VAR(uint8,AUTOMATIC) Controller,VAR(uint16,AUTOMATIC) BaudRateConfigID );
FUNC(Std_ReturnType,CAN_CODE) Can_SetControllerMode( VAR(uint8,AUTOMATIC) Controller, VAR(Can_StateTransitionType,AUTOMATIC) Transition );
FUNC (Std_ReturnType,CAN_CODE) Can_GetControllerMode( VAR(uint8,AUTOMATIC) Controller, P2VAR(Can_ControllerStateType,CAN_CODE,AUTOMATIC)  ControllerModePtr);
FUNC(void,CAN_CODE)  Can_EnableControllerInterrupts(VAR(uint8,AUTOMATIC) Controller);
FUNC(void,CAN_CODE)  Can_DisableControllerInterrupts(VAR(uint8,AUTOMATIC) Controller);
FUNC(void,CAN_CODE) Can_MainFunction_Mode( void );
FUNC (void,CAN_CODE)  Can_MainFunction_Read(void);
FUNC(Std_ReturnType,CAN_CODE) Can_Write(VAR(Can_HwHandleType,AUTOMATIC) HTH ,CONSTP2VAR(Can_PduType,CAN_CODE,AUTOMATIC) PduInfo);
FUNC(void,CAN_CODE)  Can_MainFunction_Write(void);
FUNC(Std_ReturnType,CAN_CODE) Can_GetControllerErrorState( VAR(uint8,AUTOMATIC) ControllerId,P2VAR(Can_ErrorStateType,CAN_CODE,AUTOMATIC)  ErrorStatePtr );











#endif /* INCLUDES_CAN_H_ */

//////////////////////////////////////////////////////////

#ifndef CAN_H_
#define CAN_H_

#include <Modules.h>









#endif /*CAN_H_*/
