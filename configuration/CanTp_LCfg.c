

#include "ComStack_Types.h"
#include "CanTp_Types.h"
#include "CanTp_Cfg.h"

const CanTpTxNSdu_s CanTpTxNSdu[NUMBER_OF_TXNPDU] =
{
		{
				.CanTpNas                       = 6              ,
				.CanTpNbs                       = 20             ,
				.CanTpNcs                       = 10             ,
				.CanTpTc                        = TRUE           ,
				.CanTpTxAddressingFormat        = CANTP_STANDARD ,
				.CanTpTxNSduId                  = 0              ,
				.CanTpTxPaddingActivation       = CANTP_OFF      ,
				.CanTpTxTaType                  = CANTP_PHYSICAL ,
				.CanTpTxNPduConfirmationPduId   = 0              ,
				.CanTpRxFcNPduId                = 0		         ,
		},

		{
				.CanTpNas                       = 6              ,
				.CanTpNbs                       = 20             ,
				.CanTpNcs                       = 10             ,
				.CanTpTc                        = TRUE           ,
				.CanTpTxAddressingFormat        = CANTP_STANDARD ,
				.CanTpTxNSduId                  = 1              ,
				.CanTpTxPaddingActivation       = CANTP_OFF      ,
				.CanTpTxTaType                  = CANTP_PHYSICAL ,
				.CanTpTxNPduConfirmationPduId   = 1              ,
				.CanTpRxFcNPduId                = 1		         ,
		},
};


const CanTpRxNSdu_s CanTpRxNSdu[NUMBER_OF_RXNPDU] =
{
		{
				.CanTpBs                      	 = 10             ,
				.CanTpSTmin                   	 = 3              ,
				.CanTpRxWftMax                 	 = 5              ,
				.CanTpNar                     	 = 5              ,
				.CanTpNbr                        = 10             ,
				.CanTpNcr                        = 20             ,
				.CanTpRxAddressingFormat         = CANTP_STANDARD ,
				.CanTpRxNSduId                   = 0              ,
				.CanTpRxNPduId                   = 0              ,
				.CanTpRxPaddingActivation        = CANTP_OFF      ,
				.CanTpRxTaType                   = CANTP_PHYSICAL ,
				.CanTpTxFcNPduConfirmationPduId  = 2              ,
		},
		{
				.CanTpBs                      	 = 20             ,
				.CanTpSTmin                   	 = 3              ,
				.CanTpRxWftMax                	 = 5              ,
				.CanTpNar                     	 = 5              ,
				.CanTpNbr                        = 10             ,
				.CanTpNcr                        = 20             ,
				.CanTpRxAddressingFormat         = CANTP_STANDARD ,
				.CanTpRxNSduId                   = 1              ,
				.CanTpRxNPduId                   = 1              ,
				.CanTpRxPaddingActivation        = CANTP_ON       ,
				.CanTpRxTaType                   = CANTP_PHYSICAL ,
				.CanTpTxFcNPduConfirmationPduId  = 3              ,
		}
};



ChannelInfo_s ChannelInfo[MAX_CHANNEL_COUNT] =
{
		{
				.Mode         	 =			 CANTP_MODE_FULL_DUPLEX ,
				.StIdx			 = 				 0                  ,
				.Length			 = 				 1                  ,
		},
		{
				.Mode            =           CANTP_MODE_FULL_DUPLEX ,
				.StIdx           =               1                  ,
				.Length          =               1                  ,
		}
};


