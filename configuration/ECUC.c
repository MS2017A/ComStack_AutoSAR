#include "Std_Types.h"
#include "ComStack_Types.h"
#include "CanIf_Cfg.h"
#include "CanTp_Cfg.h"
#include "Com_Cfg.h"


CONST( PduIdType,mem_class ) CanIf_UpperLayerUser_LookUpTable_Rx[ MAX_NUM_RX_PDU ] =
{
 0,
 1,

};
CONST( PduIdType,mem_class ) CanIf_UpperLayerUser_LookUpTable_Tx[MAX_NUM_TX_PDU  ] =
{
 0,
 1,
 2,
 3
};

CONST( PduIdType,mem_class ) CanTp_LowerLayerUser_LookUpTable_Tx[NUMBER_OF_TXNPDU + NUMBER_OF_RXNPDU]   = {
                                                                                                           0,
                                                                                                           1,
                                                                                                           2,
                                                                                                           3,

}   ;
CONST( PduIdType,mem_class ) CanTp_UpperLayerUser_LookUpTable_Tx[COM_NUM_OF_IPDU] = {


} ;
CONST( PduIdType,mem_class ) CanTp_UpperLayerUser_LookUpTable_Rx[COM_NUM_OF_IPDU] = {

} ;
CONST( PduIdType,mem_class ) Pdur_LowerLayerUser_LookUpTable[COM_NUM_OF_IPDU] = {


}     ;






