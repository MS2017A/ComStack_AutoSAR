#ifndef CAN_DET_H_
#define CAN_DET_H_



#define   CAN_E_PARAM_POINTER          1U
#define   CAN_E_PARAM_HANDLE           2U
#define   CAN_E_PARAM_DATA_LENGTH      3U
#define   CAN_E_PARAM_CONTROLLER       4U
#define   CAN_E_UNINIT                 5U
#define   CAN_E_TRANSITION             6U
#define   CAN_E_PARAM_BAUDRATE         7U
#define   CAN_E_ICOM_CONFIG_INVALID    8U
#define   CAN_E_INIT_FAILED            9U
/*Not standard in SWS       */
#define   CAN_E_NOT_VALID_MAILBOX      20U





/* Service Ids                                                                                                                   */
#define CAN_INIT_SID                           (uint8)0x00
#define CAN_MAINFUNCTION_WRITE_SID             (uint8)0x01
#define CAN_INIT_CONTROLLER_SID                (uint8)0x02
#define CAN_SET_CONTROLLER_MODE_SID            (uint8)0x03
#define CAN_SET_BAUDRATE_SID                   (uint8)0x0F
#define CAN_GET_CONTROLLER_MODE_SID            (uint8)0x12
#define CAN_DISABLE_CONTROLLER_INTERRUPTS_SID  (uint8)0x04
#define CAN_ENABLE_CONTROLLER_INTERRUPTS_SID   (uint8)0x05
#define CAN_WRITE_SID                          (uint8)0x06
#define CAN_MAIN_FUNCTION_READ_SID             (uint8)0x08
#define CAN_DEINIT_SID                         (uint8)0x10
#define CAN_GET_CONTROLLER_ERROR_STATUS_SID    (uint8)0x11
#define CAN_INSTANCE_ID_0                      (uint8)CAN_CONTROLLER_ZERO
#define CAN_INSTANCE_ID_1                      (uint8)CAN_CONTROLLER_ONE
#define CAN_MODULE_ID                          (uint16)MODULE_ID_CAN


#endif /* CAN_DET_H_ */
