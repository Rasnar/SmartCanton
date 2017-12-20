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

#define gFastConnAdvTime_c              3000  /* s */
#define gReducedPowerAdvTime_c          30000 /* s */

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

/*! Idle Task Stack Size */
#define gDevBoxAppTaskStackSize_c (2048)

/*! Lorawan Controller Task OS Abstraction Priority */
#define gDevBoxAppTaskPriority_c  (5)

extern osaEventId_t gDevBoxAppEvent;

/* Task Events */
#define gDevBoxTaskEvtNewLoRaWANConfig_c       		(1 << 0)
#define gDevBoxTaskEvtNewGPSDataRdy_c       		(1 << 1)
#define gDevBoxTaskEvtNewBME680Measure_c       		(1 << 2)
#define gDevBoxTaskEvtNewBNO055Measure_c       		(1 << 3)
#define gDevBoxEvtSendNewLoRaData_c       			(1 << 4)


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
