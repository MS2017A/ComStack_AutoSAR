/********************************************************************************************
 *                                                                                          *
 * File Name   : PduR_Cbk.h                                                                 *
 *                                                                                          *
 * Author      : ITI AUTOSAR Com Team                                                       *
 *                                                                                          *
 * contacts:   : Mahmoud Gamal (mahmouddgamall93@gmail.com)                                 *
 *                                                                                          *
 *               Mohammed Ibrahim (mohammedibra96@gmail.com)                                *
 *                                                                                          *
 *               Nada Mohammed (nada2478@gmail.com)                                         *
 *                                                                                          *
 *               Hisham El Sherbieny (h.elsherbieny@gmail.com)                              *
 *                                                                                          *
 *               Menna Mostafa (menamoustafa29@gmail.com)                                   *
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
#ifndef COMMUNICATIONSTACK_PDUR_INCLUDE_PDUR_CBK_H_
#define COMMUNICATIONSTACK_PDUR_INCLUDE_PDUR_CBK_H_



BufReq_ReturnType PduR_CanTpCopyTxData( PduIdType id,  PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr );
BufReq_ReturnType PduR_CanTpStartOfReception(PduIdType id,PduInfoType *info,PduLengthType length,PduLengthType *bufferSizePtr);
BufReq_ReturnType PduR_CanTpCopyRxData(PduIdType id,PduInfoType *info,PduLengthType *bufferSizePtr);
void PduR_CanTpTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result);
void PduR_CanTpRxIndication(PduIdType CanTpRxPduId,Std_ReturnType result);



void PduR_CanIfTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result);
void PduR_CanIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr);





#endif /* COMMUNICATIONSTACK_PDUR_INCLUDE_PDUR_CBK_H_ */
