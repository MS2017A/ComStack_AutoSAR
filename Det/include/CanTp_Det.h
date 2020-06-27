 /*******************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Det.h                                                                *
 *                                                                                          *
 * Author      : ITI Intake 40 AUTOSAR                                                      *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Date        : 6 Jun 2020                                                                 *
 *                                                                                          *
 * Version     : 4.0.0                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR canTp Macros used by Det  Release 4.3.1              *
 *                                                                                          *
 ********************************************************************************************/

#ifndef CANTP_DET_H_
#define CANTP_DET_H_


#define CANTP_MODULE_ID                                (0x0E) /* CanTp Module Id */
#define CANTP_INSTANCE_ID                              (0x00)  /* CanTp Instance Id */

/* Runtime Errors */
#define CANTP_E_PARAM_CONFIG                            0x01
#define CANTP_E_PARAM_ID                                0x02
#define CANTP_E_PARAM_POINTER                           0x03
#define CANTP_E_INIT_FAILED                             0x04
#define CANTP_E_UNINIT                                  0x20
#define CANTP_E_INVALID_TX_ID                           0x30
#define CANTP_E_INVALID_RX_ID                           0x40
#define CANTP_E_PADDING                                 0x70
#define CANTP_E_INVALID_TATYPE                          0x90
#define CANTP_E_OPER_NOT_SUPPORTED                      0xA0
#define CANTP_E_COM                                     0xB0
#define CANTP_E_RX_COM                                  0xC0
#define CANTP_E_TX_COM                                  0xD0

/* Service ID for API's */
#define CANTP_INIT_SERVICE_ID                           0x01
#define CANTP_TRANSMIT_SERVICE_ID                       0x49
#define CANTP_MAINFUNCTION_SERVICE_ID                   0x06
#define CANTP_RXINDICATION_SERVICE_ID                   0x42
#define CANTP_TXCONFIRMATION_SERVICE_ID                 0x40

#endif /* CANTP_DET_H_ */
