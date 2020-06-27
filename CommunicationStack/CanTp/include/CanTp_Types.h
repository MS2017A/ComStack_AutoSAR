/********************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Types.h                                                              *
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

#ifndef CANTP_TYPES_H_
#define CANTP_TYPES_H_

#include "CanTp_Cfg.h"


#define SEGMENT_NUMBER_MASK                            (uint8)0x0F

#define MAX_FRAME_BYTES                                 8U

#define MAX_PAYLOAD_STANDRAD_SF                         7U
#define MAX_PAYLOAD_STANDRAD_FF                         6U
#define MAX_PAYLOAD_STANDRAD_CF                         7U

#define MAX_PAYLOAD_EXTENDED_SF                         6U
#define MAX_PAYLOAD_EXTENDED_FF                         5U
#define MAX_PAYLOAD_EXTENDED_CF                         6U

typedef uint8 CanTp_PaddingActivationType;
typedef uint8 CanTp_InternalStateType;

#define CANTP_OFF                                       0U
#define CANTP_ON                                        1U


typedef uint8 CanTp_TransferInstanceMode;
#define CANTP_RX_WAIT                                   (CanTp_TransferInstanceMode)0x00
#define CANTP_RX_PROCESSING                             (CanTp_TransferInstanceMode)0x01
#define CANTP_TX_WAIT                                   (CanTp_TransferInstanceMode)0x02
#define CANTP_TX_PROCESSING                             (CanTp_TransferInstanceMode)0x03

typedef uint8 FrameType;
#define INVALID_FRAME                                                    (FrameType)0x00
#define SINGLE_FRAME                                                     (FrameType)0x01
#define FIRST_FRAME                                                      (FrameType)0x02
#define CONSECUTIVE_FRAME                                                (FrameType)0x03
#define FLOW_CONTROL_CTS_FRAME                                           (FrameType)0x04
#define FLOW_CONTROL_WAIT_FRAME                                          (FrameType)0x05
#define FLOW_CONTROL_OVERFLOW_FRAME                                      (FrameType)0x06

typedef uint8 TransferStateTypes;
#define UNINITIALIZED                                           (TransferStateTypes)0x00
#define IDLE                                                    (TransferStateTypes)0x01
#define SF_OR_FF_RECEIVED_WAITING_PDUR_BUFFER                   (TransferStateTypes)0x02
#define RX_WAIT_CONSECUTIVE_FRAME                               (TransferStateTypes)0x03
#define RX_WAIT_SDU_BUFFER                                      (TransferStateTypes)0x04
#define TX_WAIT_STMIN                                           (TransferStateTypes)0x05
#define TX_WAIT_TRANSMIT                                        (TransferStateTypes)0x06
#define TX_WAIT_FLOW_CONTROL                                    (TransferStateTypes)0x07
#define TX_WAIT_TX_CONFIRMATION                                 (TransferStateTypes)0x08
#define RX_WAIT_TX_CONFIRMATION									(TransferStateTypes)0x09

typedef uint8 ChannelModeType;
#define CANTP_MODE_FULL_DUPLEX                                     (ChannelModeType)0x00
#define CANTP_MODE_HALF_DUPLEX                                     (ChannelModeType)0x01


typedef uint8 TaType;
#define CANTP_FUNCTIONAL                                                    (TaType)0x00
#define CANTP_PHYSICAL                                                      (TaType)0x01

typedef uint8 AddressingFormatType;
#define CANTP_EXTENDED                                        (AddressingFormatType)0x00
#define CANTP_MIXED                                           (AddressingFormatType)0x01
#define CANTP_MIXED29BIT                                      (AddressingFormatType)0x02
#define CANTP_NORMALFIXED                                     (AddressingFormatType)0x03
#define CANTP_STANDARD                                        (AddressingFormatType)0x04


#define ISO15765_TPCI_MASK                                                   (uint8)0x30
#define ISO15765_TPCI_SF                                                     (uint8)0x00  /* Single Frame */
#define ISO15765_TPCI_FF                                                     (uint8)0x10  /* First Frame */
#define ISO15765_TPCI_CF                                                     (uint8)0x20  /* Consecutive Frame */
#define ISO15765_TPCI_FC                                                     (uint8)0x30  /* Flow Control */
#define ISO15765_TPCI_DL                                                     (uint8)0x07  /* Single frame data length mask */

#define ISO15765_TPCI_DLFF                                                   (uint8)0x0f  /* First frame First 4 bits data length mask */

#define ISO15765_TPCI_FS_MASK                                                (uint8)0x0F  /* FlowControl status mask */
#define ISO15765_FLOW_CONTROL_STATUS_CTS                                     (uint8)0x00  /* FC Clear/Continue To Send Status */
#define ISO15765_FLOW_CONTROL_STATUS_WAIT                                    (uint8)0x01  /* FC Waiting Status */
#define ISO15765_FLOW_CONTROL_STATUS_OVFLW                                   (uint8)0x02  /* FC OverFlows Status */


/**************************** Function-Like Macros ******************************************/

#define TIMER_DECREMENT(timer) \
        if (timer > 0) { \
            timer = timer - 1; \
        } \

#define COUNT_DECREMENT(count) TIMER_DECREMENT(count)

/****************************** CanTpTxNSdu Structure ******************************************/
typedef struct
{
    const float64          CanTpNas;                     /* Value in second of the N_As timeout. N_As is the time for transmission of
                                                            a CAN frame (any N_PDU) on the part of the sender. */

    const float64          CanTpNbs;                     /* Value in seconds of the N_Bs timeout. N_Bs is the time of transmission
                                                            until reception of the next Flow Control N_PDU */

    const float64          CanTpNcs;                     /* Value in seconds of the performance requirement of (N_Cs + N_As).
                                                            N_Cs is the time in which CanTp is allowed to request the Tx data of a
                                                            Consecutive Frame N_PDU. */

    boolean                CanTpTc;                      /* Switch for enabling Transmit Cancellation. */

    uint8                  CanTpTxAddressingFormat ;     /* Declares which communication addressing format is supported for this TxNSdu.
                                                            Definition of Enumeration values:
                                                                  CanTpStandard & CanTpExtended &
                                                                  CanTpMixed & CanTpNormalFixed &CanTpMixed29Bit  */

    uint16                 CanTpTxNSduId;                /* Unique identifier to a structure that contains all useful information to
                                                            process the transmission of a TxNsdu. */

    const uint8            CanTpTxPaddingActivation;     /* Defines if the transmit frame use padding or not. This parameter is restricted
                                                                  to 8 byte N-PDUs. Definition of Enumeration values ( CANTP_OFF & CANTP_ON) */

    uint8                  CanTpTxTaType;                /* Declares the communication type of this TxNsdu.
                                                                  Enumeration values: (CANTP_FUNCTIONAL & CANTP_PHYSICAL) */

    uint16                 CanTpTxNPduConfirmationPduId; /* Handle Id to be used by the CanIf to confirm the transmission of the
                                                                  CanTpTxNPdu to the CanIf module */

    uint16                 CanTpRxFcNPduId;              /* N-PDU identifier attached to the FC N-PDU of this TxNsdu
                                                                  Each TxNsdu identifier is linked to one Rx FC N-PDU identifier only.
                                                                  However, in the case of extended addressing format, the same FC N-PDU
                                                                  identifier can be used for several N-SDU identifiers. The distinction is
                                                                  made by means of the N_TA value (first data byte of FC frames) */
}CanTpTxNSdu_s;

/**************** CanTpRxNSduType Structure ****************/
typedef struct
{
    const uint8            CanTpBs ;                     /* Sets the number of N-PDUs the CanTp receiver allows the sender to send, before
                                                           waiting for an authorization to continue transmission of the following  N-PDUs.*/

    const float64          CanTpSTmin;                   /* Sets the duration of the minimum time the CanTp sender shall wait
                                                            between the transmissions of two CF N-PDUs.*/

    const uint16           CanTpRxWftMax;                /* his parameter indicates how many Flow Control wait N-PDUs can be consecutively
                                                            transmitted by the receiver. It is local to the node and is not  transmitted
                                                            inside the FC protocol data unit ,it is used to avoid sender nodes being potentially
                                                            hookedup in case of a temporarily reception inability on the part of the receiver nodes,
                                                            whereby the sender could be waiting continuously. */

    const float64          CanTpNar;                     /* Value in seconds of the N_Ar timeout. N_Ar is the time for transmission of
                                                            a CAN frame (any N_PDU) on the receiver side */

    const float64          CanTpNbr;                     /* Value in seconds of the N_Cr timeout. N_Cr is the time until reception of
                                                            the next Consecutive Frame N_PDU */

    const float64          CanTpNcr;                     /* Value in seconds of the N_Cr timeout. N_Cr is the time until reception of
                                                                  the next Consecutive Frame N_PDU. */

    uint8                  CanTpRxAddressingFormat;           /* Declares which communication addressing mode is supported for this RxNSdu.
                                                                 Definition of Enumeration values:(CANTP_EXTENDED & CANTP_MIXED
                                                                               & CANTP_MIXED29BIT & CANTP_NORMALFIXED & CANTP_STANDARD) */



    uint16                 CanTpRxNSduId;                /* Unique identifier user by the upper layer to call CanTp_CancelReceive,
                                                                  CanTp_ChangeParameter and CanTp_ReadParameter. */

    uint16                 CanTpRxNPduId;                 /* The N-PDU identifier attached to the RxNsdu is identified by CanTpRxNSduId.
                                                                   Each RxNsdu identifier is linked to only one SF/FF/CF N-PDU identifier.
                                                                   Nevertheless, in the case of extended or mixed addressing format, the
                                                                   same N-PDU identifier can be used for several N-SDU identifiers. The
                                                                   distinction is made by the N_TA or N_AE value (first data byte of SF or FF frames).*/

    const uint8            CanTpRxPaddingActivation;     /* Defines if the receive frame uses padding or not. This parameter is restricted to
                                                                  8 byte N-PDUs. Definition of enumeration values ( CANTP_OFF & CANTP_ON) */

    uint8                  CanTpRxTaType;                /* Declares the communication type of this Rx N-SDU
                                                                  Definition of enumeration values ( CANTP_FUNCTIONAL & CANTP_PHYSICAL) */

    uint16                 CanTpTxFcNPduConfirmationPduId; /* Handle Id to be used by the CanIf to confirm the transmission of the
                                                                    CanTpTxFcNPdu to the CanIf module. */
}CanTpRxNSdu_s;

/************** Channel Info Structure ********************/
typedef struct
{
  
  /* Channel ID is the index of ChannelInfo Array created in CanTp.c , so no need to define a parameter for ID */
  
  ChannelModeType Mode;         /* Channel mode configuration options {CANTP_MODE_HALF_DUPLEX or CANTP_MODE_FULL_DUPLEX} */                    
  PduIdType       StIdx;        /* Start index of TxNSdu and RxNSdu in configuration arrays generated */
  uint8           Length;       /* Length of TxNSdu and RxNSdu sharing the same channel ID 
                                 * "should be ordered in configuration arrays generated" */

}ChannelInfo_s;


/* User-defined structure to hold information regarding CanIF */
typedef struct
{
    uint8                  *IFdataPtr;
    PduLengthType          IFByteCount;
}IfInfoType;

/**************** RunTimeInfo Structure ****************/
typedef struct
{
    uint16                        nextFlowControlCount;
    uint16                        framesHandledCount;
    uint32                        stateTimeoutCount;
    uint8                         STmin;
    uint8                         BS;
    TransferStateTypes            state;
    uint32                        transferTotal;
    uint32                        transferCount;
    uint32                        availableDataSize;
    PduInfoType                   pdurBuffer;       // The PDUR make an instance of this.
    IfInfoType                    IfBuffer;
    CanTp_TransferInstanceMode    mode;
    PduLengthType                 Buffersize;
    
}RunTimeInfo_s;


#endif /* CANTP_TYPES_H_ */
