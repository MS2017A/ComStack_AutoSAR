 /***************************************************
 *
 * File Name: Com_cfg.h 
 *
 * Author: AUTOSAR COM Team 
 * 
 * Date Created: Jun 2020	
 * 
 * Version	: 4.0	
 * 
 ****************************************************/
#ifndef COM_CFG_H_
#define COM_CFG_H_
#define COM_NUM_OF_TX_IPDU  1
#define COM_NUM_OF_IPDU     2
#define COM_NUM_OF_SIGNAL   4
#define COM_NUM_OF_GROUP_SIGNAL 3


/* signal definition */
#define SW_1_TX 						0
#define SW_2_TX 						1
#define SW_1_RX 						2
#define SW_2_RX 						3
//#define CanDB_Signal_29_12_BE_Tester 3
//#define CanDB_Signal_32_21_BE 4
//#define CanDB_Signal_1_4_LE 5



/************************************************comGeneral****************************************************************/
/*parameter: cancellation feature
	range: true, false
	default_value: True
	//name from sws & _
	signal group not now
*/
#define	COM_CANCELLATION_SUPPORT					TRUE

/*error detection*/
#define COM_DEV_ERROR_DETECT					TRUE

/*Enables globally for the whole Com module the minimum delay time moni-toring for cyclic and repeated transmissions 
  (ComTxModeMo-de=PERIODIC or ComTxModeMode=MIXED
	default_value: True
*/
#define COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION		TRUE

/* The period between successive calls to Com_MainFunctionRx in ms */
#define COM_RX_TIMEBASE 								((float32)0.1)

/* The period between successive calls to Com_MainFunctionTx in ms */
#define COM_TX_TIMEBASE 								((float32)0.2)


#endif



