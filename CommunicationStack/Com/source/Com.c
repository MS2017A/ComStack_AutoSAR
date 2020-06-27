/********************************************************************************************
 *                                                                                          *
 * File Name   : Com.c                                                                      *
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
#include "Com.h"
#include "Com_Lcfg.h"
#include "PduR_Com.h"
#include "PduR.h"
#include "Com_Cbk.h"
#include "Det.h"
#include "Rte_criticalSection.h"

#define SHADOW_BUFFER                               0
#define REAL_BUFFER                                 1

#define NORMAL_SIGNAL   ((uint8)0x00)
#define GROUP_SIGNAL    ((uint8)0xff)



#define SETBIT(dest,bit)    ( *( (uint8 *)(dest)    + ((bit) / (uint32)8) ) |= (uint8)(1u << ((bit) % (uint32)8)) )
#define CLEARBIT(dest,bit)  ( *( (uint8 *)(dest)    + ((bit) / (uint32)8) ) &= (uint8)~(uint8)(1u << ((bit) % (uint32)8)) )
#define CHECKBIT(source,bit)    ( *( (uint8 *)(source)  + ((bit) / (uint32)8) ) &  (uint8)(1u << ((bit) % (uint32)8)) )

#define GET_Signal(SignalId) \
        (&ComSignals[(SignalId)])

#define GET_IPdu(IPduId) \
        (&ComIPdus[(IPduId)])

#define NUMBER_OF_AUXILARY_ARR             2


typedef struct
{
    uint32  CurrentPosition;
    /*TODO:move updated to private TX*/
    boolean updated;
    boolean locked;
}privateIPdu_type;

typedef struct
{
    float32 remainingTimePeriod;
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
    float32 minimumDelayTimer;
#endif
    uint8   numberOfRepetitionsLeft;
}privateTxIPdu_type;

typedef P2FUNC(void, ptrclass, notificationType)(void);

LOCAL FUNC(void, memclass)      Com_WriteSignalDataToPduBuffer(CONSTP2CONST(ComSignal_type, memclass, ptrclass) signal,VAR(uint8, memclass) type);
LOCAL FUNC(void, memclass)      Com_ReadSignalDataFromPduBuffer(VAR(PduIdType, memclass) ComRxPduId,CONSTP2CONST(ComSignal_type, memclass, ptrclass) SignalRef,VAR(uint8, memclass) type);
LOCAL FUNC(void, memclass)      privateGeneralTxConfirmation(VAR(PduIdType,memclass) TxPduId);
LOCAL FUNC(void, memclass)      Com_PackSignalsToPdu(VAR(uint16, memclass) ComIPuId);
LOCAL FUNC(void, memclass)      Com_PduUnpacking(VAR(PduIdType, memclass) ComRxPduId);
LOCAL FUNC(void, memclass)      Com_ReadSignalDataFromSignalBuffer (CONST(uint16, memclass)  signalId,  P2VAR(void, memclass, ptrclass) signalData);
LOCAL FUNC(boolean ,memclass)   validateSignalID (VAR(Com_SignalIdType, memclass) SignalId);
LOCAL FUNC(void, memclass)      Com_WriteSignalDataToSignalBuffer (CONST(uint16, memclass) signalId, CONSTP2VAR(void, memclass, ptrclass) signalData);

/* Com_Config declaration*/
/*VAR(PduIdType,memclass) com_pdur[] = {vcom};*/

/* Global variables*/
LOCAL VAR(privateIPdu_type, memclass)         privateIPdus[COM_NUM_OF_IPDU];
LOCAL VAR(privateTxIPdu_type, memclass)       privateTxIPdus[COM_NUM_OF_TX_IPDU];
LOCAL VAR(uint16, memclass)                   txIPdusIds[COM_NUM_OF_TX_IPDU];
LOCAL VAR(notificationType, memclass)         pendingTxNotifications[NUMBER_OF_AUXILARY_ARR][COM_NUM_OF_SIGNAL];
LOCAL VAR(uint8, memclass)                    pendingTxNotificationsBufferIndex;
LOCAL VAR(uint16, memclass)                   pendingTxNotificationsNumber;

LOCAL VAR(uint8, memclass)                    rxIndicationProcessingDeferredPduIndex;
LOCAL VAR(uint16, memclass)                   rxindicationNumberOfRecievedPdu;
LOCAL VAR(PduIdType, memclass)                rxDeferredPduArr[NUMBER_OF_AUXILARY_ARR][COM_NUM_OF_IPDU];

/*****************************************************************
 *                     Functions Definitions                     *
 *****************************************************************/
LOCAL FUNC(boolean ,memclass) validateSignalID (VAR(Com_SignalIdType, memclass) SignalId)
                                                                                                                                                                                                                                                                                        {
    boolean result=TRUE;
    if(SignalId<(Com_SignalIdType)COM_NUM_OF_SIGNAL)
    {
        result = (boolean)TRUE;
    }
    else
    {
        result = (boolean)FALSE;
    }
    return result;
                                                                                                                                                                                                                                                                                        }

LOCAL FUNC(void, memclass)
Com_ReadSignalDataFromSignalBuffer (CONST(uint16, memclass)  signalId,  P2VAR(void, memclass, ptrclass) signalData)
{
    VAR(uint8, AUTOMATIC) Size;
    if(ComSignals[signalId].ComSignalType==UINT8_N)
    {
        Rte_enterCriticaSection();
        memcpy(signalData, ComSignals[signalId].ComSignalDataPtr,ComSignals[signalId].ComSignalLength);
        Rte_exitCriticalSection();
    }
    else
    {
        Size=ComSignals[signalId].ComBitSize/(uint8)8;
        if(ComSignals[signalId].ComBitSize%(uint8)8)
        {
            Size++;
        }
        Rte_enterCriticaSection();
        memcpy(signalData, ComSignals[signalId].ComSignalDataPtr, (uint32)Size);
        Rte_exitCriticalSection();
    }
}


LOCAL FUNC(void, memclass)
Com_WriteSignalDataToSignalBuffer (CONST(uint16, memclass) signalId, CONSTP2VAR(void, memclass, ptrclass) signalData)
{
    CONSTP2VAR(ComSignal_type, AUTOMATIC, ptrclass) Signal;
    VAR(uint8, AUTOMATIC) mod;

    Signal =  GET_Signal(signalId);
    if(Signal->ComSignalType==UINT8_N)
    {
        Rte_enterCriticaSection();
        memcpy(Signal->ComSignalDataPtr, signalData, Signal->ComSignalLength);
        Rte_exitCriticalSection();
    }
    else
    {
        if(Signal->ComBitSize%(uint8)8)
        {
            mod=(uint8)1;
        }
        else
        {
            mod=(uint8)0;
        }
        Rte_enterCriticaSection();
        memcpy(Signal->ComSignalDataPtr, signalData, (uint32)((Signal->ComBitSize/(uint8)8)+(uint8)mod));
        Rte_exitCriticalSection();
    }
}

LOCAL FUNC(void, memclass)
Com_ReadSignalDataFromPduBuffer(VAR(PduIdType, memclass) ComRxPduId,CONSTP2CONST(ComSignal_type, memclass, ptrclass) SignalRef,VAR(uint8, memclass) type)
{
    /*TODO: add the sequence of the TP case (for UINT8_DYN)*/
    VAR(uint8, AUTOMATIC) signalLength;
    VAR(uint32, AUTOMATIC) startBit;
    VAR(uint64, AUTOMATIC) Data;

    startBit = SignalRef->ComBitPosition;
    signalLength = SignalRef->ComBitSize;

    switch(SignalRef->ComSignalType)
    {
    case BOOLEAN:
    case SINT8:
    case UINT8:
        if(type==NORMAL_SIGNAL)
        {
            Data=*((uint64*)ComIPdus[ComRxPduId].ComIPduDataPtr);
        }
        else
        {
            Data = *((uint64*)(((uint8*)ComIPdus[ComRxPduId].ComIPduDataPtr)+1));
        }
        Data = Data << ((uint32)64 - (startBit+signalLength));
        Data = Data >> ((uint32)64 - (signalLength));
        *((uint8*)SignalRef->ComSignalDataPtr)=(uint8)Data;
        break;
    case FLOAT32:
    case UINT32:
    case SINT32:
        if(type==NORMAL_SIGNAL)
        {
            Data=*((uint64*)ComIPdus[ComRxPduId].ComIPduDataPtr);
        }
        else
        {
            Data = *((uint64*)(((uint8*)ComIPdus[ComRxPduId].ComIPduDataPtr)+4));
        }
        Data = Data << ((uint32)64 - (startBit+signalLength));
        Data = Data >> ((uint32)64 - (signalLength));
        *((uint32*)SignalRef->ComSignalDataPtr)=(uint32)Data;
        break;
    case FLOAT64:
    case UINT64:
    case SINT64:
        if(type==NORMAL_SIGNAL)
        {
            Data=*((uint64*)ComIPdus[ComRxPduId].ComIPduDataPtr);
        }
        else
        {
            Data = *((uint64*)(((uint8*)ComIPdus[ComRxPduId].ComIPduDataPtr)+8));
        }
        Data = Data << ((uint32)64 - (startBit+signalLength));
        Data = Data >> ((uint32)64 - (signalLength));
        *((uint64*)SignalRef->ComSignalDataPtr)=(uint64)Data;
        break;
    case UINT16:
    case SINT16:
        if(type==NORMAL_SIGNAL)
        {
            Data=*((uint64*)ComIPdus[ComRxPduId].ComIPduDataPtr);
        }
        else
        {
            Data = *((uint64*)(((uint8*)ComIPdus[ComRxPduId].ComIPduDataPtr)+2));
        }
        Data = Data << ((uint32)64 - (startBit+signalLength));
        Data = Data >> ((uint32)64 - (signalLength));
        *((uint16*)SignalRef->ComSignalDataPtr)=(uint16)Data;
        break;
    case UINT8_N:
        if(type==NORMAL_SIGNAL)
        {
            memcpy(SignalRef->ComSignalDataPtr, (void*)&((uint8*)ComIPdus[ComRxPduId].ComIPduDataPtr)[startBit/(uint32)8],SignalRef->ComSignalLength);
        }
        else
        {
            memcpy((void*)(((uint8*)SignalRef->ComSignalDataPtr)+SignalRef->ComSignalLength), (void*)&((uint8*)ComIPdus[ComRxPduId].ComIPduDataPtr)[startBit/(uint32)8],SignalRef->ComSignalLength);
        }
        break;
    default:
        /*MISRA c*/
        break;
    }
}

LOCAL FUNC(void, memclass)
Com_WriteSignalDataToPduBuffer(CONSTP2CONST(ComSignal_type, memclass, ptrclass) signal,VAR(uint8, memclass) type)
{
    P2VAR(uint8, AUTOMATIC, ptrclass)    pdu;
    VAR(uint64, AUTOMATIC)               mask;
    VAR(uint32, AUTOMATIC)               position;
    VAR(uint32, AUTOMATIC)               length;

    pdu=ComIPdus[signal->ComIPduHandleId].ComIPduDataPtr;
    if(signal->ComSignalType==UINT8_N)/*TODO:check UINT8_DYN*/
    {
        if(type==NORMAL_SIGNAL)
        {
            memcpy((void*)(pdu+(signal->ComBitPosition/(uint32)8)),(void*) signal->ComSignalDataPtr,signal->ComSignalLength);
        }
        else
        {
            memcpy((void*)(pdu+(signal->ComBitPosition/(uint32)8)), (void*)(((uint8*)signal->ComSignalDataPtr)+signal->ComSignalLength),signal->ComSignalLength);
        }
    }
    else
    {
        mask=(uint64)0xffffffffffffffffu;
        position=signal->ComBitPosition;
        length=(uint32)signal->ComBitSize;
        mask=mask<<((uint32)64 - (position+length));
        mask=mask>>((uint32)64 - (length));
        mask=mask<<position;
        (*((uint64*)pdu))&=~mask;
        if(type==NORMAL_SIGNAL)
        {
            (*((uint64*)pdu))|=(((uint64)(*((uint64*)signal->ComSignalDataPtr))<<position))&mask;
        }
        else
        {
            switch(signal->ComSignalType)
            {
            case BOOLEAN:
            case SINT8:
            case UINT8:
                (*((uint64*)pdu))|=(((uint64)(*((uint64*)(((uint8*)signal->ComSignalDataPtr)+1)))<<position))&mask;
                break;
            case FLOAT32:
            case UINT32:
            case SINT32:
                (*((uint64*)pdu))|=(((uint64)(*((uint64*)(((uint8*)signal->ComSignalDataPtr)+4)))<<position))&mask;
                break;
            case FLOAT64:
            case UINT64:
            case SINT64:
                (*((uint64*)pdu))|=(((uint64)(*((uint64*)(((uint8*)signal->ComSignalDataPtr)+8)))<<position))&mask;
                break;
            case UINT16:
            case SINT16:
                (*((uint64*)pdu))|=(((uint64)(*((uint64*)(((uint8*)signal->ComSignalDataPtr)+2)))<<position))&mask;
                break;
            default:
                /*MISRA c*/
                break;
            }
        }
    }
}

LOCAL FUNC(void, memclass)
Com_PduUnpacking(VAR(PduIdType, memclass) ComRxPduId)
{
    VAR(uint8, AUTOMATIC) signalIndex;
    VAR(uint8, AUTOMATIC) signalGroupIndex;
    for ( signalIndex = (uint8)0; (ComIPdus[ComRxPduId].ComIPduNumOfSignals > signalIndex); signalIndex++)
    {
        if(ComIPdus[ComRxPduId].ComIPduSignalRef[signalIndex].ComUpdateBitEnabled)
        {
            if (CHECKBIT(ComIPdus[ComRxPduId].ComIPduDataPtr, ComIPdus[ComRxPduId].ComIPduSignalRef[signalIndex].ComUpdateBitPosition))
            {
                /*TODO:add this part in an inline function*/
                /*TODO:rename this function*/
                Com_ReadSignalDataFromPduBuffer(ComRxPduId,&ComIPdus[ComRxPduId].ComIPduSignalRef[signalIndex],NORMAL_SIGNAL);
                if (ComIPdus[ComRxPduId].ComIPduSignalRef->ComNotification != NULL_PTR)
                {
                    ComIPdus[ComRxPduId].ComIPduSignalRef->ComNotification();
                }
                else
                {
                    /* Following MISRA rules */
                }
            }
            else
            {

            }
        }
        else
        {
            Com_ReadSignalDataFromPduBuffer(ComRxPduId,&ComIPdus[ComRxPduId].ComIPduSignalRef[signalIndex],NORMAL_SIGNAL);
            if (ComIPdus[ComRxPduId].ComIPduSignalRef->ComNotification != NULL_PTR)
            {
                ComIPdus[ComRxPduId].ComIPduSignalRef->ComNotification();
            }
            else
            {
                /* Following MISRA rules */
            }
        }
    }
    for(signalGroupIndex=(uint8)0;signalGroupIndex<ComIPdus[ComRxPduId].ComIPduNumberOfSignalGroups;signalGroupIndex++)
    {
        if(ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComUpdateBitEnabled)
        {
            if (CHECKBIT(ComIPdus[ComRxPduId].ComIPduDataPtr, ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComIPduSignalRef[signalIndex].ComUpdateBitPosition))
            {
                for(signalIndex=(uint8)0;signalIndex<ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComIPduNumberOfGroupSignals;signalIndex++)
                {
                    Com_ReadSignalDataFromPduBuffer(ComRxPduId,&ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComIPduSignalRef[signalIndex],GROUP_SIGNAL);
                }
                if(ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification)
                {
                    ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification();
                }
            }
        }
        else
        {
            for(signalIndex=(uint8)0;signalIndex<ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComIPduNumberOfGroupSignals;signalIndex++)
            {
                Com_ReadSignalDataFromPduBuffer(ComRxPduId,&ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComIPduSignalRef[signalIndex],GROUP_SIGNAL);
            }
            if(ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification)
            {
                ComIPdus[ComRxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification();
            }
        }
    }
    privateIPdus[ComRxPduId].locked=(boolean)FALSE;
}

LOCAL FUNC(void, memclass)
Com_PackSignalsToPdu(VAR(uint16, memclass) ComIPuId)
{
    VAR(uint8, AUTOMATIC) signalIndex;
    VAR(uint8, AUTOMATIC) signalGroupIndex;
    CONSTP2VAR(ComIPdu_type, AUTOMATIC, ptrclass) IPdu;

    IPdu = GET_IPdu(ComIPuId);

    for( signalIndex =(uint8) 0 ; signalIndex < IPdu->ComIPduNumOfSignals ; signalIndex++ )
    {
        Com_WriteSignalDataToPduBuffer(&IPdu->ComIPduSignalRef[signalIndex] ,NORMAL_SIGNAL);
    }

    for(signalGroupIndex=(uint8)0;signalGroupIndex<IPdu->ComIPduNumberOfSignalGroups;signalGroupIndex++)
    {
        for( signalIndex =(uint8) 0 ; signalIndex < IPdu->ComIPduSignalGroupRef[signalGroupIndex].ComIPduNumberOfGroupSignals ; signalIndex++ )
        {
            Com_WriteSignalDataToPduBuffer(&IPdu->ComIPduSignalGroupRef[signalGroupIndex].ComIPduSignalRef[signalIndex],GROUP_SIGNAL);
        }
    }
}

FUNC(void, memclass)
Com_Init(CONSTP2VAR(ComConfig_type, memclass, ptrclass) config)
{
    /* 1- loop on IPDUs */
    VAR(uint16, AUTOMATIC) pduId;
    VAR(uint16, AUTOMATIC) signalIndex;
    VAR(uint16, AUTOMATIC) signalGroupIndex;
    VAR(uint16, AUTOMATIC) txIndex;
    VAR(uint32, AUTOMATIC) Counter;

    txIndex=(uint16)0;
    for ( pduId = (uint16)0; pduId<(uint16)COM_NUM_OF_IPDU; pduId++)
    {
        if(ComIPdus[pduId].ComIPduDirection==SEND)
        {
            privateIPdus[pduId].updated=(boolean)TRUE;
            txIPdusIds[txIndex]=pduId;
            txIndex++;
        }

        /* Initialize the memory with the default value.] */
        if (ComIPdus[pduId].ComIPduDirection == SEND)
        {
            for(Counter=(uint32)0x00;Counter<ComIPdus[pduId].ComIPduSize;Counter++)
            {
                *(uint8*)ComIPdus[pduId].ComIPduDataPtr=ComTxIPdus[pduId].ComTxIPduUnusedAreasDefault;
            }
        }

        /* For each signal in this PDU */
        for ( signalIndex = (uint16)0; (uint16)ComIPdus[pduId].ComIPduNumOfSignals > signalIndex; signalIndex++)
        {
            /* Check for the update Bit is enabled or disabled */
            if((ComIPdus[pduId].ComIPduSignalRef)[signalIndex].ComUpdateBitEnabled)
            {
                /* Clear update bits */
                CLEARBIT(ComIPdus[pduId].ComIPduDataPtr, (ComIPdus[pduId].ComIPduSignalRef)[signalIndex].ComUpdateBitPosition);
            }
            else
            {
                /* For MISRA rules */
            }
        }
        for ( signalGroupIndex = (uint16)0; (uint16)ComIPdus[pduId].ComIPduNumberOfSignalGroups > signalGroupIndex; signalGroupIndex++)
        {
            /* Check for the update Bit is enabled or disabled */
            if((ComIPdus[pduId].ComIPduSignalGroupRef)[signalGroupIndex].ComUpdateBitEnabled)
            {
                /* Clear update bits */
                CLEARBIT(ComIPdus[pduId].ComIPduDataPtr, (ComIPdus[pduId].ComIPduSignalGroupRef)[signalGroupIndex].ComUpdateBitPosition);
            }
            else
            {
                /* For MISRA rules */
            }
        }
    }
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
    for(txIndex=(uint16)0;txIndex<(uint16)COM_NUM_OF_TX_IPDU;txIndex++)
    {
        privateTxIPdus[txIndex].minimumDelayTimer=ComTxIPdus[txIndex].ComMinimumDelayTime;
    }
#endif
}

FUNC(void, memclass)
Com_MainFunctionRx(VAR(void, memclass))
{
    VAR(uint16, AUTOMATIC) mainRxNumberOfReceivedPdu;
    VAR(uint16, AUTOMATIC) pduId;
    VAR(uint16, AUTOMATIC) DeferredIndex;

    Rte_enterCriticaSection();
    mainRxNumberOfReceivedPdu=rxindicationNumberOfRecievedPdu;

    rxindicationNumberOfRecievedPdu=(uint16)0x00;
    /*TODO:flip rx indication buffer macro*/
    rxIndicationProcessingDeferredPduIndex^=(uint8)1;
    Rte_exitCriticalSection();
    for(DeferredIndex=(uint16)0x00 ; DeferredIndex<mainRxNumberOfReceivedPdu ; DeferredIndex++)
    {
        pduId =  rxDeferredPduArr[rxIndicationProcessingDeferredPduIndex^((uint8)0x01)][DeferredIndex];
        /* copy the deferred buffer to the actual pdu buffer */
        Com_PduUnpacking(pduId);
    }
}

FUNC(void, memclass)
Com_MainFunctionTx(VAR(void, memclass))
{
    AUTOMATIC CONSTP2VAR(ComIPdu_type, AUTOMATIC, ptrclass) IPdu;
    VAR(boolean, AUTOMATIC)                       mixed;
    VAR(boolean, AUTOMATIC)                       sent;
    VAR(uint16, AUTOMATIC)                        sendIPduIndex;
    VAR(uint16, AUTOMATIC)                        pendingIndex;
    VAR(uint16, AUTOMATIC)                        mainTxPendingTxNotificationsNumber;

    for ( sendIPduIndex = (uint16)0x00; sendIPduIndex<(uint16)COM_NUM_OF_TX_IPDU; sendIPduIndex++)
    {
        IPdu = GET_IPdu(txIPdusIds[sendIPduIndex]);
        mixed = (boolean)FALSE;
        sent  = (boolean)FALSE;

        switch(ComTxIPdus[IPdu->ComTxIPdu].ComTxModeMode)
        {
        /* if the transmission mode is mixed */
        case MIXED:
            /*Violate MISRA rules intentionally: MISRA C-2004:15.2 */
            mixed = (boolean)TRUE;
            /* no break because the mixed is periodic and direct */
            /* if the transmission mode is direct */
        case DIRECT:
            /*Violate MISRA rules intentionally: MISRA C-2004:15.2 */
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
            if(privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer < ComTxIPdus[IPdu->ComTxIPdu].ComMinimumDelayTime)
            {
                privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer+=COM_TX_TIMEBASE;
            }
#endif
            if(privateTxIPdus[IPdu->ComTxIPdu].numberOfRepetitionsLeft > (uint8)0x00)
            {
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
                if(privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer >= ComTxIPdus[IPdu->ComTxIPdu].ComMinimumDelayTime)
                {
#endif
                    if(Com_TriggerIPDUSend(txIPdusIds[sendIPduIndex])== E_OK)
                    {
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
                        privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer=(float32)0.0;
#endif
                        privateTxIPdus[IPdu->ComTxIPdu].numberOfRepetitionsLeft--;
                        sent=(boolean)TRUE;
                    }
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
                }
#endif
            }
            if(mixed==(boolean)FALSE)/* in case the Pdu is mixed don't break */
            {
                break;
            }
            /* if the transmission mode is periodic */
        case PERIODIC:
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
            if(mixed!=(boolean)FALSE)
            {
                if(privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer < ComTxIPdus[IPdu->ComTxIPdu].ComMinimumDelayTime)
                {
                    privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer+=COM_TX_TIMEBASE;
                }
            }
#endif
            if((privateTxIPdus[IPdu->ComTxIPdu].remainingTimePeriod<=(float32)0.0)&&(sent==(boolean)FALSE))
            {
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
                if(privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer >= ComTxIPdus[IPdu->ComTxIPdu].ComMinimumDelayTime)
                {
#endif
                    if(Com_TriggerIPDUSend(txIPdusIds[sendIPduIndex]) == E_OK)
                    {
                        privateTxIPdus[IPdu->ComTxIPdu].remainingTimePeriod = \
                                (float32)ComTxIPdus[IPdu->ComTxIPdu].ComTxModeTimePeriod;
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
                        privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer=(float32)0.0;
#endif
                    }
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
                }
                else
                {
#if COM_DEV_ERROR_DETECT
                    Det_ReportError(COM_MODULE_ID, COM_INSTANCE_ID, COM_MAIN_FUNCTION_TX_ID, COM_E_SKIPPED_TRANSMISSION);
#endif
                }
#endif
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                if(sent==(boolean)TRUE)
                {
                    Det_ReportError(COM_MODULE_ID, COM_INSTANCE_ID, COM_MAIN_FUNCTION_TX_ID, COM_E_SKIPPED_TRANSMISSION);
                }
                else
                {

                }
#endif
            }
            if (privateTxIPdus[IPdu->ComTxIPdu].remainingTimePeriod > (float32)0.0)
            {
                privateTxIPdus[IPdu->ComTxIPdu].remainingTimePeriod = \
                        privateTxIPdus[IPdu->ComTxIPdu].remainingTimePeriod - COM_TX_TIMEBASE;
            }
            break;
        default:
            break;
        }
    }
    Rte_enterCriticaSection();
    mainTxPendingTxNotificationsNumber=pendingTxNotificationsNumber;
    pendingTxNotificationsNumber=(uint16)0x00;
    pendingTxNotificationsBufferIndex^=(uint8)0x01;
    Rte_exitCriticalSection();

    for ( pendingIndex = (uint16)0x00; pendingIndex<mainTxPendingTxNotificationsNumber; pendingIndex++)
    {
        pendingTxNotifications[pendingTxNotificationsBufferIndex^(uint8)0x01][pendingIndex]();
    }
}

/* Updates the signal object identified by SignalId with the signal referenced by the SignalDataPtr parameter */
FUNC(uint8, memclass)
Com_SendSignal( VAR(Com_SignalIdType, memclass) SignalId, CONSTP2VAR(void, memclass, ptrclass) SignalDataPtr )
{
    VAR(Std_ReturnType, AUTOMATIC)                   result;
    VAR(uint8, AUTOMATIC)                            byteIndex;
    VAR(boolean, AUTOMATIC)                          signalUpdated;
    /* Get signal of "SignalId" */
    CONSTP2VAR(ComSignal_type, AUTOMATIC, ptrclass)  Signal;

    /*Get IPdu of this signal */
    AUTOMATIC CONSTP2VAR(ComIPdu_type, AUTOMATIC, ptrclass)    IPdu;
    AUTOMATIC P2VAR(privateIPdu_type, AUTOMATIC, ptrclass)     privateIPdu;
    result=E_OK;

    /* validate signalID */
    if(!validateSignalID(SignalId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_SEND_SIGNAL_ID,COM_E_PARAM);
#endif
        result=E_NOT_OK;
    }
    else
    {
        /* Get signal of "SignalId" */
        Signal = GET_Signal(SignalId);
        /*Get IPdu of this signal */
        IPdu =GET_IPdu(Signal->ComIPduHandleId);
        if(IPdu->ComIPduDirection==SEND)
        {
            if(SignalDataPtr)
            {
                privateIPdu=&privateIPdus[Signal->ComIPduHandleId];
                signalUpdated=(boolean)FALSE;
                switch(Signal->ComTransferProperty)
                {
                case TRIGGERED_WITHOUT_REPETITION:
                    signalUpdated=(boolean)TRUE;
                    break;
#if 0 /*repitition is not supported now*/
                case TRIGGERED:
                    break;
                case TRIGGERED_ON_CHANGE:
                    break;
#endif
                case TRIGGERED_ON_CHANGE_WITHOUT_REPETITION:
                    switch(Signal->ComSignalType)
                    {
                    case BOOLEAN:
                    case UINT8:
                    case SINT8:
                        if((*((uint8*)(Signal->ComSignalDataPtr)))!=(*((const uint8*)SignalDataPtr)))
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        break;
                    case UINT16:
                    case SINT16:
                        if((*((uint16*)(Signal->ComSignalDataPtr)))!=(*((const uint16*)SignalDataPtr)))
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        break;
                    case FLOAT32:
                    case UINT32:
                    case SINT32:
                        if((*((uint32*)(Signal->ComSignalDataPtr)))!=(*((const uint32*)SignalDataPtr)))
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        break;
                    case FLOAT64:
                    case UINT64:
                    case SINT64:
                        if((*((uint64*)(Signal->ComSignalDataPtr)))!=(*((const uint64*)SignalDataPtr)))
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        else
                        {

                        }
                        break;
                    case UINT8_N:
                        for(byteIndex=(uint8)0x00;byteIndex<Signal->ComSignalLength;byteIndex++)
                        {
                            if(((uint8*)(Signal->ComSignalDataPtr))[byteIndex]!=((const uint8*)SignalDataPtr)[byteIndex])
                            {
                                signalUpdated=(boolean)TRUE;
                                break;
                            }
                            else
                            {

                            }
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                    default:
                        break;
                }

                /* update the Signal buffer with the signal data */
                if((boolean)TRUE==signalUpdated)
                {
                    privateTxIPdus[IPdu->ComTxIPdu].numberOfRepetitionsLeft = (uint8)0x01;
                    privateIPdu->updated=(boolean)TRUE;
                    Rte_enterCriticaSection();
                    Com_WriteSignalDataToSignalBuffer(SignalId, SignalDataPtr);
                    Rte_exitCriticalSection();
                    if(Signal->ComUpdateBitEnabled!=(boolean)FALSE)
                    {
                        /* Set the update bit of this signal */
                        SETBIT(IPdu->ComIPduDataPtr, Signal->ComUpdateBitPosition);
                    }
                    else
                    {

                    }
                }
                else
                {

                }
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_SEND_SIGNAL_ID,COM_E_PARAM_POINTER);
#endif
                result=E_NOT_OK;
            }
        }
        else
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_SEND_SIGNAL_ID,COM_E_PARAM);
#endif
            result=E_NOT_OK;
        }
    }
    return result;
}

/* Copies the data of the signal identified by SignalId to the location specified by SignalDataPtr */
FUNC(uint8, memclass)
Com_ReceiveSignal( VAR(Com_SignalIdType, memclass) SignalId, P2VAR(void, memclass, ptrclass) SignalDataPtr)
{
    /*Return uint8 result*/
    VAR(uint8, AUTOMATIC) result = E_OK;
    /* validate signalID */
    if(!validateSignalID(SignalId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RECEIVE_SIGNAL_ID,COM_E_PARAM);
#endif
        result = E_NOT_OK;
    }
    else
    {
        if(ComIPdus[ComSignals[SignalId].ComIPduHandleId].ComIPduDirection == RECEIVE)
        {
            if(SignalDataPtr)
            {
                /*Extract the signal data from the Pdu buffer*/
                Rte_enterCriticaSection();
                Com_ReadSignalDataFromSignalBuffer(SignalId, SignalDataPtr);
                Rte_exitCriticalSection();
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RECEIVE_SIGNAL_ID,COM_E_PARAM_POINTER);
#endif
                result = E_NOT_OK;
            }
        }
        else
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RECEIVE_SIGNAL_ID,COM_E_PARAM);
#endif
            result = E_NOT_OK;
        }
    }
    return result;
}

FUNC(BufReq_ReturnType, memclass)
Com_CopyTxData(VAR(PduIdType, memclass) PduId, CONSTP2VAR(PduInfoType,memclass,ptrclass) info, CONSTP2VAR(RetryInfoType, memclass, ptrclass) retry, P2VAR(PduLengthType, memclass, ptrclass) availableDataPtr )
{
    /*Return BufReq_ReturnType result */
    VAR(BufReq_ReturnType, AUTOMATIC)  result = BUFREQ_OK;
    /*Pointer to variable for the source (Pdu buffer)*/
    AUTOMATIC P2VAR(uint8, AUTOMATIC, ptrclass) source;
    /*Validate the Pdu is available*/
    if(!validateSignalID(PduId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_COPY_TX_DATA_ID,COM_E_PARAM);
#endif
        result = BUFREQ_E_NOT_OK;
    }
    else
    {
        if((ComIPdus[PduId].ComIPduDirection != SEND)&&(ComIPdus[PduId].ComIPduType != TP))
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_COPY_TX_DATA_ID,COM_E_PARAM);
#endif
            result = BUFREQ_E_NOT_OK;
        }
        else
        {
            if(info&&availableDataPtr)
            {
                if(((uint32)privateIPdus[PduId].CurrentPosition + info->SduLength) <= (uint32)(ComIPdus[PduId].ComIPduSize))
                {
                    /*Point to the Pdu data buffer*/
                    source = (uint8*)ComIPdus[PduId].ComIPduDataPtr;
                    /*Move to required next data*/
                    source += privateIPdus[PduId].CurrentPosition;
                    /*Lock the buffer*/
                    privateIPdus[PduId].locked=(boolean)TRUE;
                    /*Copy the data from the Pdu data buffer to the lower layer data buffer*/
                    memcpy((void*) info->SduDataPtr, (void*)source, info->SduLength);
                    /*Move forward to the next data position*/
                    privateIPdus[PduId].CurrentPosition += info->SduLength;
                    /*Calculate the new data buffer available*/
                    *availableDataPtr = (PduLengthType)ComIPdus[PduId].ComIPduSize - (PduLengthType)privateIPdus[PduId].CurrentPosition;
                    result = BUFREQ_OK;
                }
                else
                {
                    result = BUFREQ_E_OVFL;
                }
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_COPY_TX_DATA_ID,COM_E_PARAM_POINTER);
#endif
                result = BUFREQ_E_NOT_OK;
            }
        }
    }
    return result;
}

FUNC(BufReq_ReturnType, memclass)
Com_CopyRxData(VAR(PduIdType, memclass)  PduID, CONSTP2VAR(PduInfoType, memclass, ptrclass) info, P2VAR(PduLengthType, memclass, ptrclass) bufferSizePtr )
{
    /*Return BufReq_ReturnType result*/
    VAR(BufReq_ReturnType, AUTOMATIC) result = BUFREQ_OK;
    /*Pointer to variable for the destination (Pdu buffer)*/
    AUTOMATIC P2VAR(uint8, AUTOMATIC, ptrclass) destination;
    /*Validate the Pdu is available*/
    if(!validateSignalID(PduID))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_COPY_RX_DATA_ID,COM_E_PARAM);
#endif
        result = BUFREQ_E_NOT_OK;
    }
    else
    {
        if((ComIPdus[PduID].ComIPduDirection != RECEIVE)&&(ComIPdus[PduID].ComIPduType !=TP))
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_COPY_RX_DATA_ID,COM_E_PARAM);
#endif
            result = BUFREQ_E_NOT_OK;
        }
        else
        {
            if(info&&bufferSizePtr)
            {
                if((uint32)ComIPdus[PduID].ComIPduSize - (uint32)privateIPdus[PduID].CurrentPosition >= info->SduLength )
                {
                    /*Point to the Pdu data buffer*/
                    destination = (uint8 *) ComIPdus[PduID].ComIPduDataPtr;
                    /*Move to required next data*/
                    destination += privateIPdus[PduID].CurrentPosition;
                    if((info->SduDataPtr != NULL_PTR) && (info->SduLength !=(uint32)0x00))
                    {
                        /*Copy the data from the lower layer data buffer  buffer to the Pdu data*/
                        memcpy(destination, info->SduDataPtr, info->SduLength);
                        /*Move forward to the next data position*/
                        privateIPdus[PduID].CurrentPosition += info->SduLength;
                        /*Calculate the new data buffer available*/
                        *bufferSizePtr = (uint32)ComIPdus[PduID].ComIPduSize - privateIPdus[PduID].CurrentPosition;
                    }
                    result = BUFREQ_OK;
                }
                else
                {
                    result = BUFREQ_E_NOT_OK;
                }
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_COPY_RX_DATA_ID,COM_E_PARAM_POINTER);
#endif
                result = E_NOT_OK;
            }
        }
    }
    return result;
}

FUNC(Std_ReturnType, memclass)
Com_TriggerIPDUSend(VAR(PduIdType, memclass) PduId)
{
    AUTOMATIC CONSTP2VAR(ComIPdu_type, AUTOMATIC, ptrclass) IPdu;
    VAR(PduInfoType, AUTOMATIC)  PduInfoPackage;
    VAR(uint8, AUTOMATIC)  signalIndex;
    VAR(uint8, AUTOMATIC)  signalGroupIndex;
    /*Return Std_ReturnType result*/
    VAR(Std_ReturnType, AUTOMATIC)  result;
    AUTOMATIC P2VAR(privateIPdu_type, AUTOMATIC, ptrclass) privateIPdu;

    result=E_OK;

    if(!validateSignalID(PduId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TRIGGER_IPDU_SEND_ID,COM_E_PARAM);
#endif
        result = E_NOT_OK;
    }
    else
    {
        if(ComIPdus[PduId].ComIPduDirection!= SEND)
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TRIGGER_IPDU_SEND_ID,COM_E_PARAM);
#endif
            result = E_NOT_OK;
        }
        else
        {
            privateIPdu=&privateIPdus[PduId];
            IPdu=GET_IPdu(PduId);
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
            if(privateTxIPdus[IPdu->ComTxIPdu].minimumDelayTimer >= ComTxIPdus[IPdu->ComTxIPdu].ComMinimumDelayTime)
            {
#endif

                if (privateIPdu->locked)
                {
#if COM_DEV_ERROR_DETECT
                    Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TRIGGER_IPDU_SEND_ID,COM_E_SKIPPED_TRANSMISSION);
#endif
                    result=E_NOT_OK;
                }
                else
                {
                    privateIPdu->locked=(boolean)TRUE;
                    if(privateIPdu->updated)
                    {
                        privateIPdu->updated=(boolean)FALSE;
                        Com_PackSignalsToPdu(PduId);
                    }
                    IPdu = GET_IPdu(PduId);
                    PduInfoPackage.SduDataPtr = (uint8 *)IPdu->ComIPduDataPtr;
                    PduInfoPackage.SduLength = (uint32)IPdu->ComIPduSize;
                    if (PduR_ComTransmit(PduId, &PduInfoPackage) != (Std_ReturnType)E_OK)
                    {
                        result=E_NOT_OK;
                    }
                    else
                    {

                    }
                    if(IPdu->ComIPduType==NORMAL)
                    {
                        privateIPdu->locked=(boolean)FALSE;
                    }
                    else
                    {

                    }
                    if((ComTxIPdus[IPdu->ComTxIPdu].ComTxIPduClearUpdateBit == TRIGGER_TRANSMIT)||\
                            ((ComTxIPdus[IPdu->ComTxIPdu].ComTxIPduClearUpdateBit == TRANSMIT)&&(result==E_OK)))
                    {
                        for ( signalIndex = (uint8)0x00 ; signalIndex < IPdu->ComIPduNumOfSignals ; signalIndex++ )
                        {
                            if(IPdu->ComIPduSignalRef[signalIndex].ComUpdateBitEnabled!=(boolean)FALSE)/*Update bit is enabled*/
                            {
                                CLEARBIT(IPdu->ComIPduDataPtr, IPdu->ComIPduSignalRef[signalIndex].ComUpdateBitPosition);
                            }
                        }
                        for(signalGroupIndex=(uint8)0;signalGroupIndex<IPdu->ComIPduNumberOfSignalGroups;signalGroupIndex++)
                        {
                            if(IPdu->ComIPduSignalGroupRef[signalGroupIndex].ComUpdateBitEnabled!=(boolean)FALSE)
                            {
                                CLEARBIT(IPdu->ComIPduDataPtr,IPdu->ComIPduSignalGroupRef[signalGroupIndex].ComUpdateBitPosition);
                            }
                        }
                    }
                }
#if COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TRIGGER_IPDU_SEND_ID,COM_E_SKIPPED_TRANSMISSION);
#endif
                result=E_NOT_OK;
            }
#endif
        }
    }
    return result;
}

FUNC(void, memclass)
Com_RxIndication(VAR(PduIdType, memclass) ComRxPduId,CONSTP2VAR(PduInfoType, memclass, ptrclass) PduInfoPtr)
{
    if(!validateSignalID(ComRxPduId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RX_INDICATION_ID,COM_E_PARAM);
#endif
    }
    else
    {
        if((ComIPdus[ComRxPduId].ComIPduDirection != RECEIVE )&&(ComIPdus[ComRxPduId].ComIPduType != NORMAL))
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RX_INDICATION_ID,COM_E_PARAM);
#endif
        }
        else
        {
            if(PduInfoPtr)
            {
                Rte_enterCriticaSection();
                memcpy(ComIPdus[ComRxPduId].ComIPduDataPtr, PduInfoPtr->SduDataPtr, (uint32)ComIPdus[ComRxPduId].ComIPduSize);
                Rte_exitCriticalSection();
                if(ComIPdus[ComRxPduId].ComIPduSignalProcessing == IMMEDIATE)
                {
                    Com_PduUnpacking(ComRxPduId);
                }
                else
                {
                    Rte_enterCriticaSection();
                    rxDeferredPduArr[rxIndicationProcessingDeferredPduIndex][rxindicationNumberOfRecievedPdu]=ComRxPduId;
                    rxindicationNumberOfRecievedPdu++;
                    Rte_exitCriticalSection();
                }
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RX_INDICATION_ID,COM_E_PARAM_POINTER);
#endif
            }
        }
    }
}

FUNC(BufReq_ReturnType, memclass)
Com_StartOfReception(VAR(PduIdType, memclass) PduId,CONSTP2VAR(PduInfoType, memclass, ptrclass) info,VAR(PduLengthType, memclass) TpSduLength,P2VAR(PduLengthType, memclass, ptrclass) bufferSizePtr)
{
    VAR(BufReq_ReturnType, AUTOMATIC) result=BUFREQ_OK;
    if(!validateSignalID(PduId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_START_OF_RECEPTION_ID,COM_E_PARAM);
#endif
        result = BUFREQ_E_NOT_OK;
    }
    else
    {
        if((ComIPdus[PduId].ComIPduDirection!=RECEIVE) && (ComIPdus[PduId].ComIPduType != TP))
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_START_OF_RECEPTION_ID,COM_E_PARAM);
#endif
            result = BUFREQ_E_NOT_OK;
        }
        else
        {
            if(info&&bufferSizePtr)
            {
                //making sure that the buffer is unlocked
                if(!privateIPdus[PduId].locked)
                {
                    //making sure that we have the enough space for the sdu
                    if(ComIPdus[PduId].ComIPduSize>=TpSduLength)
                    {
                        privateIPdus[PduId].locked=(boolean)TRUE;
                        /* Initialize the current position */
                        privateIPdus[PduId].CurrentPosition = (uint32)0;
                        ///return the available buffer size
                        *bufferSizePtr=(uint32)ComIPdus[PduId].ComIPduSize;
                        result = BUFREQ_OK;
                    }
                    else
                    {
                        result = BUFREQ_E_OVFL;
                    }
                }
                else
                {
                    /*TODO: check in SWS of TP*/
                    result = BUFREQ_E_BUSY;
                }
            }
            else
            {
#if COM_DEV_ERROR_DETECT
                Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_START_OF_RECEPTION_ID,COM_E_PARAM_POINTER);
#endif
                result = BUFREQ_E_NOT_OK;
            }
        }
    }
    return result;
}

FUNC(void, memclass)
Com_TpRxIndication(VAR(PduIdType, memclass) ComRxPduId,VAR(Std_ReturnType, memclass) Result)
{
    if(!validateSignalID(ComRxPduId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TP_RX_INDICATION_ID,COM_E_PARAM);
#endif
    }
    else
    {
        if((ComIPdus[ComRxPduId].ComIPduDirection != RECEIVE) && (ComIPdus[ComRxPduId].ComIPduType != TP))
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TP_RX_INDICATION_ID,COM_E_PARAM);
#endif
        }
        else
        {
            if (Result == E_OK)
            {
                if(ComIPdus[ComRxPduId].ComIPduSignalProcessing == IMMEDIATE)
                {
                    Com_PduUnpacking(ComRxPduId);
                }
                else
                {
                    Rte_enterCriticaSection();
                    rxDeferredPduArr[rxIndicationProcessingDeferredPduIndex][rxindicationNumberOfRecievedPdu]=ComRxPduId;
                    rxindicationNumberOfRecievedPdu++;
                    Rte_exitCriticalSection();
                }
            }
            else
            {
                privateIPdus[ComRxPduId].locked=(boolean)FALSE;
            }
        }
    }
}

LOCAL FUNC(void, memclass)
privateGeneralTxConfirmation(VAR(PduIdType, memclass) TxPduId)
{
    VAR(uint8, AUTOMATIC)  signalIndex;
    VAR(uint8, AUTOMATIC)  signalGroupIndex;
    for ( signalIndex = (uint8)0x00 ; signalIndex < ComIPdus[TxPduId].ComIPduNumOfSignals ; signalIndex++ )
    {
        /*Update bit is enabled*/
        if((ComIPdus[TxPduId].ComIPduSignalRef[signalIndex].ComUpdateBitEnabled!=(boolean)FALSE)&&(ComTxIPdus[ComIPdus[TxPduId].ComTxIPdu].ComTxIPduClearUpdateBit==CONFIRMATION))
        {
            CLEARBIT(ComIPdus[TxPduId].ComIPduDataPtr, ComIPdus[TxPduId].ComIPduSignalRef[signalIndex].ComUpdateBitPosition);
        }
        if(ComIPdus[TxPduId].ComIPduSignalProcessing==IMMEDIATE)
        {
            if(ComIPdus[TxPduId].ComIPduSignalRef[signalIndex].ComNotification)
            {
                ComIPdus[TxPduId].ComIPduSignalRef[signalIndex].ComNotification();
            }
        }
        else
        {
            if(ComIPdus[TxPduId].ComIPduSignalRef[signalIndex].ComNotification)
            {
                Rte_enterCriticaSection();
                pendingTxNotifications[pendingTxNotificationsBufferIndex][pendingTxNotificationsNumber]=\
                        ComIPdus[TxPduId].ComIPduSignalRef[signalIndex].ComNotification;
                pendingTxNotificationsNumber++;
                Rte_exitCriticalSection();
            }
        }
    }
    for(signalGroupIndex=(uint8)0;signalGroupIndex<ComIPdus[TxPduId].ComIPduNumberOfSignalGroups;signalGroupIndex++)
    {
        /*Update bit is enabled*/
        if((ComIPdus[TxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComUpdateBitEnabled!=(boolean)FALSE)&&(ComTxIPdus[ComIPdus[TxPduId].ComTxIPdu].ComTxIPduClearUpdateBit==CONFIRMATION))
        {
            CLEARBIT(ComIPdus[TxPduId].ComIPduDataPtr, ComIPdus[TxPduId].ComIPduSignalRef[signalIndex].ComUpdateBitPosition);
        }
        if(ComIPdus[TxPduId].ComIPduSignalProcessing==IMMEDIATE)
        {
            if(ComIPdus[TxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification)
            {
                ComIPdus[TxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification();
            }
        }
        else
        {
            if(ComIPdus[TxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification)
            {
                Rte_enterCriticaSection();
                pendingTxNotifications[pendingTxNotificationsBufferIndex][pendingTxNotificationsNumber]=\
                        ComIPdus[TxPduId].ComIPduSignalGroupRef[signalGroupIndex].ComNotification;
                pendingTxNotificationsNumber++;
                Rte_exitCriticalSection();
            }
        }
    }
}

FUNC(void, memclass)
Com_TpTxConfirmation(VAR(PduIdType, memclass) TxPduId, VAR(Std_ReturnType, memclass) result)
{
    if(!validateSignalID(TxPduId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TP_TX_CONFIRMATION_ID,COM_E_PARAM);
#endif
    }
    else
    {
        if((ComIPdus[TxPduId].ComIPduDirection!=SEND)&&(ComIPdus[TxPduId].ComIPduType != TP))
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TP_TX_CONFIRMATION_ID,COM_E_PARAM);
#endif
        }
        else
        {
            if(result==E_OK)
            {
                privateIPdus[TxPduId].locked=(uint8)FALSE;
                privateGeneralTxConfirmation(TxPduId);
            }
            else
            {
                privateIPdus[TxPduId].locked=(uint8)FALSE;
            }
        }
    }
}

FUNC(void, memclass)
Com_TxConfirmation(VAR(PduIdType, memclass) TxPduId, VAR(Std_ReturnType, memclass) result)
{
    if(!validateSignalID(TxPduId))
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TX_CONFIRMATION_ID,COM_E_PARAM);
#endif
    }
    else
    {
        if((ComIPdus[TxPduId].ComIPduDirection!=SEND)&&(ComIPdus[TxPduId].ComIPduType != NORMAL))
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_TX_CONFIRMATION_ID,COM_E_PARAM);
#endif
        }
        else
        {
            if(result==E_OK)
            {
                privateGeneralTxConfirmation(TxPduId);
            }
        }
    }
}

FUNC(uint8, memclass)
Com_SendSignalGroup(VAR(Com_SignalGroupIdType, memclass) SignalGroupId )
{
    VAR(uint8, AUTOMATIC)  result;
    VAR(uint8, AUTOMATIC)  signalIndex;
    VAR(uint32, AUTOMATIC)  byteIndex;
    VAR(boolean, AUTOMATIC) signalUpdated;

    result=E_OK;
    if(SignalGroupId<(Com_SignalGroupIdType)COM_NUM_OF_GROUP_SIGNAL)
    {
        if(ComIPdus[ComSignalGroups[SignalGroupId].ComIPduHandleId].ComIPduDirection==SEND)
        {
            signalUpdated=(boolean)FALSE;
            Rte_enterCriticaSection();
            for(signalIndex=(uint8)0;signalIndex< ComSignalGroups[SignalGroupId].ComIPduNumberOfGroupSignals;signalIndex++)
            {
                switch(ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalType)
                {
                case BOOLEAN:
                case SINT8:
                case UINT8:
                    ((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]=\
                    ((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER];
                    break;
                case FLOAT32:
                case UINT32:
                case SINT32:
                    ((uint32*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]=\
                    ((uint32*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER];
                    break;
                case FLOAT64:
                case UINT64:
                case SINT64:
                    ((uint64*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]=\
                    ((uint64*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER];
                    break;
                case UINT16:
                case SINT16:
                    ((uint16*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]=\
                    ((uint16*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER];
                    break;
                case UINT8_N:
                    memcpy((void*)(((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)+\
                            ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength)\
                            ,ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr,\
                            ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength);
                    break;
                default:
                    break;
                }
            }
            Rte_exitCriticalSection();
            switch(ComSignalGroups[SignalGroupId].ComTransferProperty)
            {
            case TRIGGERED_WITHOUT_REPETITION:
                signalUpdated=(boolean)TRUE;
                break;
#if 0 /*repitition is not supported now*/
            case TRIGGERED:
                break;
            case TRIGGERED_ON_CHANGE:
                break;
#endif
            case TRIGGERED_ON_CHANGE_WITHOUT_REPETITION:
                for(signalIndex=(uint8)0;signalIndex< ComSignalGroups[SignalGroupId].ComIPduNumberOfGroupSignals;signalIndex++)
                {
                    switch(ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalType)
                    {
                    case BOOLEAN:
                    case SINT8:
                    case UINT8:
                        if(((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]!=\
                                ((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER])
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        break;
                    case FLOAT32:
                    case UINT32:
                    case SINT32:
                        if(((uint32*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]!=\
                                ((uint32*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER])
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        break;
                    case FLOAT64:
                    case UINT64:
                    case SINT64:
                        if(((uint64*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]!=
                                ((uint64*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER])
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        break;
                    case UINT16:
                    case SINT16:
                        if(((uint16*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER]!=
                                ((uint16*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER])
                        {
                            signalUpdated=(boolean)TRUE;
                        }
                        break;
                    case UINT8_N:
                        memcpy((void*)(((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)+\
                                ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength)\
                                ,ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr,\
                                ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength);
                        for(byteIndex=(uint32)0;byteIndex<ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength;byteIndex++)
                        {
                            if((((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)+\
                                    ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength)[byteIndex]!=\
                                    ((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[byteIndex])

                            {
                                signalUpdated=(boolean)TRUE;
                                break;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                    if(signalUpdated==(boolean)TRUE)
                    {
                        break;
                    }
                }
                break;
            default:
                break;
            }
            if((boolean)TRUE==signalUpdated)
            {
                privateTxIPdus[ComIPdus[ComSignalGroups[SignalGroupId].ComIPduHandleId].ComTxIPdu].numberOfRepetitionsLeft = (uint8)0x01;
                privateIPdus[ComSignalGroups[SignalGroupId].ComIPduHandleId].updated=(boolean)TRUE;
                if(ComSignalGroups[SignalGroupId].ComUpdateBitEnabled!=(boolean)FALSE)
                {
                    /* Set the update bit of this signal */
                    SETBIT(ComIPdus[ComSignalGroups[SignalGroupId].ComIPduHandleId].ComIPduDataPtr,ComSignalGroups[SignalGroupId].ComUpdateBitPosition);
                }
                else
                {

                }
            }
            else
            {

            }
        }
        else
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_SEND_SIGNAL_GROUP_ID,COM_E_PARAM);
#endif
            result=E_NOT_OK;
        }
    }
    else
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_SEND_SIGNAL_GROUP_ID,COM_E_PARAM);
#endif
        result=E_NOT_OK;
    }
    return result;
}

FUNC(uint8, memclass)
Com_ReceiveSignalGroup(VAR(Com_SignalGroupIdType, memclass) SignalGroupId )
{
    VAR(uint8, AUTOMATIC) result;
    VAR(uint8, AUTOMATIC) signalIndex;

    result=E_OK;
    if(SignalGroupId<(Com_SignalGroupIdType)COM_NUM_OF_GROUP_SIGNAL)
    {
        if(ComIPdus[ComSignalGroups[SignalGroupId].ComIPduHandleId].ComIPduDirection==RECEIVE)
        {
            Rte_enterCriticaSection();
            for(signalIndex=(uint8)0;signalIndex< ComSignalGroups[SignalGroupId].ComIPduNumberOfGroupSignals;signalIndex++)
            {
                switch(ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalType)
                {
                case BOOLEAN:
                case SINT8:
                case UINT8:
                    ((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER]=\
                    ((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER];
                    break;
                case FLOAT32:
                case UINT32:
                case SINT32:
                    ((uint32*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER]=\
                    ((uint32*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER];
                    break;
                case FLOAT64:
                case UINT64:
                case SINT64:
                    ((uint64*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER]=\
                    ((uint64*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER];
                    break;
                case UINT16:
                case SINT16:
                    ((uint16*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[SHADOW_BUFFER]=\
                    ((uint16*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)[REAL_BUFFER];
                    break;
                case UINT8_N:
                    memcpy(ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr,\
                           (void*)(((uint8*)ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalDataPtr)+\
                                   ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength),\
                                   ComSignalGroups[SignalGroupId].ComIPduSignalRef[signalIndex].ComSignalLength);
                    break;
                default:
                    break;
                }
            }
            Rte_exitCriticalSection();
        }
        else
        {
#if COM_DEV_ERROR_DETECT
            Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RECEIVE_SIGNAL_GROUP_ID,COM_E_PARAM);
#endif
            result=E_NOT_OK;
        }
    }
    else
    {
#if COM_DEV_ERROR_DETECT
        Det_ReportError(COM_MODULE_ID,COM_INSTANCE_ID,COM_RECEIVE_SIGNAL_GROUP_ID,COM_E_PARAM);
#endif
        result=E_NOT_OK;
    }
    return result;
}
