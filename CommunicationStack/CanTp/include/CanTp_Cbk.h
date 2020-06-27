/********************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Cbk.h                                                                *
 *                                                                                          *
 * Author      : ITI AUTOSAR CanTP Team                                                     *
 *                                                                                          *
 * contacts:   : Nada Mohammed (nada2478@gmail.com)                                         *
 *                                                                                          *
 *               Hisham El Sherbieny (h.elsherbieny@gmail.com)                              *
 *                                                                                          *
 * Date        : 26 Jun 2020                                                                *
 *                                                                                          *
 * Version     : 4.1.1                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR  Basic Software module CAN Transport Layer           *
 *               source file Release 4.3.1                                                  *
 *                                                                                          *
 ********************************************************************************************/


#ifndef CANTP_CBK_H_
#define CANTP_CBK_H_

FUNC(void,CANTP) CanTp_TxConfirmation( VAR(PduIdType,AUTOMATIC) TxPduId, VAR(Std_ReturnType,AUTOMATIC) result );
FUNC(void,CANTP) CanTp_RxIndication( VAR(PduIdType,AUTOMATIC) RxPduId, CONSTP2VAR(PduInfoType, CANTP, AUTOMATIC) PduInfoPtr);


#endif /* CANTP_CBK_H_ */
