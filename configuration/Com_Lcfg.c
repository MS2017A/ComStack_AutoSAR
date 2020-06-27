
/***************************************************
 *
 * File Name: Com_cfg.c 

 *
 * Author: AUTOSAR COM Team 
 * 
 * Date Created: Jun 2020
 * 
 * Version  : 4.0
 * 
 ****************************************************/

#include "Com.h"

/* signals Buffer */
uint8 ComSignalBuffer_1 [1];
uint8 ComSignalBuffer_2 [1];
uint8 ComSignalBuffer_3 [1];
uint8 ComSignalBuffer_4 [1];
#if 0
uint8 ComSignalBuffer_4 [2];
uint8 ComSignalBuffer_5 [1];
uint8 ComSignalBuffer_6 [4];
#endif
const ComSignal_type ComSignals[] = 
{
 {	/*signal0*/
  	.ComBitPosition= 0,
  	.ComUpdateBitPosition= 18 ,
  	.ComUpdateBitEnabled=FALSE,
  	.ComBitSize= 1,
  	.ComSignalType = BOOLEAN,
  	.ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
  	.ComIPduHandleId=0,
  	.ComNotification=NULL_PTR,
  	.ComSignalDataPtr = ComSignalBuffer_1
 },
 {	/*signal1*/
  	.ComBitPosition= 8,
  	.ComUpdateBitPosition= 19,
  	.ComUpdateBitEnabled=FALSE,
  	.ComBitSize= 1,
  	.ComSignalType = BOOLEAN,
  	.ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
  	.ComIPduHandleId=0,
  	.ComNotification=NULL_PTR,
  	.ComSignalDataPtr = ComSignalBuffer_2
 },
 {	//signal2
  	.ComBitPosition= 0,
  	.ComUpdateBitPosition= 18 ,
  	.ComUpdateBitEnabled=FALSE,
  	.ComBitSize= 8,
  	.ComSignalType = UINT8,
  	.ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
  	.ComIPduHandleId=1,
  	.ComNotification=NULL_PTR,
  	.ComSignalDataPtr = ComSignalBuffer_3
 },
 {   //signal3
     .ComBitPosition= 8,
     .ComUpdateBitPosition= 19 ,
     .ComUpdateBitEnabled=FALSE,
     .ComBitSize= 8,
     .ComSignalType = UINT8,
     .ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
     .ComIPduHandleId=1,
     .ComNotification=NULL_PTR,
     .ComSignalDataPtr = ComSignalBuffer_4
 }
#if 0   
 ,{   //signal0
      .ComBitPosition= 0,
      .ComUpdateBitPosition= 16 ,
      .ComHandleId= 3 ,
      .ComBitSize= 16,
      .ComSignalType = UINT16,
      .ComTransferProperty = TRIGGERED_ON_CHANGE,
      .ComIPduHandleId=1,
      .ComSignalDataPtr = ComSignalBuffer_4
 },
 {   //signal1
     .ComBitPosition= 17,
     .ComUpdateBitPosition= 25,
     .ComHandleId= 4 ,
     .ComBitSize= 8,
     .ComSignalType = BOOLEAN,
     .ComTransferProperty = TRIGGERED_ON_CHANGE,
     .ComIPduHandleId=1,
     .ComNotification=NULL_PTR,
     .ComSignalDataPtr = ComSignalBuffer_5
 },
 {   //signal2
     .ComBitPosition= 26,
     .ComUpdateBitPosition= 58 ,
     .ComHandleId= 5 ,
     .ComBitSize= 32,
     .ComSignalType = UINT32,
     .ComTransferProperty = TRIGGERED_ON_CHANGE,
     .ComIPduHandleId=1,
     .ComNotification=NULL_PTR,
     .ComSignalDataPtr = ComSignalBuffer_6
 },
 {	//signal3
  	.ComBitPosition= 0,
  	.ComUpdateBitPosition= 16 ,
  	.ComHandleId= 6 ,
  	.ComBitSize= 16,
  	.ComSignalType = UINT16,
  	.ComTransferProperty = TRIGGERED,
  	.ComIPduHandleId=2,
  	.ComNotification=NULL_PTR,
  	.ComSignalDataPtr = ComSignalBuffer_6

 },
 {	//signal4
  	.ComBitPosition= 17,
  	.ComUpdateBitPosition= 33 ,
  	.ComHandleId= 4 ,
  	.ComBitSize= 16,
  	.ComSignalType = UINT16,
  	.ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
  	.ComIPduHandleId=1,
  	.ComNotification=NULL_PTR,
  	.ComSignalDataPtr = ComSignalBuffer_5
 },
 {   //signal5
     .ComBitPosition= 0,
     .ComUpdateBitPosition= 64 ,
     .ComHandleId= 5,
     .ComBitSize= 64,
     .ComSignalType = UINT64,
     .ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
     .ComIPduHandleId=2,
     .ComNotification=NULL_PTR,
     .ComSignalDataPtr = ComSignalBuffer_6
 }
#endif
};

/* IPdu buffers and signal */
uint8 ComIPduBuffer_1[3];

uint8 ComIPduBuffer_2[3];

//uint8 ComIPduBuffer_3[8];

/* Com Ipdu */
const ComIPdu_type ComIPdus[] =
{
 { // CanDB_Message_1
   .ComIPduDirection = SEND ,
   .ComIPduSize=3,
   .ComIPduHandleId = 0 ,
   .ComIPduSignalRef = &ComSignals[SW_1_TX],
   .ComIPduDataPtr=ComIPduBuffer_1,
   .ComIPduType = NORMAL,
   .ComIPduSignalProcessing = IMMEDIATE,
   .ComIPduNumOfSignals = 2,
   .ComTxIPdu = 0

#if 0
   {
       .ComTxModeFalse =
       {
        .ComTxMode=
        {
         .ComTxModeMode = PERIODIC,
         .ComTxModeNumberOfRepetitions = 2,
         .ComTxModeTimePeriod=200,
         .ComTxModeRepetitionPeriod=200,
        }
       },

       .ComMinimumDelayTime = 0,
       .ComTxIPduClearUpdateBit = TRANSMIT,
       .ComTxIPduUnusedAreasDefault = 0,

   }
#endif
 },
 { // CanDB_Message_2
   .ComIPduDirection = RECEIVE ,
   .ComIPduSize=3,
   .ComIPduSignalProcessing = IMMEDIATE,
   .ComIPduHandleId = 1 ,
   .ComIPduSignalRef = &ComSignals[SW_1_RX],
   .ComIPduDataPtr=ComIPduBuffer_1,
   .ComIPduType = NORMAL,
   .ComIPduSignalProcessing = IMMEDIATE,
   .ComIPduNumOfSignals = 2,

#if 0
   .ComTxIPdu =
   {
    .ComTxModeFalse =
    {
     .ComTxMode=
     {
      .ComTxModeMode = DIRECT,
      .ComTxModeNumberOfRepetitions = 10,
      .ComTxModeTimePeriod=500,
      .ComTxModeRepetitionPeriod=200
     }
    },
    .ComMinimumDelayTime = 0,
    .ComTxIPduUnusedAreasDefault = 0,
   }
#endif
 },
#if 0
 { // CanDB_Message_2
   .ComIPduDirection = SEND ,
   .ComIPduSize=8,
   .ComIPduHandleId = 2 ,
   .ComIPduSignalRef =ComIPduSignalRefs_Can_Message_3,
   .ComIPduDataPtr=ComIPduBuffer_3,
   .ComIPduType = TP,
   .ComTxIPdu =
   {
    .ComTxModeFalse =
    {
     .ComTxMode=
     {
      .ComTxModeMode = MIXED,
      .ComTxModeNumberOfRepetitions = 2,
      .ComTxModeTimePeriod = 100,
      .ComTxModeRepetitionPeriod = 1000
     }
    },

    .ComMinimumDelayTime = 0,
    .ComTxIPduUnusedAreasDefault = 0,

   }
 },
 { // CanDB_Message_3
   .ComIPduDirection = SEND ,
   .ComIPduSize=8,
   .ComIPduHandleId = 3 ,
   .ComIPduSignalRef =ComIPduSignalRefs_Can_Message_3,
   .ComIPduDataPtr=ComIPduBuffer_3,
   .ComTxIPdu =
   {
    .ComTxModeFalse =
    {
     .ComTxMode=
     {
      .ComTxModeMode = NONE,
      .ComTxModeNumberOfRepetitions = 2,
      .ComTxModeTimePeriod = 10,
      .ComTxModeRepetitionPeriod = 100
     }
    },

    .ComMinimumDelayTime = 0,
    .ComTxIPduUnusedAreasDefault = 0,

   }
 }
#endif
};

ComTxIPdu_type ComTxIPdus[]=
{
 {
  .ComMinimumDelayTime = 0.1,
  .ComTxModeTimePeriod = 0.2,
  .ComTxIPduClearUpdateBit = TRANSMIT,
  .ComTxIPduUnusedAreasDefault = 0x0,
  .ComTxModeMode = MIXED,
 }
};
