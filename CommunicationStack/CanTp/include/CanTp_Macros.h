/********************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Macros.h                                                             *
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

#ifndef CANTP_MACROS_H_
#define CANTP_MACROS_H_

#define SEGMENT_NUMBER_MASK                            (uint8)0x0F

#define MAX_FRAME_BYTES                                 8U

#define MAX_PAYLOAD_STANDRAD_SF                         7U
#define MAX_PAYLOAD_STANDRAD_FF                         6U
#define MAX_PAYLOAD_STANDRAD_CF                         7U

#define MAX_PAYLOAD_EXTENDED_SF                         6U
#define MAX_PAYLOAD_EXTENDED_FF                         5U
#define MAX_PAYLOAD_EXTENDED_CF                         6U


#define CANTP_OFF                                       0U
#define CANTP_ON                                        1U

#define CANTP_RX_WAIT                                   (CanTp_TransferInstanceMode)0x00
#define CANTP_RX_PROCESSING                             (CanTp_TransferInstanceMode)0x01
#define CANTP_TX_WAIT                                   (CanTp_TransferInstanceMode)0x02
#define CANTP_TX_PROCESSING                             (CanTp_TransferInstanceMode)0x03

#define INVALID_FRAME                                                    (FrameType)0x00
#define SINGLE_FRAME                                                     (FrameType)0x01
#define FIRST_FRAME                                                      (FrameType)0x02
#define CONSECUTIVE_FRAME                                                (FrameType)0x03
#define FLOW_CONTROL_CTS_FRAME                                           (FrameType)0x04
#define FLOW_CONTROL_WAIT_FRAME                                          (FrameType)0x05
#define FLOW_CONTROL_OVERFLOW_FRAME                                      (FrameType)0x06

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

#define CANTP_MODE_FULL_DUPLEX                                     (ChannelModeType)0x00
#define CANTP_MODE_HALF_DUPLEX                                     (ChannelModeType)0x01

#define CANTP_FUNCTIONAL                                                    (TaType)0x00
#define CANTP_PHYSICAL                                                      (TaType)0x01

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

#endif /* CANTP_MACROS_H_ */
