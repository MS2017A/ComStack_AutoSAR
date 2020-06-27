/********************************************************************************************
 *                                                                                          *
 * File Name   : CanIf.h                                                                    *
 *                                                                                          *
 * Author      : ITI AUTOSAR Can_CanIf Team                                                 *
 *                                                                                          *
 * contacts:   : Mahmoud Gamal (mahmouddgamall93@gmail.com)                                 *
 *                                                                                          *
 *               Mohammed Ibrahim (mohammedibra96@gmail.com)                                *
 *                                                                                          *
 * Date        : 26 Jun 2020                                                                *
 *                                                                                          *
 * Version     : 4.1.1                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR  Basic Software module CAN Transport Layer           *
 *               source file Release 4.3.1                                                  *
 *                                                                                          *
 ********************************************************************************************/
#ifndef CANIF_H_
#define CANIF_H_


#include "Modules.h"
#include "Std_Types.h"


#define CANIF_VENDOR_ID          (VENDOR_ID_ARCCORE)
#define CANIF_MODULE_ID          (MODULE_ID_CANIF)



#if defined(USE_DET)
#include "Det.h"
#endif
#include "CanIf_Types.h"
#include "CanIf_Cfg.h"

#define IS_PDU_MODE(PDU_MODE)   (PDU_MODE == CANIF_OFFLINE) || (PDU_MODE == CANIF_TX_OFFLINE) || (PDU_MODE == CANIF_TX_OFFLINE_ACTIVE) || (PDU_MODE == CANIF_ONLINE)




FUNC(void,CANIF_CODE) CanIf_Init(CONSTP2VAR(CanIf_ConfigType_s,CANIF_CODE,AUTOMATIC) ConfigPtr);
FUNC(void,CANIF_CODE) CanIf_DeInit(void);
FUNC(Std_ReturnType,CANIF_CODE) CanIf_SetControllerMode(VAR(uint8,AUTOMATIC) ControllerId,VAR(Can_ControllerStateType , AUTOMATIC) ControllerMode);
FUNC(Std_ReturnType,CANIF_CODE) CanIf_GetControllerMode(VAR(uint8 ,AUTOMATIC) ControllerId, Can_ControllerStateType* ControllerModePtr);
FUNC(Std_ReturnType,CANIF) CanIf_GetControllerErrorState(VAR(uint8,AUTOMATIC) ControllerId,P2VAR( Can_ErrorStateType,CANIF_CODE,AUTOMATIC) ErrorStatePtr);
FUNC(Std_ReturnType,CANIF) CanIf_Transmit(VAR(PduIdType,AUTOMATIC) TxPduId,  CONSTP2VAR(PduInfoType,CANIF_CODE,AUTOMATIC) PduInfoPtr);
FUNC(Std_ReturnType,CANIF_CODE) CanIf_SetPduMode(VAR(uint8,AUTOMATIC) ControllerId,VAR( CanIf_PduModeType, AUTOMATIC) PduModeRequest);
FUNC(Std_ReturnType ,CANIF) CanIf_GetPduMode(VAR(uint8,AUTOMATIC) ControllerId,P2VAR(CanIf_PduModeType,CANIF_CODE,AUTOMATIC)  PduModePtr);



#endif
