/********************************************************************************************
 *                                                                                          *
 * File Name   : CanIf_Types.h                                                              *
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
#ifndef CANIF_TYPES_H_
#define CANIF_TYPES_H_

#include "Can_Cfg.h"
#include "Can_GeneralTypes.h"
#include "CanIf_Cfg.h"
#include "ComStack_Types.h"
#include "Platform_Types.h"
#include "stdint.h"
#include "stdbool.h"


/********************************************************************************************************/
/********************************************************************************************************/
/*************************************************      MACROS  *****************************************/
/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/

#define EXTENDED_CAN                              0
#define EXTENDED_FD_CAN                           1
#define STANDARD_CAN                              2
#define STANDARD_FD_CAN                           3


/** Channel is in the offline mode ==> no transmission or reception */
#define CANIF_GET_OFFLINE                         0
/** Receive path of the corresponding channel is enabled and
*transmit path is disabled */
#define CANIF_GET_RX_ONLINE                       1
/** Transmit path of the corresponding channel is enabled and
*receive path is disabled */
#define CANIF_GET_TX_ONLINE                       2
/** Channel is in the online mode ==> full operation mode */
#define CANIF_GET_ONLINE                          3
/** Transmit path of the corresponding channel is in
*the offline mode ==> transmit notifications are processed but
*transmit requests are blocked. The receiver path is disabled. */
#define CANIF_GET_OFFLINE_ACTIVE                  4
/** Transmit path of the corresponding channel is in the offline
*active mode ==> transmit notifications are processed but transmit
*requests are blocked. The receive path is enabled. */
#define CANIF_GET_OFFLINE_ACTIVE_RX_ONLINE        5





/********************************************************************************************************/
/********************************************************************************************************/
/************************************     Standard types in SWS     ************************************/
/********************************************************************************************************/
/********************************************************************************************************/

typedef  void(*Pointer2Func)(void);

typedef enum
{
    CANIF_OFFLINE,                  /*Transmit and receive path of the corresponding channel are disabled => no communication mode*/
    CANIF_TX_OFFLINE,               /* Transmit path of the corresponding channel is disabled. The receive path is enabled */
    CANIF_TX_OFFLINE_ACTIVE,        /* Transmit path of the corresponding channel is in off line active mode.
                                       The receive path is disabled. This mode requires CanIfTxOfflineActiveSupport = TRUE */
    CANIF_ONLINE                    /* Transmit and receive path of the corresponding channel are enabled => full operation mode */
}CanIf_PduModeType;



typedef enum {
    CAN_TP ,
    CDD    ,/*Not standard  call upear layer handler in the struct */
    CAN_NM ,
    J1939TP,
    PDUR   ,
    XCP    ,
    J1939NM,
    CAN_TSYN
}CanIfRxPduUserRxConfirmationUL;



typedef struct
{
    uint8 CanIfCtrlId;

    uint8 CanIfCtrlCanCtrlRef;

}CanIfCtrlCfg_s;

typedef struct 
{

    /** Reference to controller Id to which the HRH belongs to. A controller can
     *  contain one or more HRHs. */
    uint8 CanIfHrhCanCtrlIdRef;

    /* The parameter refers to a particular HRH object in the CanDrv
     * configuration */
    uint8 CanIfHrhIdSymRef;
    
} CanIfHrhCfg_s;



typedef struct{
    /* Reference to controller Id to which the HTH belongs to. A controller can contain one or more HTHs*/
    uint8 CanIfHthCanCtrlIdRef;

    /* The parameter refers to a particular HTH object in the CanDrvconfiguration */
    uint8 CanIfHthIdSymRef;

}CanIfHthCfg_s;




typedef struct 
{

    uint8 CanIfRxPduCanIdType;

  /** CAN Identifier of receive CAN L-PDUs used by the CAN Driver for CAN L-
   *  PDU transmission.
     *  EXTENDED_CAN  The CANID is of type Extended (29 bits)
   *  STANDARD_CAN  The CANID is of type Standard (11 bits) */
    uint32 CanIfRxPduCanId; //

  /** Data Length code of received CAN L-PDUs used by the CAN Interface.
   *  Exa: DLC check. The data area size of a CAN L-PDU can have a range
   *  from 0 to 8 bytes.*/
    uint16 CanIfRxPduDataLength;  //
Pointer2Func CanIfRxPduUserRxIndicationName;

    /*This parameter defines the upper layer (UL) module to which the
        indication of the successfully received CANRXPDUID has to be routed
        via <User_RxIndication>. This <User_RxIndication> has to be invoked
        when the indication of the configured CANRXPDUID will be received
        by an Rx indication event from the CAN Driver module. If no upper
        layer (UL) module is configured, no <User_RxIndication> has to be
        called in case of an Rx indication event of the CANRXPDUID from the
        CAN Driver module. */
    uint8 CanIfRxPduUserRxIndicationUL;

  /** The HRH to which Rx L-PDU belongs to, is referred through this
   *  parameter. */
    uint8  CanIfRxPduHrhIdRef;  //



}CanIfRxPduCfg_s;


typedef struct {

    /** CAN Identifier of transmit CAN L-PDUs used by the CAN Driver for CAN L-
     *  PDU transmission. Range: 11 Bit For Standard CAN Identifier ... 29 Bit For
     *  Extended CAN identifier */
    uint32 CanIfTxPduCanId;

/*TOBEASKED:what is the usage of  CanIfTxPduCanIdType ?*/

    uint8 CanIfTxPduCanIdType; 

    /*   This parameter contains the upper layer of each indivdual pdu   */
    uint8 CanIfTxPduUserTxConfirmationUL;


    uint8 CanIfHthCfgRef;

} CanIfTxPduCfg_s;

typedef struct
{

} CanIf_ConfigType_s;

/****************************   Types Not in SWS    *************************************/

typedef enum {
    CANIF_CHANNEL_1,
    CANIF_CHANNEL_2,
    MAX_NUM_CHANNELS
}CanIf_Channel_t;


typedef struct{
    Can_ControllerStateType Controller_Mode;
    CanIf_PduModeType PduMode;
}CanIf_ChannelPrivateType;

typedef struct {
    boolean initRun;
    CanIf_ChannelPrivateType channelData[MAX_CANIF_CTRL];
}CanIf_GlobalType;



#endif /* CANIF_TYPES_H_ */
