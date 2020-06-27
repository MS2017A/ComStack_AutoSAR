/********************************************************************************************
 *                                                                                          *
 * File Name   : PduR.c                                                                     *
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
#include "PduR.h"
#include "PduR_types.h"
#include "PduR_Cfg.h"
#include "PduR_Lcfg.h"
#include "ComStack_Types.h"
#include "CanIf.h"
#include "CanTp.h"
#include "Com_Cbk.h"
#include "ECUC.h"


#define DBUG TRUE

#define		IS_VALID_PDUID(TxPduId)		(TxPduId < MAX_NUM_ROUTES)
#define		IS_VALID_RESULT(result)		(result == E_OK || result == E_NOT_OK)


typedef P2FUNC( Std_ReturnType, ptrclass, p2func )(PduIdType , const PduInfoType* ) ;

/* TransmitLookUpTable shall include all lower layer function */
CONST (p2func,memclass)TransmitLookUpTable[NUM_OF_MODULES] =
{
 CanIf_Transmit,
 CanTp_Transmit
} ;


BufReq_ReturnType PduR_CanTpCopyTxData( PduIdType id,  PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr )
{
#if 0
    return Com_CopyTxData(id, info, retry, availableDataPtr);
#endif

#if DBUG == TRUE
    static uint8 count = 0;
       uint8 Array[] = {'I','T','I','_','4','0','_','C','a','N','T','p','_','T','e','a','m'};

       if (count == 0)
       {
           info->SduDataPtr = Array;
           *availableDataPtr = 17 - info->SduLength ;
       }

       if (count == 1)
       {
           info->SduDataPtr = &Array[6];
           *availableDataPtr = 11 - info->SduLength ;

       }

       if (count == 2)
       {
           info->SduDataPtr = &Array[13];
           *availableDataPtr = 4 - info->SduLength ;

       }
       count++;
       return BUFREQ_OK;

#endif
}


BufReq_ReturnType PduR_CanTpStartOfReception(PduIdType id,PduInfoType *info,PduLengthType length,PduLengthType *bufferSizePtr)
{
#if 0
    return Com_StartOfReception(id, info, length, bufferSizePtr);
#endif

#if DUBGE == TRUE
    *bufferSizePtr = 40;          // any number != 0
     return BUFREQ_OK;
#endif
}



BufReq_ReturnType PduR_CanTpCopyRxData(PduIdType id,PduInfoType *info,PduLengthType *bufferSizePtr)
{
#if 0
    return Com_CopyRxData(id, info, bufferSizePtr);
#endif

#if DUBGE == TRUE
    *bufferSizePtr = *bufferSizePtr - info->SduLength;
    return BUFREQ_OK;
#endif
}





void PduR_CanTpTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result)
{
#if 0
    Com_TpTxConfirmation(CanTpTxPduId, result);
#endif
}

void PduR_CanTpRxIndication(PduIdType CanTpRxPduId,Std_ReturnType result)
{
#if 0
    Com_TpRxIndication(CanTpRxPduId, result);
#endif
}




void PduR_CanIfTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result)
{
#if 0
    Com_TxConfirmation(CanTpTxPduId, result);
#endif
}



void PduR_CanIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr)
{
#if 0
    Com_RxIndication(RxPduId, PduInfoPtr );
#endif
}


Std_ReturnType PduR_ComTransmit( PduIdType TxPduId, const PduInfoType* PduInfoPtr )
{
#if 0
    PduIdType  Lower_Layer  = Pdur_LowerLayerUser_LookUpTable[TxPduId];
    return TransmitLookUpTable[PduRRoutingPath[TxPduId].PduRDestPduRRef ]( Lower_Layer ,PduInfoPtr);
#endif

}
