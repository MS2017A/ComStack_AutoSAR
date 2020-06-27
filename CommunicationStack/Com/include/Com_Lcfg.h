/********************************************************************************************
 *                                                                                          *
 * File Name   : Com_Lcfg.h                                                                 *
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
#ifndef COM_LCFG_H_
#define COM_LCFG_H_


extern CONST(ComIPdu_type, memclass)          ComIPdus[COM_NUM_OF_IPDU];
extern CONST(ComSignal_type, memclass)        ComSignals[COM_NUM_OF_SIGNAL];
extern CONST(ComTxIPdu_type, memclass)        ComTxIPdus[COM_NUM_OF_TX_IPDU];
extern CONST(ComSignalGroup_type, memclass)   ComSignalGroups[COM_NUM_OF_GROUP_SIGNAL];


#endif /* COM_LCFG_H_ */
