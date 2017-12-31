/*! *********************************************************************************
 * \addtogroup Heart Rate Sensor
 * @{
 ********************************************************************************** */
/*!
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * \file
 *
 * This file is the source file for the Heart Rate Sensor application
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/

#include "dev_box_app_task.h"

/* Connection Manager */
#include "ble_conn_manager.h"

/* Framework / Drivers */
#include "RNG_Interface.h"
#include "Keyboard.h"
#include "LED.h"
#include "TimersManager.h"
#include "FunctionLib.h"
#include "MemManager.h"
#include "Panic.h"

#if (cPWR_UsePowerDownMode)
#include "PWR_Interface.h"
#include "PWR_Configuration.h"
#endif

/* Profile / Services */
#include "battery_interface.h"
#include "device_info_interface.h"
#include "smartcanton_devbox_lora_interface.h"
#include "smartcanton_devbox_gps_interface.h"
#include "smartcanton_devbox_bme680_interface.h"
#include "smartcanton_devbox_bno055_interface.h"
#include "lorawan_controller.h"

#include "pin_mux.h"
#include "board.h"
#include "neo-m8.h"
#include "bno055_support.h"
#include "bme680_bsec_support.h"
#include "gpio_pins.h"

#include "ApplMain.h"

#include "CayenneLPP.h"

/* Necessary to communicate to the LoRaWAN task */
#include "lorawan_controller_task.h"
#include "bno055_task.h"
#include "bme680_task.h"
#include "gps_task.h"

/************************************************************************************
 *************************************************************************************
 * Public memory declarations
 *************************************************************************************
 ************************************************************************************/
OSA_TASK_DEFINE(DevBox_App_Task, gDevBoxAppTaskPriority_c, 1, gDevBoxAppTaskStackSize_c, FALSE);

osaTaskId_t gDevBoxAppTaskId = 0;

/* Event ID to send new data to LoRaWAN task or save data from sensors */
osaEventId_t gDevBoxAppEvent;


extern gapAdvertisingParameters_t   gAdvParams;
extern gapScanningParameters_t      gAppScanParams;
extern gapConnectionRequestParameters_t gConnReqParams;

#define mBleAppCmdsCount_c                20
#define mBleAppMaxScannedDevicesCount_c   20
#define mBleAppMaxDeviceNameLength_c      20

typedef struct gapScannedDevices_tag
{
    bleAddressType_t    addrType;
    bleDeviceAddress_t  aAddress;
    uchar_t             name[mBleAppMaxDeviceNameLength_c];
}gapScannedDevices_t;

static gapScannedDevices_t     mScannedDevices[mBleAppMaxScannedDevicesCount_c];
static uint8_t                 mScannedDevicesCount;

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
#define mBatteryLevelReportInterval_c   (30)  /* battery level BLE report interval in seconds  */
#define mLoRaNewDataReportInterval_default_c   (60)   /* LoRaWAN new data interval in seconds  */

/************************************************************
 * Cayenne application configuration
 ************************************************************/
/* This LoRaWAN downlink port can't be changed with Cayenne */
#define mCayenneDefaultPortDownlinkLoRaWAN		99

/* Uplink and downlink Cayenne ports */
#define mCayenneChannelGps									1

#define mCayenneChannelBno055Accelerometer					2
#define mCayenneChannelBno055Gyroscope						3

#define mCayenneChannelBme680Temperature					4
#define mCayenneChannelBme680Humidity						5
#define mCayenneChannelBme680Pressure						6
#define mCayenneChannelBme680Iaq							8

#define mCayenneChannelBatteryLevel							10

#define mCayenneChannelLed2DigitalOutput					50

#define mCayenneChannelLoRaPacketIntervalOutput				60

#define mCayenneChannelEnableGpsDataInLoRaPacket			100
#define mCayenneChannelEnableBme680DataInLoRaPacket			101
#define mCayenneChannelEnableBno055DataInLoRaPacket			102
#define mCayenneChannelEnableBleScannerDataInLoRaPacket		103
#define mCayenneChannelEnableBleBatteryLevelInLoRaPacket	104

/************************************************************************************
 *************************************************************************************
 * Private type definitions
 *************************************************************************************
 ************************************************************************************/
typedef enum
{
#if gAppUseBonding_d
	fastWhiteListAdvState_c,
#endif
	fastAdvState_c, slowAdvState_c
} advType_t;

typedef struct advState_tag
{
	bool_t advOn;
	advType_t advType;
} advState_t;

typedef struct scnState_tag
{
	bool_t scnOn;
} scnState_t;

/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/

/* Adv State */
static advState_t mAdvState;
static bool_t mRestartAdv;
static uint32_t mAdvTimeout;
static deviceId_t mPeerDeviceId = gInvalidDeviceId_c;

/* Scanning state */
static scnState_t mScanningState;

/* Service Data*/
static basConfig_t basServiceConfig =
{ service_battery, 0 };

static scdbGPSConfig_t scdbGPSServiceConfig =
{ service_smartcanton_devbox_gps };

static scdbBme680Config_t scdbBme680ServiceConfig =
{ service_smartcanton_devbox_bme680 };

static scdbBno055Config_t scdbBno055ServiceConfig =
{ service_smartcanton_devbox_bno055 };

static lorawanControllerConfiguration_t loraConfig;
static scdbLoRaConfig_t scdbLoRaServiceConfig =
{ service_smartcanton_devbox_lora, &loraConfig };

static uint16_t writeHandles[10] =
{ value_lora_app_eui, value_lora_app_key, value_lora_device_eui, value_lora_confirm_mode,
		value_lora_device_address, value_lora_network_session_key, value_lora_app_session_key,
		value_lora_validate_new_configuration, value_bno055_measure_interval };

static uint16_t readHandles[2] =
{ value_lora_network_join_status, value_bno055_measure_interval };

/* Application specific data*/
static tmrTimerID_t mAdvTimerId;
static tmrTimerID_t mBatteryMeasurementTimerId;
static tmrTimerID_t mLoRaSendNewFrameTimerId;

/* indicate the interval beetwen each LoRa packet
 * This value can be modified by a LoRa Downlink request (*/
static int mLoRaNewDataReportInterval_c = mLoRaNewDataReportInterval_default_c;

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/

/* Gatt and Att callbacks */
static void BleApp_AdvertisingCallback(gapAdvertisingEvent_t* pAdvertisingEvent);
static void BleApp_ConnectionCallback(deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent);
static void BleApp_GattServerCallback(deviceId_t deviceId, gattServerEvent_t* pServerEvent);
static void BleApp_Config();

/* Timer Callbacks */
static void AdvertisingTimerCallback(void *);
static void BatteryMeasurementTimerCallback(void *);

static void BleApp_Advertise(void);

static void BleApp_SetScanParameters();
static void BleApp_ParseScannedDevice(gapScannedDevice_t* pData);
static void BleApp_ScanningCallback (gapScanningEvent_t* pScanningEvent);

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
 * \brief    Initializes application specific functionality before the BLE stack init.
 *
 ********************************************************************************** */
void BleApp_Init(void)
{
	/* Initialize application support for drivers */
	BOARD_InitPins();
	BOARD_InitAdc();
}

/*! *********************************************************************************
 * \brief    Starts the BLE application.
 *
 ********************************************************************************** */
void BleApp_Start(void)
{
	/* Device is not connected and not advertising*/
	if (!mAdvState.advOn)
	{
#if gAppUseBonding_d
		if (gcBondedDevices > 0)
		{
			mAdvState.advType = fastWhiteListAdvState_c;
		}
		else
		{
#endif
			mAdvState.advType = fastAdvState_c;
#if gAppUseBonding_d
		}
#endif
		BleApp_Advertise();
	}

#if (cPWR_UsePowerDownMode)    
	PWR_ChangeDeepSleepMode(1); /* MCU=LLS3, LL=DSM, wakeup on GPIO/LL */
	PWR_AllowDeviceToSleep();
#endif       

	//BleApp_SetScanParameters();
	if(!mScanningState.scnOn) {
		bleResult_t result = Gap_StartScanning(&gAppScanParams, BleApp_ScanningCallback, TRUE);
		result = result;
	}
}

static void BleApp_SetScanParameters()
{

	uint16_t interval = 1 *1000/625;
	gAppScanParams.interval = interval;
	gConnReqParams.scanInterval = interval;

	uint16_t window = 0x10;
	gAppScanParams.window = window;
	gConnReqParams.scanWindow = window;

	gAppScanParams.type = gScanTypePassive_c;

}

static void BleApp_ParseScannedDevice(gapScannedDevice_t* pData)
{
    uint8_t index = 0;
    uint8_t nameLength;

    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t)pData->data[index + 1];
        adElement.aData = &pData->data[index + 2];

        if ((adElement.adType == gAdShortenedLocalName_c) ||
          (adElement.adType == gAdCompleteLocalName_c))
        {
            nameLength = MIN(adElement.length-1, mBleAppMaxDeviceNameLength_c);
            FLib_MemCpy(mScannedDevices[mScannedDevicesCount].name, adElement.aData, nameLength);

            printf("%s",(char*)mScannedDevices[mScannedDevicesCount].name);
            printf(" ");
        }

        /* Move on to the next AD elemnt type */
        index += adElement.length + sizeof(uint8_t);
    }

    //printf(pData->aAddress, sizeof(bleDeviceAddress_t));

    /* Temporary store scanned data to use for connection */
    mScannedDevices[mScannedDevicesCount].addrType = pData->addressType;
    FLib_MemCpy(mScannedDevices[mScannedDevicesCount].aAddress,
                pData->aAddress,
                sizeof(bleDeviceAddress_t));

    printf("%d",pData->rssi);
    printf(" dBm");

    mScannedDevicesCount++;
}

/*! *********************************************************************************
 * \brief        Handles BLE Scanning callback from host stack.
 *
 * \param[in]    pScanningEvent    Pointer to gapScanningEvent_t.
 ********************************************************************************** */
static void BleApp_ScanningCallback (gapScanningEvent_t* pScanningEvent)
{
	switch (pScanningEvent->eventType)
	{
	case gDeviceScanned_c:
	{
		if (mScannedDevicesCount >= mBleAppMaxScannedDevicesCount_c)
		{
			break;
		}

		BleApp_ParseScannedDevice(&pScanningEvent->eventData.scannedDevice);
		break;
	}

	case gScanStateChanged_c:
	{
		mScanningState.scnOn = !mScanningState.scnOn;
		break;
	}

	case gScanCommandFailed_c:
	{
		break;
	}
	default:
		break;
	}
}


/*! *********************************************************************************
 * \brief        Handles keyboard events.
 *
 * \param[in]    events    Key event structure.
 ********************************************************************************** */
void BleApp_HandleKeys(key_event_t events)
{
	switch (events)
	{
	case gKBD_EventPressPB1_c:
	{
		if (mPeerDeviceId == gInvalidDeviceId_c)
		{
			BleApp_Start();
		}
		break;
	}
	case gKBD_EventPressPB2_c:
	{
		/* Force the task to send a new frame to the network */
		OSA_EventSet(gDevBoxAppEvent, gDevBoxEvtSendNewLoRaData_c);
	}
	break;
	case gKBD_EventLongPB1_c:
	{
		if (mPeerDeviceId != gInvalidDeviceId_c)
		{
			Gap_Disconnect(mPeerDeviceId);
		}
		break;
	}
	case gKBD_EventLongPB2_c:
	{

		break;
	}
	default:
		break;
	}
}

/*! *********************************************************************************
 * \brief        Handles BLE generic callback.
 *
 * \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
 ********************************************************************************** */
void BleApp_GenericCallback(gapGenericEvent_t* pGenericEvent)
{
	/* Call BLE Conn Manager */
	BleConnManager_GenericEvent(pGenericEvent);

	switch (pGenericEvent->eventType)
	{
	case gInitializationComplete_c:
	{
		BleApp_Config();
	}
	break;

	case gAdvertisingParametersSetupComplete_c:
	{
		App_StartAdvertising(BleApp_AdvertisingCallback, BleApp_ConnectionCallback);
	}
	break;

	/**
	 * TODO : Only here for developpement. No need to press button to start advertissements
	 */
	case gAdvertisingDataSetupComplete_c:
	{
		BleApp_Start();
	}
	break;

	default:
		break;
	}
}

/************************************************************************************
 *************************************************************************************
 * Private functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
 * \brief        Configures BLE Stack after initialization. Usually used for
 *               configuring advertising, scanning, white list, services, et al.
 *
 ********************************************************************************** */
static void BleApp_Config()
{
	/* Configure as GAP peripheral */
	BleConnManager_GapPeripheralConfig();

	/* Register for callbacks*/
	GattServer_RegisterHandlesForWriteNotifications(NumberOfElements(writeHandles), writeHandles);
	GattServer_RegisterHandlesForReadNotifications(NumberOfElements(readHandles), readHandles);
	App_RegisterGattServerCallback(BleApp_GattServerCallback);

	mAdvState.advOn = FALSE;

	/* Start services */

	basServiceConfig.batteryLevel = BOARD_GetBatteryLevel();
	Bas_Start(&basServiceConfig);

	/**
	 * Get last configuration store in the flash memory
	 * /!\ Be sure that the function lorawan_controller_init(void)
	 * as been called before (eg. from another task)
	 */
	*scdbLoRaServiceConfig.loRaCtrlConfig = lorawan_controller_get_current_configuration();
	ScDbLoRa_Start(&scdbLoRaServiceConfig);

	ScDbGPS_Start(&scdbGPSServiceConfig);

	ScDbBno055_Start(&scdbBno055ServiceConfig);

	ScDbBme680_Start(&scdbBme680ServiceConfig);

	/* Allocate application timers */
	mAdvTimerId = TMR_AllocateTimer();
	mBatteryMeasurementTimerId = TMR_AllocateTimer();

#if (cPWR_UsePowerDownMode)    
	PWR_ChangeDeepSleepMode(3); /* MCU=LLS3, LL=IDLE, wakeup on GPIO/LL */
	PWR_AllowDeviceToSleep();
#endif    
}

/*! *********************************************************************************
 * \brief        Configures GAP Advertise parameters. Advertise will satrt after
 *               the parameters are set.
 *
 ********************************************************************************** */
static void BleApp_Advertise(void)
{
	switch (mAdvState.advType)
	{
#if gAppUseBonding_d
	case fastWhiteListAdvState_c:
	{
		gAdvParams.minInterval = gFastConnMinAdvInterval_c;
		gAdvParams.maxInterval = gFastConnMaxAdvInterval_c;
		gAdvParams.filterPolicy = gProcessWhiteListOnly_c;
		mAdvTimeout = gFastConnWhiteListAdvTime_c;
	}
	break;
#endif
	case fastAdvState_c:
	{
		gAdvParams.minInterval = gFastConnMinAdvInterval_c;
		gAdvParams.maxInterval = gFastConnMaxAdvInterval_c;
		gAdvParams.filterPolicy = gProcessAll_c;
		mAdvTimeout = gFastConnAdvTime_c - gFastConnWhiteListAdvTime_c;
	}
	break;

	case slowAdvState_c:
	{
		gAdvParams.minInterval = gReducedPowerMinAdvInterval_c;
		gAdvParams.maxInterval = gReducedPowerMinAdvInterval_c;
		gAdvParams.filterPolicy = gProcessAll_c;
		mAdvTimeout = gReducedPowerAdvTime_c;
	}
	break;
	}

	/* Set advertising parameters*/
	Gap_SetAdvertisingParameters(&gAdvParams);
}

/*! *********************************************************************************
 * \brief        Handles BLE Advertising callback from host stack.
 *
 * \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
 ********************************************************************************** */
static void BleApp_AdvertisingCallback(gapAdvertisingEvent_t* pAdvertisingEvent)
{
	switch (pAdvertisingEvent->eventType)
	{
	case gAdvertisingStateChanged_c:
	{
		mAdvState.advOn = !mAdvState.advOn;

		if (!mAdvState.advOn && mRestartAdv)
		{
			BleApp_Advertise();
			break;
		}

#if (cPWR_UsePowerDownMode)
		if(!mAdvState.advOn)
		{
			Led4Off();
			PWR_ChangeDeepSleepMode(3);
			PWR_SetDeepSleepTimeInMs(cPWR_DeepSleepDurationMs);
			PWR_AllowDeviceToSleep();
		}
		else
		{
			PWR_ChangeDeepSleepMode(1);
			/* Start advertising timer */
			TMR_StartLowPowerTimer(mAdvTimerId,gTmrLowPowerSecondTimer_c,
					TmrSeconds(mAdvTimeout), AdvertisingTimerCallback, NULL);
			Led4On();
		}
#else
		StopLed4Flashing();

		if (mAdvState.advOn)
		{
			Led4Flashing();
			TMR_StartLowPowerTimer(mAdvTimerId, gTmrLowPowerSecondTimer_c, TmrSeconds(mAdvTimeout),
					AdvertisingTimerCallback, NULL);
		}
#endif 
	}
	break;

	case gAdvertisingCommandFailed_c:
	{
		panic(0, 0, 0, 0);
	}
	break;

	default:
		break;
	}
}

/*! *********************************************************************************
 * \brief        Handles BLE Connection callback from host stack.
 *
 * \param[in]    peerDeviceId        Peer device ID.
 * \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
 ********************************************************************************** */
static void BleApp_ConnectionCallback(deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
{
	/* Connection Manager to handle Host Stack interactions */
	BleConnManager_GapPeripheralEvent(peerDeviceId, pConnectionEvent);

	switch (pConnectionEvent->eventType)
	{
	case gConnEvtConnected_c:
	{
		mPeerDeviceId = peerDeviceId;

		/* Advertising stops when connected */
		mAdvState.advOn = FALSE;

		/* Subscribe client*/
		Bas_Subscribe(peerDeviceId);
		ScDbLoRa_Subscribe(peerDeviceId);
		ScDbGPS_Subscribe(peerDeviceId);
		ScDbBno055_Subscribe(peerDeviceId);
		ScDbBme680_Subscribe(peerDeviceId);

#if (!cPWR_UsePowerDownMode)  
		/* UI */
		StopLed4Flashing();
		Led4On();
#endif            

		/* Stop Advertising Timer*/
		mAdvState.advOn = FALSE;
		TMR_StopTimer(mAdvTimerId);

		/* Start battery measurements */
		TMR_StartLowPowerTimer(mBatteryMeasurementTimerId,
				gTmrLowPowerIntervalMillisTimer_c, TmrSeconds(mBatteryLevelReportInterval_c),
				BatteryMeasurementTimerCallback, NULL);

#if (cPWR_UsePowerDownMode)
		PWR_SetDeepSleepTimeInMs(900);
		PWR_ChangeDeepSleepMode(1);
		PWR_AllowDeviceToSleep();
#endif
	}
	break;

	case gConnEvtDisconnected_c:
	{
		/* Unsubscribe client */
		Bas_Unsubscribe();
		ScDbLoRa_Unsubscribe();
		ScDbGPS_Unsubscribe();
		ScDbBme680_Unsubscribe();
		ScDbBno055_Unsubscribe();

		mPeerDeviceId = gInvalidDeviceId_c;

		TMR_StopTimer(mBatteryMeasurementTimerId);

#if (cPWR_UsePowerDownMode)            
		/* UI */
		Led4Off();

		/* Go to sleep */
		PWR_ChangeDeepSleepMode(3); /* MCU=LLS3, LL=IDLE, wakeup on swithes/LL */
		PWR_SetDeepSleepTimeInMs(cPWR_DeepSleepDurationMs);
		PWR_AllowDeviceToSleep();
#else
		if (pConnectionEvent->eventData.disconnectedEvent.reason == gHciConnectionTimeout_c)
		{
			/* Link loss detected*/
			BleApp_Start();
		}
		else
		{
			/* Connection was terminated by peer or application */
			BleApp_Start();
		}
#endif			
	}
	break;
	default:
		break;
	}
}

/*! *********************************************************************************
 * \brief        Handles GATT server callback from host stack.
 *
 * \param[in]    deviceId        Peer device ID.
 * \param[in]    pServerEvent    Pointer to gattServerEvent_t.
 ********************************************************************************** */
static void BleApp_GattServerCallback(deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
	uint16_t handle;
	uint8_t status;

	switch (pServerEvent->eventType)
	{

	/**
	 * The gEvtCharacteristicCccdWritten_c is only used for debugging. We can see
	 * when we have reached the maximum CCCD element when the result from Gap_SaveCccd
	 * is gDevDbCccdLimitReached_c. In this case, we need to remove a notification
	 * characteristic.
	 */
	case gEvtCharacteristicCccdWritten_c:
	{
		handle = pServerEvent->eventData.charCccdWrittenEvent.handle;
		gattCccdFlags_t cccd = pServerEvent->eventData.charCccdWrittenEvent.newCccd;
		volatile bleResult_t result = Gap_SaveCccd(deviceId, handle, cccd);
		result = result;
	}
	break;

	case gEvtAttributeWritten_c:
	{
		handle = pServerEvent->eventData.attributeWrittenEvent.handle;
		status = gAttErrCodeNoError_c;

		if (handle == value_lora_app_eui)
		{
			status = ScDbLoRa_SetAppEui(&scdbLoRaServiceConfig,
					(uint8_array_t){ pServerEvent->eventData.attributeWrittenEvent.cValueLength,
				pServerEvent->eventData.attributeWrittenEvent.aValue });
		}

		if (handle == value_lora_app_key)
		{
			status = ScDbLoRa_SetAppKey(&scdbLoRaServiceConfig,
					(uint8_array_t){ pServerEvent->eventData.attributeWrittenEvent.cValueLength,
				pServerEvent->eventData.attributeWrittenEvent.aValue });
		}

		if (handle == value_lora_confirm_mode)
		{
			status = ScDbLoRa_SetConfirmMode(&scdbLoRaServiceConfig,
					(uint8_array_t){ pServerEvent->eventData.attributeWrittenEvent.cValueLength,
				pServerEvent->eventData.attributeWrittenEvent.aValue });
		}

		if (handle == value_lora_validate_new_configuration)
		{
			OSA_EventSet(gLoRaControllerEvent,
					gLoRaCtrlTaskEvtConfigureFromModuleConfig_c);
		}

		if (handle == value_bno055_measure_interval)
		{
			status = Bno055Task_SetMeasureInterval(pServerEvent->eventData.attributeWrittenEvent.aValue[0] |
					pServerEvent->eventData.attributeWrittenEvent.aValue[1] << 8);
		}

		GattServer_SendAttributeWrittenStatus(deviceId, handle, status);
	}
	break;
	case gEvtAttributeRead_c:
	{

		handle = pServerEvent->eventData.attributeWrittenEvent.handle;
		status = gAttErrCodeNoError_c;

		/*
		 * To be sure to have the last value of the state we ask directly the
		 * LoRaWAN module to know the state of the connection.
		 * TODO: It's not a clean thing to do. Event if it's only a read and
		 * the read is thread safe, we should only access the LoRaWAN module
		 * inside the LoRaWAN task. Should be modified in the future to be
		 * integrated to the LoRaWAN task.
		 */
		if (handle == value_lora_network_join_status)
		{
			char strJoinStatus[32];
			uint8_t joinStatus = 0;
			uint8_array_t joinStatusArray =
			{ .arrayLength = 1,
					.pUint8_array = &joinStatus };
			int bytesRead = lorawan_controller_get_cmd(
					CMD_GET_NETWORK_JOIN_STATUS,
					strJoinStatus, sizeof(strJoinStatus));
			if (bytesRead < 0)
			{
				break;
			}

			if (strcmp(strJoinStatus, "1") == 0)
			{
				joinStatus = 1;
			}
			else
			{
				joinStatus = 0;
			}

			status = ScDbLoRa_SetJoinStatus(&scdbLoRaServiceConfig, joinStatusArray);
		}

		/* Update the measure interval inside the database in case something else has changed it */
		if (handle == value_bno055_measure_interval) {
			ScDbBno055_RecordValueMeasureInterval(service_smartcanton_devbox_bno055,
					Bno055Task_GetMeasureInterval());
		}

		GattServer_SendAttributeReadStatus(deviceId, handle, status);
	}
	break;
	default:
		break;
	}
}

/*! *********************************************************************************
 * \brief        Handles advertising timer callback.
 *
 * \param[in]    pParam        Calback parameters.
 ********************************************************************************** */
static void AdvertisingTimerCallback(void * pParam)
{
	/* Stop and restart advertising with new parameters */
	Gap_StopAdvertising();

	switch (mAdvState.advType)
	{
#if gAppUseBonding_d
	case fastWhiteListAdvState_c:
	{
		mAdvState.advType = fastAdvState_c;
		mRestartAdv = TRUE;
	}
	break;
#endif
	case fastAdvState_c:
	{
		mAdvState.advType = slowAdvState_c;
		mRestartAdv = TRUE;
	}
	break;

	default:
	{
		mRestartAdv = FALSE;
	}
	break;
	}
}

/*! *********************************************************************************
 * \brief        Handles battery measurement timer callback.
 *
 * \param[in]    pParam        Callback parameters.
 ********************************************************************************** */
static void BatteryMeasurementTimerCallback(void * pParam)
{
	basServiceConfig.batteryLevel = BOARD_GetBatteryLevel();
	Bas_RecordBatteryMeasurement(basServiceConfig.serviceHandle, basServiceConfig.batteryLevel);
}

/*! *********************************************************************************
 * \brief        Handles battery measurement timer callback.
 *
 * \param[in]    pParam        Callback parameters.
 ********************************************************************************** */
static void LoRaSendNewDataTimerCallback(void * pParam)
{
	/* Notify the main task to send a new LoRa packet with the latest available data */
	OSA_EventSet(gDevBoxAppEvent, gDevBoxEvtSendNewLoRaData_c);
}

osaStatus_t DevBoxApp_TaskInit(void)
{
	if (gDevBoxAppTaskId)
	{
		return osaStatus_Error;
	}

	/* Create application event */
	gDevBoxAppEvent = OSA_EventCreate(TRUE);
	if ( NULL == gDevBoxAppEvent)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	mLoRaSendNewFrameTimerId = TMR_AllocateTimer();

	/* Task creation */
	gDevBoxAppTaskId = OSA_TaskCreate(OSA_TASK(DevBox_App_Task), NULL);

	if ( NULL == gDevBoxAppTaskId)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}


	return osaStatus_Success;
}

void DevBox_App_Task(osaTaskParam_t argument)
{
	/* Constants to invalidate the data stored locally */
	static const struct minmea_sentence_rmc EmptyframeRmcGps;
	static const bme680Data_t EmptyBme680;
	static const bno055Data_t EmptyBno055;

	/* Last data received from the GPS */
	gpsData_t gpsData;

	/* Last data received from the bno055 */
	bno055Data_t bno055Data;

	/* Last data received from the bme680 */
	bme680Data_t bme680Data;

	/* Data to be sent using the LoRaWAN module */
	lorawanControllerDataToSend_t* lorawanControllerData;

	/* Contain the last event reveiced */
	osaEventFlags_t event;

	/* Infinite loop */
	while (1)
	{
		/* Wait to receive an event from a task to do an action */
		OSA_EventWait(gDevBoxAppEvent, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &event);

		/* Event from the LoRaWAN controller indicating that the LoRaWAN module is ready to be used.
		 * A valid network has been join and we can update the GATT table with the latest information.
		 * We can also start sending data to this network. */
		if (event & gDevBoxTaskEvtNewLoRaWANConfig_c)
		{
			if (lorawan_controller_get_configuration_validity() == lorawanController_Success)
			{
				*scdbLoRaServiceConfig.loRaCtrlConfig = lorawan_controller_get_current_configuration();
				ScDbLoRa_UpdateAllGattTable(&scdbLoRaServiceConfig);

				/* If the timer is already running, stop if first before restarting it */
				if(TMR_IsTimerActive(mLoRaSendNewFrameTimerId))
					TMR_StopTimer(mLoRaSendNewFrameTimerId);

				/* Start timer to notify application every mLoRaNewDataReportInterval_c
				 * to send a new LoRaWAN frame to the network with the last values available */
				TMR_StartLowPowerTimer(mLoRaSendNewFrameTimerId,
						gTmrLowPowerIntervalMillisTimer_c, TmrSeconds(mLoRaNewDataReportInterval_c),
						LoRaSendNewDataTimerCallback, NULL);
			}
		}

		/* Event received when a new GPS frame is available to be read */
		if (event & gDevBoxTaskEvtNewGgpMeasureAvailable_c)
		{
			/* Last data received from the BME680 */
			gpsData_t* gpsData_tmp;

			/* Retrieve data pointer */
			while (OSA_MsgQGet(gGpsNewMessageMeasureQ, &gpsData_tmp, 0) == osaStatus_Success)
			{
				/* Store value in local in case we want to use it later on */
				FLib_MemCpy(&gpsData, gpsData_tmp, sizeof(gpsData));

				/* Destroy tmp buffer allocated by the bme680_task */
				vPortFree(gpsData_tmp);

				/* Send data as notification to connected BLE peer if there is one
				 * and stored them inside the BLE database for future reads */
				ScDbGPS_RecordNotificationFrameRmcAll(service_smartcanton_devbox_gps,
						&gpsData.frame_rmc);
			}
		}

		/* Event received when a new measure to the BME680 as to be done */
		if (event & gDevBoxTaskEvtNewBme680MeasureAvailable_c)
		{
			/* Last data received from the BME680 */
			bme680Data_t* bme680Data_tmp;

			/* Retrieve data pointer */
			while (OSA_MsgQGet(gBme680NewMessageMeasureQ, &bme680Data_tmp, 0) == osaStatus_Success)
			{
				/* Store value in local in case we want to use it later on */
				FLib_MemCpy(&bme680Data, bme680Data_tmp, sizeof(bme680Data));

				/* Destroy tmp buffer allocated by the bme680_task */
				vPortFree(bme680Data_tmp);

				/* Send data as notification to connected BLE peer if there is one */
				ScDbBme680_InstantValueNotificationAll(service_smartcanton_devbox_bme680, &bme680Data);
			}
		}

		/* Event received when a new measure to the BNO055 has been done */
		if (event & gDevBoxTaskEvtNewBno055MeasureAvailable_c)
		{
			/* Last data received from the BNO055 */
			bno055Data_t* bno055Data_tmp;

			/* Retrieve data pointer */
			while (OSA_MsgQGet(gBno055NewMessageMeasureQ, &bno055Data_tmp, 0) == osaStatus_Success)
			{
				/* Store value in local in case we want to use it later on */
				FLib_MemCpy(&bno055Data, bno055Data_tmp, sizeof(bno055Data));

				/* Destroy tmp buffer allocated by the bno055_task */
				vPortFree(bno055Data_tmp);

				/* Send data as notification to connected BLE peer if there is one */
				ScDbBno055_InstantValueNotificationAll(service_smartcanton_devbox_bno055, &bno055Data);
			}
		}

		/* Event received when the user want to send a new data to the LoRaWAN module */
		if (event & gDevBoxEvtSendNewLoRaData_c)
		{
			/* Only send data if the configuration is correct (the network as been joined) */
			if (lorawan_controller_get_configuration_validity() == lorawanController_Success)
			{
				// Reset the buffer for a new Cayenne frame
				cayenneLPPreset();

				/* Send data only if new values since last transmission */
				if (gpsData.frame_rmc.valid)
				{
					cayenneLPPaddGPS(mCayenneChannelGps,
							minmea_tocoord(&gpsData.frame_rmc.latitude), // Latitude
							minmea_tocoord(&gpsData.frame_rmc.longitude), // Longitude
							0.0); // Altitude (not supported with RMC parsing)

					gpsData.frame_rmc = EmptyframeRmcGps; // Invalidate the local data for the next msg
				}

				/* Send data only if new values since last transmission */
				if(!((bno055Data.accel_xyz.x == 0.0) &&
						(bno055Data.accel_xyz.y == 0.0) &&
						(bno055Data.accel_xyz.z == 0.0) &&
						(bno055Data.gyro_xyz.x == 0.0) &&
						(bno055Data.gyro_xyz.y == 0.0) &&
						(bno055Data.gyro_xyz.x == 0.0)))
				{
					cayenneLPPaddAccelerometer(mCayenneChannelBno055Accelerometer,
							bno055Data.accel_xyz.x / 1000.0,
							bno055Data.accel_xyz.y / 1000.0,
							bno055Data.accel_xyz.z / 1000.0);

					cayenneLPPaddGyrometer(mCayenneChannelBno055Gyroscope,
							bno055Data.gyro_xyz.x,
							bno055Data.gyro_xyz.y,
							bno055Data.gyro_xyz.z);
					bno055Data = EmptyBno055; // Invalidate the local data for the next msg
				}

				/* Send data only if new values since last transmission */
				if(!((bme680Data.temperature == 0.0) &&
						(bme680Data.humidity == 0.0) &&
						(bme680Data.pressure == 0.0)))
				{
					cayenneLPPaddTemperature(mCayenneChannelBme680Temperature,
							bme680Data.temperature);
					cayenneLPPaddRelativeHumidity(mCayenneChannelBme680Humidity,
							bme680Data.humidity);
					cayenneLPPaddBarometricPressure(mCayenneChannelBme680Pressure,
							bme680Data.pressure);
					cayenneLPPaddAnalogInput(mCayenneChannelBme680Iaq,
							bme680Data.iaq);

					bme680Data = EmptyBme680; // Invalidate the local data for the next msg
				}

				/* Read the current battery level */
				cayenneLPPaddAnalogInput(mCayenneChannelBatteryLevel,
						(float)BOARD_GetBatteryLevel());

				/* Read the current LED2 state and send it */
				cayenneLPPaddDigitalOutput(mCayenneChannelLed2DigitalOutput,
						!GpioReadOutputPin(&ledPins[1]));

				//cayenneLPPaddAnalogOutput(3, 120.0);
				//cayenneLPPaddDigitalOutput(4, 1);

				/* Allocate data on the HEAP to send it to the LoRaWAN task */
				lorawanControllerData = pvPortMalloc(sizeof(lorawanControllerDataToSend_t));
				lorawanControllerData->dataLength = cayenneLPPgetSize();

				/* Copy data formatted previously inside the Cayenne functions */
				FLib_MemCpy(lorawanControllerData->data, cayenneLPPgetBuffer(),
						lorawanControllerData->dataLength);

				/* Send and notify LoRaWAN task to send this new message */
				OSA_MsgQPut(gLorawanCtrlSendNewMessageQ, &lorawanControllerData);
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);
			}
		}

		/* Event received when the LoRaWAN as received a new Downlink message */
		if (event & gDevBoxEvtNewLoRaDataReceived_c)
		{
			/* Last data received from the BNO055 */
			lorawanControllerDataReceived_t* lorawanDataReceived;

			/* Retrieve data pointer */
			while (OSA_MsgQGet(gLorawanCtrlReceiveNewMessageQ, &lorawanDataReceived, 0)
					== osaStatus_Success)
			{
				switch (lorawanDataReceived->port)
				{
				/* Cayenne default port, can't be changed */
				case mCayenneDefaultPortDownlinkLoRaWAN:
				{

					/* The first byte define the Cayenne port
					 * Each port is assigned to a specific task*/
					switch (lorawanDataReceived->data[0])
					{
					case mCayenneChannelLed2DigitalOutput:

						/* If the data ask to turn on the LED */
						if (lorawanDataReceived->data[2] == 100)
						{
							Led2On();
						}
						else
						{
							Led2Off();
						}
						break;
					default:
						break;
					}
				}
					break;
				}

				/* Destroy tmp buffer allocated by the lorawan controller task */
				vPortFree(lorawanDataReceived);
			}
		}
	}
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
