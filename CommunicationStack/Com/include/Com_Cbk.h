/********************************************************************************************
 *                                                                                          *
 * File Name   : Com_Cbk.h                                                                  *
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
#ifndef COM_CBK_H_
#define COM_CBK_H_



/*Indication of a received PDU from a lower layer communication interface module.*/
void Com_RxIndication(PduIdType ComRxPduId, const PduInfoType* PduInfoPtr);

/*Called after an I-PDU has been received via the TP API, the result indicates whether the transmission was successful or not.*/
void Com_TpRxIndication(PduIdType ComRxPduId,Std_ReturnType Result);

void Com_TxConfirmation( PduIdType TxPduId, Std_ReturnType result );

/*This function is called after the I-PDU has been transmitted on its network, the result indicates whether the transmission was successful or not.*/
void Com_TpTxConfirmation( PduIdType TxPduId, Std_ReturnType result );

/*This function is called at the start of receiving an N-SDU.*/
BufReq_ReturnType Com_StartOfReception(PduIdType PduId,const PduInfoType *info,PduLengthType TpSduLength,PduLengthType *bufferSizePtr);

/*This function is called to provide the received data of an I-PDU segment (N-PDU) to the upper layer.*/
BufReq_ReturnType Com_CopyRxData( PduIdType PduID, const PduInfoType* info, PduLengthType* bufferSizePtr );

/*This function is called to acquire the transmit data of an I-PDU segment (N-PDU).*/
BufReq_ReturnType Com_CopyTxData( PduIdType PduId, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr );


#endif /* COM_CBK_H_ */
