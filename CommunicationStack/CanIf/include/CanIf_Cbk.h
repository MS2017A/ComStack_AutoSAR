/********************************************************************************************
 *                                                                                          *
 * File Name   : CanIf_Cbk.h                                                                *
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
#ifndef COMMUNICATIONSTACK_CANIF_INCLUDE_CANIF_CBK_H_
#define COMMUNICATIONSTACK_CANIF_INCLUDE_CANIF_CBK_H_



FUNC(void ,CANIF_CODE) CanIf_RxIndication(CONSTP2VAR(Can_HwType,CANIF_CODE,AUTOMATIC) Mailbox, CONSTP2VAR(PduInfoType,CANIF_CODE,AUTOMATIC) PduInfoPtr);
FUNC(void ,CANIF_CODE) CanIf_TxConfirmation(VAR(PduIdType,AUTOMATIC) CanTxPduId);





#endif /* COMMUNICATIONSTACK_CANIF_INCLUDE_CANIF_CBK_H_ */
