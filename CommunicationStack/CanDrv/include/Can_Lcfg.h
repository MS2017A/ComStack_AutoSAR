/********************************************************************************************
 *                                                                                          *
 * File Name   : Can_Lcfg.h                                                                 *
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
#ifndef CAN_LCFG_H_
#define CAN_LCFG_H_




/*
 *This container contains bit timing related configuration parameters of the CAN /
 * controller(s).The Multiplicity is 1..many /
 */
extern CONST( CanControllerBaudrateConfig_s, AUTOMATIC ) CanControllerBaudrateConfig[MAX_NUM_OF_BAUDRATES ]  ;
/*
 * Describes a mask for hardware-based filtering of CAN identifiers./
 * The CAN identifiers of incoming messages are masked with the appropriate/
 * CanFilterMaskValue.Bits holding a 0 mean don't care,/
 * i.e. do not compare the message's identifier in the respective bit position./
 * The mask shall be build by filling with leading 0./
 * The In case of CanIdType EXTENDED or MIXED a 29 bit mask shall
 */

 extern VAR(uint32,AUTOMATIC) CanHwFilterCfg[MAX_NUM_FILTERS];
/*
 * This container contains the configuration parameters of the CAN controller(s).
 */
extern VAR(CanController_s,AUTOMATIC) CanController[MAX_NUM_OF_CAN_CONTROLLERS]  ;
/*
 * This container contains the configuration (parameters) of CAN Hardware Objects.
 */
extern VAR(CanHardwareObject_s,AUTOMATIC) CanHardwareObject[ MAX_NUM_OF_HO ];




#endif /* CAN_LCFG_H_ */
