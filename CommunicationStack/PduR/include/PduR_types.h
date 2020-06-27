/********************************************************************************************
 *                                                                                          *
 * File Name   : PduR_types.h                                                               *
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
#ifndef PDUR_TYPES_H
#define PDUR_TYPES_H


typedef uint8 PduRSrcPduRRefType ;
typedef uint8 PduRDestPduRRefType;

typedef struct
{
    PduRSrcPduRRefType          PduRSrcPduRRef;      /*index*/
    PduRDestPduRRefType         PduRDestPduRRef;    /*index We support only one destination*/

}PduRRoutingPath_s;


#endif

