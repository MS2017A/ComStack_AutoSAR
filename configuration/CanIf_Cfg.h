
#ifndef CANIF_CFG_H_
#define CANIF_CFG_H_


#define MAX_NUM_RX_PDU                      (uint8)5
#define MAX_NUM_TX_PDU                      (uint8)4

#define MAX_NUM_HTH                         (uint8)5
#define MAX_NUM_HRH                         (uint8)4

#define MAX_CANIF_CTRL                      (uint8)10


#define CANIF_DEV_ERROR_DETECT                   STD_ON


#define DUBGE                                    TRUE
#define CANIF_PRIVATE_DATA_LENGTH_CHECK          TRUE

/* These macros shall be set to TRUE for enabling CanIf_RxIndication */
#define CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API TRUE
#define CANIF_RXPDU_READ_NOTIFYSTATUS            TRUE


#endif /* CANIF_CFG_H_ */
