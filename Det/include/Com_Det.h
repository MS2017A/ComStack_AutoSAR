#ifndef DET_INCLUDE_COM_DET_H_
#define DET_INCLUDE_COM_DET_H_


#define COM_E_PARAM                                 ((uint8)0x01)
#define COM_E_PARAM_POINTER                         ((uint8)0x03)
#define COM_E_SKIPPED_TRANSMISSION                  ((uint8)0x05)

#define COM_INIT_ID                                 ((uint8)0x01)
#define COM_SEND_SIGNAL_ID                          ((uint8)0x0a)
#define COM_RECEIVE_SIGNAL_ID                       ((uint8)0x0b)
#define COM_SEND_SIGNAL_GROUP_ID                    ((uint8)0x0d)
#define COM_RECEIVE_SIGNAL_GROUP_ID                 ((uint8)0x0e)
#define COM_TRIGGER_IPDU_SEND_ID                    ((uint8)0x17)
#define COM_RX_INDICATION_ID                        ((uint8)0x42)
#define COM_TP_RX_INDICATION_ID                     ((uint8)0x45)
#define COM_TX_CONFIRMATION_ID                      ((uint8)0x40)
#define COM_TP_TX_CONFIRMATION_ID                   ((uint8)0x48)
#define COM_START_OF_RECEPTION_ID                   ((uint8)0x46)
#define COM_COPY_RX_DATA_ID                         ((uint8)0x44)
#define COM_COPY_TX_DATA_ID                         ((uint8)0x43)
#define COM_MAIN_FUNCTION_RX_ID                     ((uint8)0x18)
#define COM_MAIN_FUNCTION_TX_ID                     ((uint8)0x19)



#endif /* DET_INCLUDE_COM_DET_H_ */
