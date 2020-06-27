#ifndef CANIF_DET_H_
#define CANIF_DET_H_

/* Det defines */

#define CANIF_E_PARAM_CANID                 (uint8) 10
#define CANIF_E_PARAM_HOH                   (uint8) 12
#define CANIF_E_PARAM_LPDU                  (uint8) 13
#define CANIF_E_PARAM_CONTROLLERID          (uint8) 15
#define CANIF_E_PARAM_WAKEUPSOURCE          (uint8) 16
#define CANIF_E_PARAM_TRCV                  (uint8) 17
#define CANIF_E_PARAM_TRCVMODE              (uint8) 18
#define CANIF_E_PARAM_TRCVWAKEUPMODE        (uint8) 19
#define CANIF_E_PARAM_CTRLMODE              (uint8) 21
#define CANIF_E_PARAM_PDU_MODE              (uint8) 22
#define CANIF_E_PARAM_POINTER               (uint8) 20
#define CANIF_E_UNINIT                      (uint8) 30
#define CANIF_E_INVALID_TXPDUID             (uint8) 50
#define CANIF_E_INVALID_RXPDUID             (uint8) 60
#define CANIF_E_INIT_FAILED                 (uint8) 80
#define CANIF_E_INVALID_DATA_LENGTH         (uint8) 61


#define CANIF_INIT_ID                        (uint8)0x01
#define CANIF_INIT_CONTROLLER_ID             (uint8)0x02
#define CANIF_SET_CONTROLLER_MODE_ID         (uint8)0x03
#define CANIF_GET_CONTROLLER_MODE_ID         (uint8)0x04
#define CANIF_GET_CONTROLLER_ERROR_STATE_ID  (uint8)0x4B
#define CANIF_TRANSMIT_ID                    (uint8)0x05
#define CANIF_READTXPDUDATA_ID               (uint8)0x06
#define CANIF_READTXNOTIFSTATUS_ID           (uint8)0x07
#define CANIF_READRXNOTIFSTATUS_ID           (uint8)0x08
#define CANIF_SET_PDU_MODE_ID                (uint8)0x09
#define CANIF_GET_PDU_MODE_ID                (uint8)0x0A
#define CANIF_CHECKVALIDATION_ID             (uint8)0x12
#define CANIF_TX_CONFIRMATION_ID             (uint8)0x13
#define CANIF_RX_INDICATION_ID               (uint8)0x14

#define CANIF_MODULE_ID                      (uint16) MODULE_ID_CANIF
#define CANIF_INSTANCE_ID                    (uint8)0



#endif /* CANIF_DET_H_ */
