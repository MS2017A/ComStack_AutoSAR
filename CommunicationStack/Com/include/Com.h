/********************************************************************************************
 *                                                                                          *
 * File Name   : Com.h                                                                      *
 *                                                                                          *
 * Author      : ITI AUTOSAR Com_module Team                                                *
 *                                                                                          *
 * contacts:   : Menna Mostafa (menamoustafa29@gmail.com)                                   *
 *                                                                                          *
 *               Mostafa Sayed (mostafas.ahamid@gmail.com)                                  *
 *                                                                                          *
 *               Youssef Kamal (Youx199@gmail.com)                                          *
 *                                                                                          *
 * Date        : 26 Jun 2020                                                                *
 *                                                                                          *
 * Version     : 4.1.1                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR  Basic Software module CAN Transport Layer           *
 *               source file Release 4.3.1                                                  *
 *                                                                                          *
 ********************************************************************************************/

#ifndef COM_H_
#define COM_H_

#include "Std_Types.h"
#include "Com_Cfg.h"
#include "ComStack_Types.h"
#include "Com_Types.h"


/************************************************************************
 *                      Functions Prototypes                            *
 ************************************************************************/

/*initializes internal and external interfaces and variables of the COM module */
void Com_Init( const ComConfig_type* config);

/* Processing of the AUTOSAR COM module's receive activities (PDU To Signal) */
void Com_MainFunctionRx(void);

/* Processing of the AUTOSAR COM module's transmission activities (Signal To PDU)*/
void Com_MainFunctionTx(void);

/* Updates the signal object identified by SignalId with the signal referenced by the SignalDataPtr parameter */
uint8 Com_SendSignal( Com_SignalIdType SignalId, const void* SignalDataPtr );

/*The service Com_ReceiveSignalGroup copies the received signal group from the I-PDU to the shadow buffer.*/
uint8 Com_ReceiveSignalGroup( Com_SignalGroupIdType SignalGroupId );

/*The service Com_SendSignalGroup copies the content of the associated shadow buffer to the associated I-PDU.*/
uint8 Com_SendSignalGroup( Com_SignalGroupIdType SignalGroupId );

/* Copies the data of the signal identified by SignalId to the location specified by SignalDataPtr */
uint8 Com_ReceiveSignal( Com_SignalIdType SignalId, void* SignalDataPtr );

/* the I-PDU with the given ID is triggered for transmission */
Std_ReturnType Com_TriggerIPDUSend( PduIdType PduId );


#endif

