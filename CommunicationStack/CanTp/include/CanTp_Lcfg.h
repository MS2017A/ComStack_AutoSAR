/********************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Lcfg.h                                                               *
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

#ifndef CANTP_LCFG_H_
#define CANTP_LCFG_H_

/* Extern Configuration Arrays */
extern const CanTpTxNSdu_s CanTpTxNSdu[NUMBER_OF_TXNPDU];
extern const CanTpRxNSdu_s CanTpRxNSdu[NUMBER_OF_RXNPDU];
extern ChannelInfo_s ChannelInfo[MAX_CHANNEL_COUNT];



#endif /* CANTP_LCFG_H_ */
