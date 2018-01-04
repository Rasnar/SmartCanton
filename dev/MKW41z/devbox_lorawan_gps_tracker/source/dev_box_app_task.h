/**
 * @file    dev_box_app_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Contain the main task the handle everything in the application. 
 * 			Will receive data from sensors, store it, create a Cayenne frame
 * 			and send it every X seconds through a LoRaWAN network using the 
 * 			LoRaWAN controller task.
 *			It also implement the callbacks from the Host task to control the
 * 			Bluetooth client registration. Provide data to the implemented
 * 			BLE services.
 */

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
/* Bluetooth Profile Parameters */
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

/* Dev Box App Task Stack Size */
#define gDevBoxAppTaskStackSize_c (2048)

/* Dev Box App Task OS Abstraction Priority */
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

/* Queue to hold the data to be send to the task consumming the data */
extern osaMsgQId_t gBleScannerNewMeasureQ;

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

/**
 * @brief Initializes application specific functionality before the BLE stack init.
 */
void BleApp_Init(void);

/**
 * @brief Starts the BLE application.
 */
void BleApp_Start(void);

/**
 * @brief Handles BLE generic callback.
 * 
 * @param pGenericEvent Pointer to gapGenericEvent_t.
 */
void BleApp_GenericCallback(gapGenericEvent_t* pGenericEvent);

/**
 * @brief Main application task. Handling data from sensors and sending periodically
 * data using the LoRaWAN stack
 * 
 * @param argument Parameters to pass to the task (ignored)
 */
void DevBox_App_Task(osaTaskParam_t argument);

/**
 * @brief Init the main task components, such as event handle and timers.
 * 
 * @return osaStatus_t osaStatus_Success if correctly initialized, otherwise
 * osaStatus_Error.
 */
osaStatus_t DevBoxApp_TaskInit(void);

#ifdef __cplusplus
}
#endif 

#endif /* _APP_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
