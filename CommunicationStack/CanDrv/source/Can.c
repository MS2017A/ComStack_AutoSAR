/********************************************************************************************
 *                                                                                          *
 * File Name   : Can.c                                                            *
 *                                                                                          *
 * Author      : ITI AUTOSAR Can_CanIf Team                                                 *
 *                                                                                          *
 * contacts:   : Mahmoud Gamal (mahmouddgamall93@gmail.com)                                 *
 *                                                                                          *
 *               Mohammed Ibrahim (mohammedibra96@gmail.com)                                *
 *                                                                                          *
 * Date        : 26 Jun 2020                                                                *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Version     : 4.1.1                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR  Basic Software module CAN Transport Layer           *
 *               source file Release 4.3.1                                                  *
 *                                                                                          *
 ********************************************************************************************/

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************includes*********************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/

#include "Can.h"
#include "Can_Cfg.h"
#include "Can_interrupt.h"
#include "Can_Lcfg.h"
#include "driverlib/can.h"
#include "driverlib/interrupt.h"

#if DEBUG == TRUE
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#endif

#include "inc/tm4c123gh6pm.h"
#include "MCU.h"
#include "Det.h"
#include "Can_Det.h"



/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*************************************************** MACROS *********************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/

#define CAN_CONTROLLER_ZERO_BASE_ADDRESS 0x40040000U
#define CAN_CONTROLLER_ONE_BASE_ADDRESS  0x40041000U

#define BOFF                          CAN_STATUS_BUS_OFF
#define CAN_ERRORSTATE_ACTIVE_TX_OK   CAN_STATUS_TXOK
#define CAN_ERRORSTATE_ACTIVE_RX_OK   CAN_STATUS_RXOK
#define EPASS                         CAN_STATUS_EPASS

#define IS_VALID_ERROR(Error) ((Error) == BOFF || (Error) == CAN_ERRORSTATE_ACTIVE_TX_OK || (Error) == CAN_ERRORSTATE_ACTIVE_RX_OK ||(Error) == EPASS )

#define  FIRST_HO                0U
/*CAN_CODE this macro for Can code memory section  */
#define  CAN_CODE                1

#define  MIN_DATA_SIZE           1U
#define  MAX_DATA_LENGTH         8U
#define IS_VALID_DATA_LENGTH(DataLength) ( ((DataLength)<= MAX_DATA_LENGTH) )

#define  FIRST_MAIL_BOX          0U
#define  LAST_MAIL_BOX           32U
#define IS_VALID_MB(MailBoxIndex) ( (MailBoxIndex) >=FIRST_MAIL_BOX && (MailBoxIndex) < LAST_MAIL_BOX  )


#define IS_NEW_DATA_RECEIVED(Flag) ( (( (Flag) & MSG_OBJ_NEW_DATA) == MSG_OBJ_NEW_DATA) )


#define MESSAGE_WAITING_MASK  0x100
#define IS_CONFIRMED(Register) ((Register) & MESSAGE_WAITING_MASK)   != MESSAGE_WAITING_MASK)

#define WRNRD_MASK            0x80
/*Clear bit to go to the next operation */
#define CLR_WRNRD_BIT(Register) ( (Register)&= ((uint32)(~WRNRD_MASK)) )


#define  IS_CONTROLLER_MODE_STARTED(Mode1,Mode2) (((Mode1)!= CAN_CS_STARTED ) && ( (Mode2)!= CAN_CS_STARTED))

#define  CAN_CTL_R_INIT_BIT      1U
#define  MAX_NUM_OF_MAILBOXES    32U

#define  INTERRUPT_ENABLE        1U
#define  INTERRUPT_DISABLE       0U
#define IS_INTERRUPT_ENABLED(Status) ((Status) == INTERRUPT_ENABLE )


#define MODE_CHANGED             1U
#define MODE_NOT_CHANGED         0U
#define IS_MODE_CHANGED(Mode) ( (Mode) == MODE_CHANGED )


#define NORMAL_MODE 0U
#define INITIALIZE_STARTED_MODE 1U
#define IS_NORMAL_MODE(Mode ,Register ) (( (Register) & CAN_CTL_R_INIT_BIT ) == NORMAL_MODE &&  (Mode) == CAN_CS_STARTED )
#define IS_INIT_STARTED_MODE(Mode ,Register ) (( (Register) & CAN_CTL_R_INIT_BIT ) == INITIALIZE_STARTED_MODE &&  (Mode) == CAN_CS_STOPPED || (Mode) == CAN_CS_SLEEP )



#define IS_VALID_TRANSTION(Transition) ( ( ( (Transition) == ( Can_StateTransitionType ) CAN_CS_STARTED) && ( CanControllerCurrentMode [Controller] == ( Can_StateTransitionType ) CAN_CS_STOPPED))|| (( (Transition) == ( Can_StateTransitionType )CAN_CS_SLEEP) && (CanControllerCurrentMode [Controller] == ( Can_StateTransitionType )CAN_CS_STOPPED )  ) )

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*************************************************** Global Variables ***********************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*  Can_MailBoxLookUpTables shall be filled in Can_Init to map the mailboxes to corresponding hardware object and pdu id for the CanIf_TxXonfirmation()*/
LOCAL Can_MailBoxLookUpTables_s Can_MailBoxLookUpTables[MAX_NUM_OF_CAN_CONTROLLERS][MAX_NUM_OF_MAILBOXES]  ;
/*CanControllerModeStatus this indicated that Can controller mode has changed since last checked  */
LOCAL VAR(uint8,AUTOMATIC) CanControllerModeStatus[2]={ MODE_NOT_CHANGED,MODE_NOT_CHANGED }                ;
/*CanControllerCurrentMode contains the current mode of the can controller */
LOCAL Can_ControllerStateType CanControllerCurrentMode [MAX_NUM_OF_CAN_CONTROLLERS]                        ;
/*CanControllerInterruptStatus contains the current interrupt status                            */
LOCAL uint8 CanControllerInterruptStatus [MAX_NUM_OF_CAN_CONTROLLERS]                                    ;
/*CanControllerInterruptDisable this count how many times the CanControllerInterruptDisable API has been called before sequentially */
LOCAL VAR(uint8,AUTOMATIC) CanControllerInterruptDisable[MAX_NUM_OF_CAN_CONTROLLERS]                     ;
/*[SWS_Can_00103] After power-up/reset, the Can module shall be in the state*/
/* CanDriverStatus indicates the current driver Status */
LOCAL VAR(Can_StateType,AUTOMATIC) CanDriverStatus = CAN_UNINIT                                            ;

/* CurrentMailBox indicates the max configured Mailbox */
LOCAL VAR(uint8,AUTOMATIC) CurrentMailBox[MAX_NUM_OF_CAN_CONTROLLERS] = {0,0}    ;

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************static functions Prototypes**************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/

/* this function shall initialize the can hardware object which assign the hardware object to mailboxes and to fill the look up table*/
LOCAL FUNC(void,CAN_CODE) Can_ConfigureHardwareObject(void);







/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_Init*********************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/




FUNC(void,CAN_CODE) Can_Init(P2VAR(Can_ConfigType,CAN_CODE,AUTOMATIC)  Config)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           {
    VAR(uint8,AUTOMATIC)              CanDevolpmentErrorType    = E_OK      ;
    VAR(uint16,AUTOMATIC)             BaudrateId                = 0      ;
    VAR(uint32,AUTOMATIC)             CanControllerBaseAddress  = 0      ;
    VAR(uint8,AUTOMATIC)              CanControllerIndex        = CAN_CONTROLLER_ZERO      ;
    /* structure containing bit time parameters                                                      */
    VAR(tCANBitClkParms,AUTOMATIC)      Bit_Time_Parameters0             ;


    /*[SWS_Can_00246]  The function Can_Init shall change the module state to CAN_READY, after initialising all controllers inside the HW Unit.*/
    if(CanDriverStatus == CAN_UNINIT)
    {
        /*[SWS_Can_00245]  The function Can_Init shall initialize all CAN controllers according to their configuration                                             */
        for (CanControllerIndex = CAN_CONTROLLER_ZERO ; CanControllerIndex < MAX_NUM_OF_CAN_CONTROLLERS ; CanControllerIndex++ )
        {
            BaudrateId               =   CanController[CanControllerIndex].CanControllerDefaultBaudrate    ;
            CanControllerBaseAddress =   CanController[CanControllerIndex].CanControllerBaseAddress;
            /* Initialize Can controller Registers     (TivaWare)                                                                                                      */
            CANInit( CanControllerBaseAddress )                                                              ;
            /* store the value of Baudrate refrence to simplify the access in the below lines                                           */
            /*Fill Bit_Time_Parameters0 with time parameter from the conifguration parameters                                                                           */
            Bit_Time_Parameters0.ui32SyncPropPhase1Seg= ( (uint32)(CanControllerBaudrateConfig[ BaudrateId ].CanControllerSeg1)   ) +
                    ( (uint32)(CanControllerBaudrateConfig[ BaudrateId ].CanControllerPropSeg)) ;

            Bit_Time_Parameters0.ui32Phase2Seg        =   (uint32)CanControllerBaudrateConfig[ BaudrateId ].CanControllerSeg2                         ;
            Bit_Time_Parameters0.ui32SJW              =   (uint32)CanControllerBaudrateConfig[ BaudrateId ].CanControllerSyncJumpWidth                ;
            Bit_Time_Parameters0.ui32QuantumPrescaler =   Mcu_GetSysCloock()/((Bit_Time_Parameters0.ui32SyncPropPhase1Seg +
                    Bit_Time_Parameters0.ui32Phase2Seg+1U) * CanControllerBaudrateConfig[ BaudrateId ].CanControllerBaudRate*1000U);
            /* Configures the CAN controller bit timing      (TivaWare)                                                                                  */
            CANBitTimingSet(CanControllerBaseAddress,&Bit_Time_Parameters0)        ;
            /* One-time writable registers that require initialisation directly after reset shall be initialised by the startup code                                       */
            /*This function registers the interrupt handler in the interrupt vector table, and enables CAN interrupts on the interrupt
                controller; specific CAN interrupt sources must be enabled using CANIntEnable().                                                              */
            /*TODO:we will remove with integration with os team
             * We need to check if the mode POLLING or INTERRUPT */
            if ( CanController[CanControllerIndex].CanControllerBaseAddress == CAN_CONTROLLER_ZERO_BASE_ADDRESS )
            {
                CANIntRegister(CanControllerBaseAddress,(&Can0_InterruptHandler))            ;
                /*            initialize the CanInterruptId parameter in the Can_controller struct that shall interrupt masks that uses in Can_EnableControllerInterrupts       */
                CanController[CanControllerIndex].CanInterruptId = (uint32)INT_CAN0    ;
            }
            else if (CanController[CanControllerIndex].CanControllerBaseAddress == CAN_CONTROLLER_ONE_BASE_ADDRESS)
            {
                CANIntRegister(CanControllerBaseAddress,(&Can1_InterruptHandler))            ;
                /*            initialize the CanInterruptId parameter in the Can_controller struct that shall interrupt masks that uses in Can_EnableControllerInterrupts       */
                CanController[CanControllerIndex ].CanInterruptId = (uint32)INT_CAN1    ;
            }
            else
            {

            }
            /* only function can_Init can change controller state from uninit to stopped                                                                                     */
            /*[SWS_Can_00259]  The function Can_Init shall set all CAN controllers in the state STOPPED.  */
            /*responsible for incrementing disable_interrupt so that can enable_interrupt can execute*/
            /* after you have configured some all the can controllers in your module set this general flag                                                                   */
            CanControllerCurrentMode [CanControllerIndex]= CAN_CS_STOPPED                          ;


        }/* End Of the for loop */
        /* [SWS_Can_00250] The function Can_Init shall initialize:  LOCAL variables, including flags,Common setting for the complete CAN HW unitCAN                    */
        /* controller specific settings for each CAN controller                                                                                                         */
        /* [SWS_Can_00246] The function Can_Init shall change the module state to CAN_READY, after initializing  all controllers inside the HW Unit.                     */
        /* Configure hardware Objects                                                                                                                           */
        CanDriverStatus = CAN_READY                                                                                                           ;
        Can_ConfigureHardwareObject()                                   ;
    }
    else
    {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        /* [SWS_Can_00174] If development error detection for the Can module is enabled The function Can_Init shall raise                                                */
        /* the error CAN_E_TRANSITION if the driver is not in state CAN_UNINIT.                                                                                          */

        CanDevolpmentErrorType = CAN_E_TRANSITION;
        Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_INIT_SID,CAN_E_TRANSITION);
#endif
    }

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           }
/* Can_ConfigureHardwareObject isn't included in SWS
 * It shall be called from Can_Init
 * to initialize the hardware object                 */



LOCAL FUNC(void,CAN_CODE) Can_ConfigureHardwareObject(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
    VAR(uint8,AUTOMATIC) CanControllerId = 0                                       ;
    VAR(uint8,AUTOMATIC) canHardwareObjectIndex = FIRST_HO                                ;
    VAR(tCANMsgObject,AUTOMATIC) CANMessage                                      ;
    VAR(uint8,AUTOMATIC) HO_Index     = FIRST_HO                                         ;
    VAR(uint8,AUTOMATIC) HO_Ref[MAX_NUM_OF_CAN_CONTROLLERS] = {0,0}            ;
    VAR(uint8,AUTOMATIC) NumberMailBoxes_PerHO                                 ;
    VAR(uint8,AUTOMATIC) MailBoxIndex = 0                                      ;
    VAR(uint8,AUTOMATIC) FilterIndex = 0 ;
    VAR(uint8,AUTOMATIC) CanFilterMask = 0 ;


    /* Loop for all max hardware object in the System */
    for ( HO_Index =  FIRST_HO  ; HO_Index < (uint8)MAX_NUM_OF_HO ; HO_Index++ )
    {
        /* CanControllerId reference  to CanController 0 or 1 */
        CanControllerId             = CanHardwareObject[HO_Index].CanControllerRef  ;
        /*NumberMailBoxes_PerHO indicates the number of mail boxes per hardware object */
        NumberMailBoxes_PerHO       = CanHardwareObject[HO_Index].CanHwObjectCount  ;
        /* CanHardwareObject[HO_Index].CanMailBoxStart indicates the first MailBox related to the corresponding Hardware object */
        CanHardwareObject[HO_Index].CanMailBoxStart = CurrentMailBox[CanControllerId] ;
        for( canHardwareObjectIndex = FIRST_HO ; canHardwareObjectIndex < NumberMailBoxes_PerHO ; canHardwareObjectIndex++ )
        {
            /*HO_Ref indicates the corresponding hardware object index*/
            if ( IS_VALID_MB( CurrentMailBox[CanControllerId] ) == FALSE )
            {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON

                /* This error Not in SWS */
                Det_ReportError(CAN_MODULE_ID,CanControllerId,CAN_INIT_SID,CAN_E_NOT_VALID_MAILBOX);
#endif
            }
            /*Map each mail box to corresponding Hardware object */
            Can_MailBoxLookUpTables[CanControllerId][MailBoxIndex++].HwObject = HO_Ref[CanControllerId];
            if ( CanHardwareObject[HO_Index].CanObjectType == (CanObjectType_t)RECEIVE )
            {
                /*psMsgObject->ui32MsgIDMask bits that are used for masking during comparison.                   (TivaWare) */
                /*Set psMsgObject->ui32MsgID to the full message ID, or a partial mask to use partial ID matching.(TivaWare)*/
                CANMessage.ui32MsgID     =  CanHardwareObject[HO_Index].CanHwFilterCode ;
                /*Get the Hardware filter related to this mail box  */
                for (FilterIndex = CanHardwareObject[HO_Index].FilterStartRef ;FilterIndex <CanHardwareObject[HO_Index].FilterStartRef +CanHardwareObject[HO_Index].NumberOfFilter ;FilterIndex++)
                {
                    CanFilterMask |= CanHwFilterCfg[FilterIndex];
                }

                CANMessage.ui32MsgIDMask = CanFilterMask ;
                /*Set psMsgObject->ui32MsgLen to the number of bytes in the expected data frame.         (TivaWare)          */
                if ( CanHardwareObject[HO_Index].CanIdType ==  EXTENDED )
                {
                    /* \e psMsgObject->ui32Flags as follows: Set  MSG_OBJ_RX_INT_ENABLE flag to be interrupted when the data frame is received.
                                                             Set  MSG_OBJ_USE_ID_FILTER flag to enable identifier-based filtering.(TivaWare)*/
                    CANMessage.ui32Flags = (uint32)MSG_OBJ_RX_INT_ENABLE| (uint32)MSG_OBJ_USE_ID_FILTER|(uint32)MSG_OBJ_EXTENDED_ID                     ;
                }
                else
                {
                    CANMessage.ui32Flags = (uint32)MSG_OBJ_RX_INT_ENABLE|(uint32)MSG_OBJ_USE_ID_FILTER                                          ;
                }
                /*Configures a message object in the CAN controller.(TivaWare)   Number from 1 to 32                                                       */
                CANMessageSet( CanController[CanControllerId].CanControllerBaseAddress, MailBoxIndex ,&CANMessage, MSG_OBJ_TYPE_RX )         ;
            }
            else
            {
                /* MISRA */
            }
            if (CanHardwareObject[HO_Index].CanHandleType  == FULL )
            {
                break;
            }
            else
            {

            }
            CurrentMailBox[CanControllerId]++ ;
        }
        HO_Ref[CanControllerId]++;

    }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_DeInit*******************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/


FUNC(void,CAN_CODE) Can_DeInit(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
    VAR(uint8,AUTOMATIC)         CanDevolpmentErrorType  = E_OK                       ;
    VAR(uint8,AUTOMATIC)         Controller              = CAN_CONTROLLER_ZERO        ;

    if(CanDriverStatus == CAN_READY ) /*[SWS_Can_91011]  development error detection for the Can module is enabled:
                                     The function Can_DeInit shall raise the error CAN_E_TRANSITION if the
                                       driver is not in state CAN_READY.(SRS_BSW_00369) */
    {
        /*[SWS_Can_91012] If development error detection for the Can module is enabled: The function Can_DeInit shall
        raise the error CAN_E_TRANSITION if any of the CAN controllers is in state STARTED.*/
        /*  Caveat: Caller of the Can_DeInit function has to be sure no CAN controller is in the state STARTED*/

        if  ( IS_CONTROLLER_MODE_STARTED(CanControllerCurrentMode [CAN_CONTROLLER_ZERO],CanControllerCurrentMode[CAN_CONTROLLER_ONE]) == TRUE  )
        {
            CanDriverStatus = CAN_UNINIT ;  /*   [SWS_Can_ 91009] The function Can_DeInit shall change the module state to
                               CAN_UNINIT before de-initialising all controllers inside the HW unit.*/
            for (Controller = CAN_CONTROLLER_ZERO ; Controller < MAX_NUM_OF_CAN_CONTROLLERS ; Controller++)
            {
                CANDisable( CanController[Controller].CanControllerBaseAddress );
                CanControllerCurrentMode [Controller] = CAN_CS_UNINIT                 ;
            }
        }
        else{
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
            CanDevolpmentErrorType = CAN_E_TRANSITION;
            Det_ReportError(CAN_MODULE_ID,Controller,CAN_DEINIT_SID,CAN_E_TRANSITION);
#endif
        }
    }
    else
    {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_TRANSITION;
        Det_ReportError(CAN_MODULE_ID,Controller,CAN_DEINIT_SID,CAN_E_TRANSITION);
#endif
    }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }





/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_SetBaudrate**************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/


FUNC(Std_ReturnType,CAN_CODE) Can_SetBaudrate(VAR(uint8,AUTOMATIC) Controller,VAR(uint16,AUTOMATIC) BaudRateConfigID )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
#if CAN_SET_BAUDRATE_API == TRUE

    VAR( uint8 , AUTOMATIC )         CanDevolpmentErrorType = E_OK     ;
    VAR( uint8 , AUTOMATIC )         ErrorStatus            = E_OK     ;
    VAR( tCANBitClkParms , AUTOMATIC) Bit_Time_Parameters                ;

    if( CanDriverStatus != CAN_READY)
    {
        /*  [SWS_CAN_00492] If development error detection for the Can module is enabled:
       Specification of CAN Driver
        The function Can_SetBaudrate shall raise the error CAN_E_UNINIT and return
       E_NOT_OK if the driver is not yet init_ialized.*/
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_UNINIT;
        Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_SET_BAUDRATE_SID,CAN_E_UNINIT);
#endif
        ErrorStatus = E_NOT_OK;
    }
    else
    {
        /* MISRA */
    }
    if( Controller >=  MAX_NUM_OF_CAN_CONTROLLERS )
    {
        /*[SWS_CAN_00494] If development error detection for the Can module is enabled
                 the function Can_SetBaudrate shall raise the error CAN_E_PARAM_CONTROLLER and
                 return E_NOT_OK if the parameter Controller is out of range.*/
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_PARAM_CONTROLLER;
        Det_ReportError(CAN_MODULE_ID,Controller,CAN_SET_BAUDRATE_SID,CAN_E_PARAM_CONTROLLER);
#endif
        ErrorStatus = E_NOT_OK;
    }
    else
    {
        /* MISRA */
    }
    if(BaudRateConfigID >= MAX_NUM_OF_BAUDRATES )
    {
        /* [SWS_CAN_00493] If development error detection for the Can module is enabled:
                        The function Can_SetBaudrate shall raise the error CAN_E_PARAM_BAUDRATE
                               and return E_NOT_OK if the parameter BaudRateConfigID has an invalid value.*/
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType=CAN_E_PARAM_BAUDRATE;
        Det_ReportError(CAN_MODULE_ID,Controller,CAN_SET_BAUDRATE_SID,CAN_E_PARAM_BAUDRATE);
#endif
        ErrorStatus = E_NOT_OK;
    }
    else
    {
        /* MISRA */
    }
    if ( ErrorStatus == E_OK)

    {
        Bit_Time_Parameters.ui32SyncPropPhase1Seg = CanControllerBaudrateConfig[BaudRateConfigID ].CanControllerSeg1+
                CanControllerBaudrateConfig[BaudRateConfigID ].
                CanControllerPropSeg;
        Bit_Time_Parameters.ui32Phase2Seg=CanControllerBaudrateConfig[BaudRateConfigID ].CanControllerSeg2;
        Bit_Time_Parameters.ui32SJW=CanControllerBaudrateConfig[BaudRateConfigID ].CanControllerSyncJumpWidth;
        Bit_Time_Parameters.ui32QuantumPrescaler= Mcu_GetSysCloock()/((Bit_Time_Parameters.ui32SyncPropPhase1Seg
                +Bit_Time_Parameters.ui32Phase2Seg+1U)*
                CanControllerBaudrateConfig[BaudRateConfigID ].CanControllerBaudRate*1000U);


        CANBitTimingSet(CanController[Controller].CanControllerBaseAddress,&Bit_Time_Parameters);
        if( CanControllerCurrentMode [Controller]== (uint8)CAN_CS_STOPPED||CanControllerCurrentMode [Controller]== (uint8)CAN_CS_SLEEP)
        {
            CANDisable(CanController[Controller].CanControllerBaseAddress);
        }else
        {
            /*MISRA*/
        }
        ErrorStatus = E_OK;
    }
    else
    {
        ErrorStatus= E_NOT_OK;
    }
    return ErrorStatus ;
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_SetControllerMode********************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/

/* SWS_Can_00230: This function performs software triggered state transitions of the CAN controller State machine
 *                 Which means that this function changes the state of the controller between started, stopped and sleep
 */


FUNC(Std_ReturnType,CAN_CODE) Can_SetControllerMode( VAR(uint8,AUTOMATIC) Controller, VAR(Can_StateTransitionType,AUTOMATIC) Transition )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
    VAR(uint8,AUTOMATIC)        HwObject     = FIRST_HO           ;
    VAR(Std_ReturnType,AUTOMATIC) toBeReturned = E_OK               ;
    VAR(uint8,AUTOMATIC)        CanDevolpmentErrorType = E_OK     ;
    /* Take a copy of Controller */

    /*  Check for the initialization of the Driver   */
    if(CanDriverStatus != CAN_UNINIT)
    {
        if(Controller < MAX_NUM_OF_CAN_CONTROLLERS)
        {
            /*   [SWS_Can_00409]  When the function Can_SetControllerMode (CAN_CS_STARTED) is entered and the
                CAN controller is not in state STOPPED it shall detect a invalid state transition*/
            if(IS_VALID_TRANSTION(Transition))
            {
                /*  this flag is set here to indicate to Can_Mainfunction_Mode that a change has been made
                 *  it is also cleared at the Can_Mainfunction_Mode
                 */
                CanControllerModeStatus[Controller] = MODE_CHANGED       ;

                switch(Transition)
                {

                case CAN_CS_STARTED :
                    /*[SWS_Can_00196]The function Can_SetControllerMode shall enable interrupts that are needed in the new state.*/

                    /* [SWS_Can_00261] The function Can_SetControllerMode(CAN_CS_STARTED) shall set the hardware registers in a way
                              that makes the CAN controller participating on the network.*/

                    /*[SWS_Can_00204] The Can module shall track all individual enabling and disabling of interrupts in other functions (i.e. Can_SetControllerMode)
                            , so that the correct interrupt enable state can be restored.*/

                    /* [SWS_Can_00425] Enabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled by function Can_DisableControllerInterrupts.*/
                    if( OUT_CAN_CONTROLLER_INTERRUPT_DISABLE()  && CanControllerInterruptStatus[Controller] != INTERRUPT_ENABLE )
                    {
#if CAN_CONTROLLER_CAN_TX_PROCESSING ==  INTERRUPT  || CAN_CONTROLLER_CAN_RX_PROCESSING ==  INTERRUPT
                        CANIntEnable(CanController[Controller].CanControllerBaseAddress, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
                        IntEnable(CanController[Controller].CanInterruptId)                                                              ;
                        CanControllerInterruptStatus[Controller] = INTERRUPT_ENABLE                                                                   ;
#endif
                        CANEnable(CanController[Controller].CanControllerBaseAddress)                                                    ;
                    }
                    /* if the interrupt has been enabled before hand */
                    else
                    {
                        CANEnable(CanController[Controller].CanControllerBaseAddress);
                    }

                    /*  Changing the global controller mode  */
                    CanControllerCurrentMode[Controller] = CAN_CS_STARTED;
                    break;

                case CAN_CS_STOPPED :

                    /* if the interrupt has been disabled before hand */
                    if( IN_CAN_CONTROLLER_INTERRUPT_DISABLE() )
                    {
                        CANDisable(CanController[Controller].CanControllerBaseAddress);
                    }
                    /*  if this is the first time to disabled the interrupt  */
                    else
                    {
                        CanControllerInterruptStatus[Controller] = INTERRUPT_DISABLE                                                                   ;
                        CANDisable (CanController[Controller].CanControllerBaseAddress)                                                   ;
                        CANIntDisable(CanController[Controller].CanControllerBaseAddress, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
                    }

                    /* [SWS_Can_00263] The function Can_SetControllerMode(CAN_CS_STOPPED) shall set theThis message object is not waiting for transmission.
                            bits inside the CAN hardware such that the CAN controller stops participating on the network.*/
                    for( HwObject = FIRST_HO ; HwObject < MAX_NUM_OF_HO ; HwObject++ )
                    {
                        if( CanHardwareObject[HwObject].CanObjectType == (CanObjectType_t)TRANSMIT)
                        {
                            if(Controller == CAN_CONTROLLER_ZERO)   /*  if this is Can0 then use the registers from Can0 (Note That: if you are accessing the registers at the whole code, please use struct and remove this if)  */
                            {
                                /* Selects one of the 32 message objects in the message RAM for data transfer. The message objects are numbered from 1 to 32. */
                                /* This message object is not waiting for transmission.*/
                                CAN0_IF1CRQ_R   = ((uint32) ( Can_MailBoxLookUpTables[Controller][HwObject].HwObject ) );

                                CAN0_IF1MCTL_R &= ( ( uint32 )( ~MESSAGE_WAITING_MASK ) )                                      ;
                                CAN0_IF1CMSK_R &= ( ( uint32 )( ~WRNRD_MASK ) )                                              ;
                            }
                            else if( Controller == CAN_CONTROLLER_ONE )
                            {
                                CAN1_IF1CRQ_R   = ( (uint32 )( Can_MailBoxLookUpTables[Controller][(HwObject)].HwObject ) );
                                CAN1_IF1MCTL_R &= ( (uint32 )( ~MESSAGE_WAITING_MASK))                                          ;
                                CAN1_IF1CMSK_R &= ( (uint32 )( ~WRNRD_MASK ) )                                                ;
                            }
                        }
                        else
                        {/*MISRA*/}
                    }
                    CanControllerCurrentMode[Controller] = CAN_CS_STOPPED ;
                    break;



                    /*[SWS_Can_00258] When the CAN hardware does not support sleep mode and is triggered to transition into SLEEP state,
                            the Can module shall emulate a logical SLEEP state from which it returns only, when it is triggered by software to transition into STOPPED state.*/
                    /*[SWS_Can_00404] The CAN hardware shall remain in state STOPPED, while the logical SLEEP state is active.*/
                    /*[SWS_Can_00290] If the CAN HW does not support a sleep mode,
                            the function Can_SetControllerMode(CAN_CS_SLEEP) shall set the CAN controller to the logical sleep mode.*/
                    /* [SWS_Can_00197] The function Can_SetControllerMode shall disable interrupts that are not allowed in the new state. */
                case CAN_CS_SLEEP :
                    /* if the interrupt has been disabled before hand */
                    if(IN_CAN_CONTROLLER_INTERRUPT_DISABLE())
                    {

                        CANDisable(CanController[Controller].CanControllerBaseAddress);

                    }
                    /*  if this is the first time to disabled the interrupt  */
                    else
                    {
                        CanControllerInterruptStatus[Controller] = INTERRUPT_DISABLE                                                                     ;
                        CANDisable ( CanController[Controller].CanControllerBaseAddress )                                                   ;
                        CANIntDisable( CanController[Controller].CanControllerBaseAddress, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS );
                    }

                    /* [SWS_Can_00263] The function Can_SetControllerMode(CAN_CS_STOPPED) shall set theThis message object is not waiting for transmission.
                            bits inside the CAN hardware such that the CAN controller stops participating on the network.*/

                    for( HwObject = FIRST_HO ; HwObject < MAX_NUM_OF_HO ; HwObject++)
                    {
                        if( CanHardwareObject[HwObject].CanObjectType==TRANSMIT)
                        {
                            if( Controller == CAN_CONTROLLER_ZERO )   /*  if this is Can0 then use the registers from Can0 (Note That: if you are accessing the registers at the whole code, please use struct and remove this if)  */
                            {
                                /* Selects one of the 32 message objects in the message RAM for data transfer. The message objects are numbered from 1 to 32. */
                                CAN0_IF1CRQ_R  = ((uint32)(Can_MailBoxLookUpTables[Controller][HwObject].HwObject));
                                /* This message object is not waiting for transmission.*/
                                CAN0_IF1MCTL_R &= ((uint32)(~MESSAGE_WAITING_MASK));
                                CAN0_IF1CMSK_R &= ((uint32)(~WRNRD_MASK));
                            }
                            else if( Controller == CAN_CONTROLLER_ONE )
                            {
                                CAN1_IF1CRQ_R   = ( (uint32)( Can_MailBoxLookUpTables[Controller][(HwObject)].HwObject ) ) ;
                                /* This message object is not waiting for transmission.*/
                                CAN1_IF1MCTL_R &= ( (uint32)( ~MESSAGE_WAITING_MASK))                                      ;
                                CAN1_IF1CMSK_R &= ( (uint32)( ~WRNRD_MASK))                                              ;
                            }else
                            {}
                        }
                        else
                        {/*MISRA*/}
                    }
                    CanControllerCurrentMode[Controller] = CAN_CS_SLEEP;
                    break;
                default:
                    break;
                }
            }

            /*   if the input parameter Transition is not valid, then change the error type and return E_NOT_OK indication that the operation did not take place   */
            else
            {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
                CanDevolpmentErrorType=CAN_E_TRANSITION;
                Det_ReportError(CAN_MODULE_ID,Controller,CAN_SET_CONTROLLER_MODE_SID,CanDevolpmentErrorType);
#endif

                toBeReturned = E_NOT_OK;

            }
        }
        /*   if the input parameter Controller is not valid, then change the error type and return E_NOT_OK indication that the operation did not take place   */
        else
        {
            toBeReturned = E_NOT_OK                          ;
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
            CanDevolpmentErrorType = CAN_E_PARAM_CONTROLLER  ;
            Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_SET_CONTROLLER_MODE_SID,CanDevolpmentErrorType);
#endif

        }
    }


    else
    {

#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_PARAM_CONTROLLER  ;
        Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_INIT_SID,CanDevolpmentErrorType);
#endif
        toBeReturned = E_NOT_OK;
    }

    /**********************************************************************************************************************/
    /*[SWS_Can_00262] The function Can_SetControllerMode(CAN_CS_STARTED) shall wait for
 limited time until the CAN controller is fully operational. Compare to SWS_Can_00398.*/

    /*[SWS_Can_00264] The function Can_SetControllerMode(CAN_CS_STOPPED) shall wait for
a limited time until the CAN controller is really switched off. Compare to SWS_Can_00398.*/

    return toBeReturned;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_DisableControllerInterrupts**********************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/




FUNC(void,CAN_CODE)  Can_DisableControllerInterrupts(VAR(uint8,AUTOMATIC) Controller)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
#if CAN_CONTROLLER_CAN_TX_PROCESSING ==  INTERRUPT  || CAN_CONTROLLER_CAN_RX_PROCESSING ==  INTERRUPT
    VAR(uint8, AUTOMATIC) CanDevolpmentErrorType ;
    if(CanDriverStatus!=CAN_UNINIT)
    {
        if(Controller < MAX_NUM_OF_CAN_CONTROLLERS)
        {
            /*[SWS_Can_00202] When Can_DisableControllerInterrupts has been called several times,
         Can_EnableControllerInterrupts must be called as many times before the interrupts are re-enabled.*/
            /*  in other words this counter increases everytime you call the disable function and to re-enable you need to
                call this function with the same amount.  */

            if( OUT_CAN_CONTROLLER_INTERRUPT_DISABLE() == TRUE)
            {
                CANIntDisable(CanController[Controller].CanControllerBaseAddress, (uint32)CAN_INT_MASTER | (uint32)CAN_INT_ERROR | (uint32)CAN_INT_STATUS);
                IntDisable(CanController[Controller].CanInterruptId);
                CanControllerInterruptStatus[Controller] = INTERRUPT_DISABLE ;
            }
            else
            {

            }
            ENTER_CAN_CONTROLLER_INTERRUPT_DISABLE(Controller);
        }
        else
        {

#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
            CanDevolpmentErrorType = CAN_E_PARAM_CONTROLLER  ;
            Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_DISABLE_CONTROLLER_INTERRUPTS_SID,CanDevolpmentErrorType);
#endif
        }
    }
    else
    {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_UNINIT;
        Det_ReportError(CAN_MODULE_ID,Controller,CAN_DISABLE_CONTROLLER_INTERRUPTS_SID,CanDevolpmentErrorType);
#endif
    }

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_EnableControllerInterrupts**********************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/




FUNC(void,CAN_CODE)  Can_EnableControllerInterrupts(VAR(uint8,AUTOMATIC) Controller)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
#if CAN_CONTROLLER_CAN_TX_PROCESSING ==  INTERRUPT  || CAN_CONTROLLER_CAN_RX_PROCESSING ==  INTERRUPT
    VAR(uint8,AUTOMATIC) CanDevolpmentErrorType;
    if(CanDriverStatus!=CAN_UNINIT)
    {
        if(Controller < MAX_NUM_OF_CAN_CONTROLLERS)
        {

            if( CanControllerCurrentMode[Controller] == CAN_CS_SLEEP || CanControllerCurrentMode[Controller] == CAN_CS_STOPPED)
            {

                /* [SWS_Can_00208] The function Can_EnableControllerInterrupts shall perform no action when
                      Can_DisableControllerInterrupts has not been called before.*/
                if(IN_CAN_CONTROLLER_INTERRUPT_DISABLE() == TRUE )

                {
                    /*[SWS_Can_00202]   When Can_DisableControllerInterrupts has been called several times,
                                    Can_EnableControllerInterrupts must be called as many times before the interrupts are re-enabled.*/
                    /*  in other words this counter increases every time you call the disable function and to re-enable you need to
                        call this function with the same amount.  */
                    /*  this is a macro like function, you can find it in Can_interrupt.c   */
                    EXIT_CAN_CONTROLLER_INTERRUPT_DISABLE(Controller);
                    if( OUT_CAN_CONTROLLER_INTERRUPT_DISABLE() == TRUE && CanControllerInterruptStatus[Controller] != INTERRUPT_ENABLE )
                    {
                        /* set interrupt enable, status interrupt enable, error interrupt enable to disable interrupts."*/
                        CANIntEnable(CanController[Controller].CanControllerBaseAddress, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
                        IntEnable(CanController[Controller].CanInterruptId);
                        CanControllerInterruptStatus[Controller] = INTERRUPT_ENABLE ;
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

            }
        }
        else
        {

#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
            CanDevolpmentErrorType = CAN_E_PARAM_CONTROLLER;
            Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_ENABLE_CONTROLLER_INTERRUPTS_SID,CanDevolpmentErrorType);
#endif
        }
    }
    else {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType=CAN_E_UNINIT;
        Det_ReportError(CAN_MODULE_ID,Controller,CAN_ENABLE_CONTROLLER_INTERRUPTS_SID,CanDevolpmentErrorType);
#endif
    }
#endif


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_GetControllerErrorState**************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/





FUNC(Std_ReturnType,CAN_CODE) Can_GetControllerErrorState( VAR(uint8,AUTOMATIC) ControllerId,P2VAR(Can_ErrorStateType,CAN_CODE,AUTOMATIC)  ErrorStatePtr )
                                                                                                                                                                                                                                                                                                                                                                                                                {

    VAR(Std_ReturnType,AUTOMATIC) RetuenValue =  E_OK ;
    VAR(uint8,AUTOMATIC) CanDevolpmentErrorType = E_OK;

    if( CanDriverStatus==CAN_UNINIT )
    {
        /*[SWS_Can_91005] If development error detection for the Can module is enabled: if the module is not yet initialized,
    the function Can_GetControllerErrorState shall raise development error CAN_E_UNINIT and return E_NOT_OK.*/
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_UNINIT;
        Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_GET_CONTROLLER_ERROR_STATUS_SID,CanDevolpmentErrorType);
#endif
        RetuenValue = E_NOT_OK;
    }
    else
    {

    }
    if( ControllerId >= MAX_NUM_OF_CAN_CONTROLLERS)
    {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_PARAM_CONTROLLER;
        Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_GET_CONTROLLER_ERROR_STATUS_SID,CanDevolpmentErrorType);
#endif
        RetuenValue = E_NOT_OK;
    }
    else
    {

    }

    if( ErrorStatePtr == NULL_PTR )
    {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_PARAM_POINTER ;
        Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_GET_CONTROLLER_ERROR_STATUS_SID,CanDevolpmentErrorType);
#endif
        RetuenValue = E_NOT_OK                       ;
    }
    else
    {

    }

    if ( RetuenValue == E_OK )
    {
        /* Reads one of the controller status registers. (Tivaware)*/

        *ErrorStatePtr  = CANStatusGet(CanController[ControllerId].CanControllerBaseAddress, CAN_STS_CONTROL) ;
        *ErrorStatePtr &= 0xB8U                                                                                 ;

        if (IS_VALID_ERROR(*ErrorStatePtr) == TRUE )
        {
            RetuenValue = E_OK ;
        }
        else
        {
            RetuenValue = E_NOT_OK ;
        }
    }
    else
    {
        /*MISRA*/
    }
    return RetuenValue ;
                                                                                                                                                                                                                                                                                                                                                                                                                }

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_GetControllerMode********************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/



/*[SWS_Can_91015] The service Can_GetControllerMode shall return the mode of the requested CAN controller.*/
FUNC (Std_ReturnType,CAN_CODE) Can_GetControllerMode( VAR(uint8,AUTOMATIC) Controller, P2VAR(Can_ControllerStateType,CAN_CODE,AUTOMATIC)  ControllerModePtr)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
    VAR(Std_ReturnType,AUTOMATIC) toBeReturned = E_OK;
    VAR(uint8,AUTOMATIC) CanDevolpmentErrorType;
    /*  validating inputs  */
    if(Controller < MAX_NUM_OF_CAN_CONTROLLERS)
    {
        if(ControllerModePtr!=NULL_PTR)
        {
            /*  check that the can has been initialized before  */
            if(CanDriverStatus == CAN_READY )
            {
                *ControllerModePtr= CanControllerCurrentMode [Controller];
            }
            else
            {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
                CanDevolpmentErrorType = CAN_E_UNINIT;
                Det_ReportError(CAN_MODULE_ID,Controller,CAN_GET_CONTROLLER_MODE_SID ,CanDevolpmentErrorType);
#endif
                toBeReturned = E_NOT_OK;
            }
        }
        else
        {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
            CanDevolpmentErrorType = CAN_E_PARAM_POINTER;
            Det_ReportError(CAN_MODULE_ID,Controller,CAN_GET_CONTROLLER_MODE_SID ,CanDevolpmentErrorType);
#endif
            toBeReturned = E_NOT_OK;
        }
    }
    else
    {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType=CAN_E_PARAM_CONTROLLER;
        Det_ReportError(CAN_MODULE_ID,CAN_INSTANCE_ID_0,CAN_GET_CONTROLLER_MODE_SID ,CanDevolpmentErrorType);
#endif
        toBeReturned = E_NOT_OK;
    }

    return toBeReturned;

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_Write********************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/


/*[SWS_Can_00275] The function Can_Write shall be non-blocking.*/
FUNC(Std_ReturnType,CAN_CODE) Can_Write(VAR(Can_HwHandleType,AUTOMATIC) HTH ,CONSTP2VAR(Can_PduType,CAN_CODE,AUTOMATIC) PduInfo)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
    VAR(uint8,AUTOMATIC)         CanDevolpmentErrorType = E_OK     ;
    VAR(tCANMsgObject,AUTOMATIC)    sCANMessage                          ;
    VAR(uint32,AUTOMATIC)        REGISTER_1                            ;
    VAR(uint32,AUTOMATIC)        REGISTER_2                            ;
    VAR(Std_ReturnType ,AUTOMATIC)      ErrorStatus  = E_OK              ;
    VAR(uint8,AUTOMATIC)         MailBoxIndex                          ;
    VAR(uint8,AUTOMATIC)         CanControllerId                       ;

    /* [SWS_Can_00100] Several TX hardware objects with unique HTHs may be configured. The
            CanIf module provides the HTH as parameter of the TX request.*/

    /*[SWS_Can_00276] The function Can_Write shall store the swPduHandle that is given
            inside the parameter PduInfo until the Can module calls the CanIf_TxConfirmation
            for this request where the swPduHandle is given as parameter.*/
    /*canTxPDUid_confirmation[pdu_id]=un_sent;*/

    if( CanDriverStatus == CAN_UNINIT )
    {
        /*[SWS_Can_00216] If development error detection for the Can module is enabled: The function
                Can_Write shall raise the error CAN_E_UNINIT and shall return E_NOT_OK if the driver is not yet initia_lized.*/

#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_UNINIT ;
        Det_ReportError(CAN_MODULE_ID , CAN_INSTANCE_ID_0,CAN_WRITE_SID ,CanDevolpmentErrorType);
#endif
        ErrorStatus            = E_NOT_OK     ;
    }
    if ( HTH >= (uint8)MAX_NUM_OF_HO  &&  CanHardwareObject[HTH].CanObjectType == RECEIVE )
    {
        /*[SWS_Can_00217] If development error detection for the Can module is enabled: The function Can_Write
                shall raise the errorCAN_E_PARAM_HANDLE and shall return E_NOT_OK if the parameter Hth is not a
                configured Hardware transmit Handle.*/
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_PARAM_HANDLE;
        Det_ReportError(CAN_MODULE_ID , CAN_INSTANCE_ID_0 ,CAN_WRITE_SID ,CanDevolpmentErrorType);
#endif
        ErrorStatus            = E_NOT_OK     ;

    }
    if( PduInfo == NULL_PTR  || PduInfo->sdu == NULL_PTR)
    {

        /*([SWS_CAN_00503] Can_Write() shall accept a NULL_PTR pointer as SDU (Can_PduType.Can_SduPtrType = NULL_PTR)
                if the trigger transmit API is enabled for this hardware object (CanTriggerTransmitEnable = TRUE).*/

        /*[SWS_CAN_00504] If the trigger transmit API is enabled for the hardware object, Can_Write() shall interpret a NULL_PTR_PTR pointer as
                SDU (Can_PduType.Can_SduPtrType = NULL_PTR_PTR) as request for using the trigger transmit interface.
                If so and the hardware object is free, Can_Write() shall call CanIf_TriggerTransmit() with the maximum size of the message buffer
                 to acquire the PDU’s data. NOT SUPPORTED */

        /*[SWS_CAN_00219] If development error detection for CanDrv is enabled: Can_Write()
                shall raise CAN_E_PARAM_POINTER and shall return E_NOT_OK if the parameter PduInfo
                is a NULL_PTR_PTR pointer.*/

        /*[SWS_CAN_00505] If development error detection for CanDrv is enabled: Can_Write()
                shall raise CAN_E_PARAM_POINTER and shall return E_NOT_OK if the trigger transmit
                API is disabled for this hardware object (CanTriggertransmitEnable = FALSE) and
                the SDU pointer inside PduInfo is a NULL_PTR_PTR pointer.*/
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_PARAM_POINTER;
        Det_ReportError(CAN_MODULE_ID , CAN_INSTANCE_ID_0 ,CAN_WRITE_SID ,CanDevolpmentErrorType);
#endif
        ErrorStatus            = E_NOT_OK     ;
    }


    if(IS_VALID_DATA_LENGTH(PduInfo->length) == FALSE)
    {
#if CAN_GENERAL_CAN_DEV_ERROR_DETECT == STD_ON
        CanDevolpmentErrorType = CAN_E_PARAM_DATA_LENGTH;
        Det_ReportError(CAN_MODULE_ID , CAN_INSTANCE_ID_0 ,CAN_WRITE_SID ,CanDevolpmentErrorType);
#endif

        ErrorStatus            = E_NOT_OK     ;
        /*[SWS_Can_00218] The function Can_Write shall return E_NOT_OK and if development error detection
                          for the CAN module is enabled shall raise the error CAN_E_PARAM_DATA_LENGTH,
                          If the length is more than 8 byte*/
    }
    if (ErrorStatus == E_OK  )
    {
        {
            CanControllerId  = CanHardwareObject[HTH].CanControllerRef ;
            for( MailBoxIndex = CanHardwareObject[HTH].CanMailBoxStart ; MailBoxIndex < ( CanHardwareObject[HTH].CanHwObjectCount + CanHardwareObject[HTH].CanMailBoxStart) ; MailBoxIndex ++)
            {
                /* Check if the corresponde mailbox is empty or not                             */
                if ( Can_MailBoxLookUpTables[ CanControllerId ][ MailBoxIndex ].Confirmation == CONFIRMED )
                {
                    /* Set confirmation to the correspond mailbox                           */
                    Can_MailBoxLookUpTables [ CanControllerId ][ MailBoxIndex ] .Confirmation = UNCONFIRMED                ;
                    /* Map the mailboc to pdu for the Rxindation in the ISR                 */
                    Can_MailBoxLookUpTables [ CanControllerId ][ MailBoxIndex ] .pduId        = PduInfo->swPduHandle       ;
                    sCANMessage.ui32MsgID     =  (uint32)PduInfo ->id                                       ;
                    sCANMessage.ui32MsgIDMask =  (uint32)0U                                                 ;
                    sCANMessage.ui32MsgLen    =  (uint32)PduInfo->length                                    ;
                    sCANMessage.pui8MsgData   = (uint32*)PduInfo ->sdu                                       ;

                    /*[SWS_CAN_00502]  If PduInfo->SduLength does not match possible DLC values CanDrv
                                                shall use the next higher valid DLC for transmission with initialisation of unused bytes
                                                to the value of the corresponding CanFdPaddingValue (see ECUC_Can_00485).*/

                    if( PduInfo->length < MIN_DATA_SIZE )
                    {
                        sCANMessage.ui32MsgLen   =   (uint32) MIN_DATA_SIZE                                                                      ;
                    }
                    else
                    {
                        sCANMessage.ui32MsgLen  =     (uint32)(PduInfo->length)                                                          ;
                    }

                    /* [SWS_Can_00059] Data mapping by CAN to memory is defined in a way that the
                                                  CAN data byte which is sent out first is array element 0, the CAN data byte which
                                                  is sent out last is array element 7 or 63 in case of CAN FD.
                                                 N.B:Our TIVA c doesn't support FD*/
                    CanControllerId = CanHardwareObject[HTH].CanControllerRef                 ;
                    if( CanHardwareObject[HTH].CanIdType == EXTENDED )
                    {
                        sCANMessage.ui32Flags =  ((uint32) MSG_OBJ_TX_INT_ENABLE | (uint32)MSG_OBJ_EXTENDED_ID )       ;
                    }
                    else
                    {
                        sCANMessage.ui32Flags = (uint32)MSG_OBJ_TX_INT_ENABLE                           ;
                    }
#if DEBUG == TRUE
                    /* For test only */
                    //                    UARTprintf("MailBoxSent is %d \n",MailBoxIndex+1);
#endif
                    /*Configures a message object in the CAN controller.(TivaWare)                                                          */
                    CANMessageSet( CanController[CanControllerId].CanControllerBaseAddress, MailBoxIndex+1 ,&sCANMessage, MSG_OBJ_TYPE_TX )         ;
                    break;
                }
                else
                {
                    ErrorStatus            =  CAN_BUSY    ;/*add break here when u clear multi exit points of function*/
                }
            }/*End of for Loop */
        }

        /*[SWS_Can_00011] The Can module shall directly copy the data from the upper
                          layer buffers. It is the responsibility of the upper layer to keep the buffer consistent until return
                          of function call*/
    }
    else
    {
        /* MISRA */
    }
    return ErrorStatus ;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }



/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_MainFunction_Write******************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/

/*[SWS_Can_00031] The function Can_MainFunction_Write shall perform the polling of TX confirmation when CanTxProcessingis
set to POLLING or mixed. In case of mixed processing only the hardware objects for which CanHardwareObjectUsesPolling is set
 to TRUE shall be polled (SRS_BSW_00432, SRS_BSW_00373, SRS_SPAL_00157)*/
FUNC(void,CAN_CODE)  Can_MainFunction_Write(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
#if ( CAN_CONTROLLER_CAN_TX_PROCESSING == POLLING )
    VAR(uint8,AUTOMATIC) CanMailBoxIndex              ;
    P2VAR(uint16,CAN_CODE,AUTOMATIC) pollinRegister  ;
    P2VAR(uint16,CAN_CODE,AUTOMATIC) PtrToReg  ;
    VAR(uint8,AUTOMATIC) Controller = 0    ;
    VAR(uint8,AUTOMATIC) ConfirmedMailBoxIndex        ;
    VAR(uint8,AUTOMATIC) HO_Index        ;

    /*Clear 7th bit in CANx_IF1CMSK_R */
    /* Transfer the data in the CAN message object specified by the MNUM field in the CANIFnCRQ register into the CANIFn registers.*/

    CLR_WRNRD_BIT(CAN0_IF1CMSK_R);
    CLR_WRNRD_BIT(CAN1_IF1CMSK_R);

    /* MISRA violation */
    /*casting pointer to integral type unavoidable when addressing memory mapped registers
          or other hardware specific features.*/

    for ( Controller = CAN_CONTROLLER_ZERO ; Controller < MAX_NUM_CONTROLLER ; Controller++ )
    {
        for( CanMailBoxIndex = 0U ; CanMailBoxIndex < CurrentMailBox[Controller] ; CanMailBoxIndex++ )
        {

            HO_Index = Can_MailBoxLookUpTables[Controller][CanMailBoxIndex].HwObject;
            if( CanHardwareObject[ HO_Index ] .CanObjectType== TRANSMIT )
            {
                if( Controller == CAN_CONTROLLER_ZERO )
                {
                    PtrToReg = ( uint16* )&CAN0_IF1MCTL_R ;
                }
                else
                {
                    PtrToReg = ( uint16* ) &CAN1_IF1MCTL_R ;
                }
                if( Can_MailBoxLookUpTables[Controller][CanMailBoxIndex].Confirmation == UNCONFIRMED)
                {
                    /*A message transfer is started as soon as there is a write of the message object number to the MNUM
                    field when the TXRQST bit in the CANIF1MCTL register is set.*/

                    *PtrToReg = CanMailBoxIndex + 1   ;

                    /*casting pointer to integral type unavoidable when addressing memory mapped
                                           registers or other hardware specific features.*/
                    /*This message object is not waiting for transmission if this bit is cleared  */
                    if(( IS_CONFIRMED(*PtrToReg ) )/*Message_Confirmation[i][j]=un_confirmed*/
                            {

                        Can_MailBoxLookUpTables[Controller][CanMailBoxIndex].Confirmation = CONFIRMED;
#if DEBUG == TRUE
                        UARTprintf("MainFunction : MailBox =%u\n", CanMailBoxIndex +1);

#endif

                        /*[SWS_Can_00016] The Can module shall call CanIf_TxConfirmation to indicate a
                                      successful transmission.It shall either called by the TX-interrupt service routine
                                      of the corresponding HW resource or inside the Can_MainFunction_Write in case of
                                      polling mode.*/

                        CanIf_TxConfirmation(Can_MailBoxLookUpTables[Controller][CanMailBoxIndex].pduId);

                            }
                }
                else
                {

                }
            }
            else
            {
                /* Move to the next hardware object */
                CanMailBoxIndex += (CanHardwareObject[ HO_Index ].CanHwObjectCount -1) ;
            }
        }/*End of For */
    }
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }










/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_MainFunction_Read********************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/



FUNC (void,CAN_CODE)  Can_MainFunction_Read(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
#if (CAN_CONTROLLER_CAN_RX_PROCESSING == POLLING)
    VAR(uint32,AUTOMATIC) mailBoxIndex                  ;
    /*  Buffer to receive data in                                       */
    VAR(uint8,AUTOMATIC) rx_MsgData1[8U]={0U}           ;
    /*  TI struct to use in data reception      (Tivaware)              */
    VAR(tCANMsgObject,AUTOMATIC)  CANMsgObject;
    /*  Point at the buffer defined above                               */
    VAR(CANMsgObject,AUTOMATIC) .pui8MsgData = rx_MsgData1  ;
    /*  PduInfo is needed here for Canif_RxIndication which uses Pdu    */
    VAR(PduInfoType,AUTOMATIC)  rxPduInfo                 ;
    VAR(Can_HwType,AUTOMATIC)  Rx_Mailbox                 ;
    VAR(uint8,AUTOMATIC) Controller                     ;
    VAR(uint8,AUTOMATIC) HOIndex = FIRST_HO             ;

    for ( Controller = CAN_CONTROLLER_ZERO ;Controller < MAX_NUM_CONTROLLER ;Controller++ )
    {
        for ( mailBoxIndex = FIRST_MAIL_BOX ; mailBoxIndex < CurrentMailBox[Controller] ; mailBoxIndex ++)
        {
            HOIndex = Can_MailBoxLookUpTables[Controller][mailBoxIndex].HwObject;
            if( CanHardwareObject[HOIndex].CanObjectType == RECEIVE )
            {
                /*This function shall take mailbox from 1 to 32 */
                CANMessageGet(CanController[Controller].CanControllerBaseAddress, mailBoxIndex + 1, &CANMsgObject, 0U);
                /*Check if the mailbox receive a new data or not */
                if (IS_NEW_DATA_RECEIVED(CANMsgObject.ui32Flags) == TRUE )
                {
                    Rx_Mailbox.Ho            = HOIndex                ;
                    Rx_Mailbox.CanId         = CANMsgObject.ui32MsgID ;
                    Rx_Mailbox.controllerlId = Controller             ;
                    /* [SWS_Can_00060] Data mapping by CAN to memory is defined in a way that the CAN data byte
                which is received first is array element 0 the CAN data byte which is received last is array element 7*/
                    rxPduInfo.SduLength  = CANMsgObject.ui32MsgLen    ;
                    rxPduInfo.SduDataPtr = CANMsgObject.pui8MsgData   ;
                    CanIf_RxIndication(&Rx_Mailbox,&rxPduInfo)        ;
                }
                else
                {

                }
            }
            else
            {
                /* Move to the next hardware object */
                mailBoxIndex += (CanHardwareObject[ HOIndex ].CanHwObjectCount -1) ;
            }


        }
    }
#endif


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }


/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************Can_MainFunction_Mode********************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/


FUNC(void,CAN_CODE) Can_MainFunction_Mode( void )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {

    /*[SWS_Can_00369]  The function Can_MainFunction_Mode shall implement the polling
        of CAN status register flags to detect transition of CAN Controller state. Compare to chapter 7.3.2.*/
    VAR(uint8,AUTOMATIC) Controller                                 ;
    LOCAL   Can_ControllerStateType PreviousState[MAX_NUM_CONTROLLER] = {CAN_CS_STOPPED} ;
    VAR(uint8,AUTOMATIC) RegisterCheck[MAX_NUM_CONTROLLER]                            ;
    for ( Controller = CAN_CONTROLLER_ZERO ; Controller < MAX_NUM_CONTROLLER ; Controller++)
    {
        if( IS_MODE_CHANGED(CanControllerCurrentMode[Controller]) == TRUE )
        {
            if(CanControllerCurrentMode[Controller] != PreviousState[Controller])
            {
                CanControllerModeStatus[Controller] = MODE_NOT_CHANGED                                                      ;
                /*  0 Normal operation  1 Initialization started    */
                RegisterCheck[ CAN_CONTROLLER_ZERO] =  (CAN0_CTL_R);
                RegisterCheck[ CAN_CONTROLLER_ONE ] =  (CAN1_CTL_R);


                if( IS_NORMAL_MODE(CanControllerCurrentMode [Controller],RegisterCheck[ Controller] ) == TRUE )
                {
                    PreviousState[Controller] = CanControllerCurrentMode[Controller];
                }
                else
                {

                }
            }
            else if( IS_INIT_STARTED_MODE(CanControllerCurrentMode [Controller],RegisterCheck[ Controller] ) == TRUE )
            {
                /*TOBEASKED: void CanIf_ControllerModeIndication()*/
                PreviousState[Controller] = CanControllerCurrentMode[ CAN_CONTROLLER_ZERO ];
            }
            else
            {

            }
        }
        else
        {
            /*No mode changed*/
        }
    }/*End of for*/

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }/*End of Can_MainFunction_Mode*/




/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/***************************************************CAN_IRQHandler***************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/




FUNC(void,CAN_CODE) CAN_IRQHandler(VAR(uint8, AUTOMATIC) Controller)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {
#if CAN_CONTROLLER_CAN_TX_PROCESSING == INTERRUPT ||  CAN_CONTROLLER_CAN_RX_PROCESSING == INTERRUPT
    /* Only for development debugging */
    VAR(uint8,AUTOMATIC) ErrorStatus = E_OK;
    /* The mailBoxIndex return type from CANIntStatus  */
    VAR(uint32,AUTOMATIC) mailBoxIndex;
    /*  Buffer to recieve data in   */
    VAR(uint8,AUTOMATIC) rx_MsgData1[8U] = {0U};
    /*  TI struct to use in data reception      (Tivaware)*/
    /*  PduInfo is needed here for Canif_RxIndication which uses Pdu    */

    VAR(PduInfoType,AUTOMATIC)  rxPduInfo;
    VAR(Can_HwType,AUTOMATIC)  Rx_Mailbox;
    VAR(tCANMsgObject,AUTOMATIC)  CANMsgObject;
    /*  Point at the buffer defined above   */
    CANMsgObject.pui8MsgData = rx_MsgData1;
    /*
     *  This function returns a value upon each interrupt.
     *  If the value of the interrupt register is in the range 1-32, then this indicates the number of the
     *  highest priority message object that has an interrupt pending.
     *  Otherwise, there is an error.
     *  (tivaware)
     */

    mailBoxIndex = CANIntStatus(CanController[Controller].CanControllerBaseAddress, CAN_INT_STS_CAUSE);

    CANIntClear(CanController[Controller].CanControllerBaseAddress, mailBoxIndex)                     ;

    if( mailBoxIndex >= FIRST_MAIL_BOX && mailBoxIndex <= LAST_MAIL_BOX )
    {
        /* Can_MailBoxLookUpTables first index is zero while the first the Hw mailbox index is 1 therefore the index is decremented  */
        mailBoxIndex -=  1;
        VAR(uint8,AUTOMATIC) HwObject = Can_MailBoxLookUpTables[Controller][mailBoxIndex ].HwObject;

        if( CanHardwareObject[HwObject].CanObjectType == RECEIVE )
        {
            CANMessageGet( CanController[Controller].CanControllerBaseAddress, (uint32)mailBoxIndex +1 , &CANMsgObject, 0);
            Rx_Mailbox.Ho            = HwObject          ;
            Rx_Mailbox.CanId         = CANMsgObject.ui32MsgID ;
            Rx_Mailbox.controllerlId = Controller             ;

            /* [SWS_Can_00060] Data mapping by CAN to memory is defined in a way that the CAN data byte
            which is received first is array element 0 the CAN data byte which is received last is array element 7*/

            rxPduInfo.SduLength  = CANMsgObject.ui32MsgLen;
            rxPduInfo.SduDataPtr = CANMsgObject.pui8MsgData;

            /*  not here in the confirmation only   */
            CanIf_RxIndication(&Rx_Mailbox,&rxPduInfo);
        }
        else if ( CanHardwareObject[HwObject].CanObjectType == TRANSMIT )
        {
            Can_MailBoxLookUpTables[Controller][mailBoxIndex ].Confirmation = CONFIRMED;
            CanIf_TxConfirmation(Can_MailBoxLookUpTables[Controller][mailBoxIndex ].pduId);
        }
        else
        {

        }
    }
    else
    {
        ErrorStatus = E_NOT_OK ;
        /* ERROR */

    }
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        }



