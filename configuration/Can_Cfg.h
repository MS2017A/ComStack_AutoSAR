#ifndef INCLUDES_CAN_CFG_H_
#define INCLUDES_CAN_CFG_H_

#define DEBUG    TRUE

#define  MAX_NUM_OF_HO     30
#define MAX_NUM_CONTROLLER  2



#define  Max_CanHwObjectCount             1U
#define  MAX_NUM_OF_CAN_CONTROLLERS       2U

#define MAX_NUM_FILTERS                   5U


/*************  All these macros are parameters in the CanGeneral struct, but have been moved them out because we are using precombiled configurations   **************/

#define  CAN_GENERAL_CAN_DEV_ERROR_DETECT                   STD_ON

#define  CAN_CONTROLLER_ACTIVATION                          True

#define  CAN_CONTROLLER_CAN_TX_PROCESSING                   INTERRUPT
#define  CAN_CONTROLLER_CAN_RX_PROCESSING                   INTERRUPT
#define  CAN_SET_BAUDRATE_API                               TRUE

#define  MAX_NUM_OF_BAUDRATES  6U









#endif /* INCLUDES_CAN_CFG_H_ */

