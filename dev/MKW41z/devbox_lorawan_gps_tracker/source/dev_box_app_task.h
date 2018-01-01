#ifndef _APP_H_
#define _APP_H_

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_handles.h"

/*************************************************************************************
 **************************************************************************************
 * Public macros
 **************************************************************************************
 *************************************************************************************/
/* Profile Parameters */

#define gFastConnMinAdvInterval_c       32 /* 20 ms */
#define gFastConnMaxAdvInterval_c       48 /* 30 ms */

#define gReducedPowerMinAdvInterval_c   1600 /* 1 s */
#define gReducedPowerMaxAdvInterval_c   4000 /* 2.5 s */

#define gFastConnAdvTime_c              300  /* s */
#define gReducedPowerAdvTime_c          300  /* s */

#if gAppUseBonding_d
#define gFastConnWhiteListAdvTime_c     10 /* s */
#else
#define gFastConnWhiteListAdvTime_c     0
#endif

/************************************************************************************
 *************************************************************************************
 * Public memory declarations
 *************************************************************************************
 ********************************************************************************** */
extern gapAdvertisingData_t gAppAdvertisingData;
extern gapScanResponseData_t gAppScanRspData;
extern gapAdvertisingParameters_t gAdvParams;
extern gapSmpKeys_t gSmpKeys;
extern gapPairingParameters_t gPairingParameters;
extern gapDeviceSecurityRequirements_t deviceSecurityRequirements;

/*! Dev Box App Task Stack Size */
#define gDevBoxAppTaskStackSize_c (2048)

/*! Dev Box App Task OS Abstraction Priority */
#define gDevBoxAppTaskPriority_c  (OSA_PRIORITY_NORMAL)

/* Event ID to sent new data to task or to request a new LoRaWAN to be sent
 * cf. below to see the events possible */
extern osaEventId_t gDevBoxAppEvent;

/* Task Events */
/* <Set by the LoRaWAN controller task when a network as been successfully joined> */
#define gDevBoxTaskEvtNewLoRaWANConfig_c       						(1 << 0)

/* <Set by the GPS task when need data are ready to be process> */
/* <Data can be read from the gGpsNewMessageMeasureQ Queue> */
#define gDevBoxTaskEvtNewGgpMeasureAvailable_c       				(1 << 1)

/* <Set by the BME680 task when need data are ready to be process> */
/* <Data can be read from the gBme680NewMessageMeasureQ Queue> */
#define gDevBoxTaskEvtNewBme680MeasureAvailable_c       			(1 << 2)

/* <Set by the BNO055 task when need data are ready to be process> */
/* <Data can be read from the gBno055NewMessageMeasureQ Queue> */
#define gDevBoxTaskEvtNewBno055MeasureAvailable_c       			(1 << 3)

/* <Set by the BLE Scanner when need data are ready to be process> */
/* <Data can be read from the gBleScannerNewMessageMeasureQ Queue> */
#define gDevBoxTaskEvtNewBleScannerMeasureAvailable_c       		(1 << 4)

/* <Set when the user want to send new data to the LoRaWAN network> */
/* <Data should be sent to the gLorawanCtrlSendNewMessageQ Queue> */
#define gDevBoxEvtSendNewLoRaData_c       							(1 << 5)

/* <Set by the LoRaWAN controller when new data have been received> */
/* <Data can be read from the gLorawanCtrlReceiveNewMessageQ Queue> */
#define gDevBoxEvtNewLoRaDataReceived_c 							(1 << 6)


#define BLE_SCANNER_MEASURE_QUEUE_SIZE 								4

extern osaMsgQId_t gBleScannerNewMessageMeasureQ;

typedef struct bleScannerData_tag
{
	uint16_t bleBeaconsFound;
} bleScannerData_t;

/************************************************************************************
 *************************************************************************************
 * Public prototypes
 *************************************************************************************
 ************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

void BleApp_Init(void);

void BleApp_Start(void);

void BleApp_GenericCallback(gapGenericEvent_t* pGenericEvent);

void DevBox_App_Task(osaTaskParam_t argument);

osaStatus_t DevBoxApp_TaskInit(void);

#ifdef __cplusplus
}
#endif 

#endif /* _APP_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
