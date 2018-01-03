/**
 * @file    dev_box_app_task.c
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
#include "smartcanton_devbox_blescanner_interface.h"
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
}gapScannedDevices_t;

static gapScannedDevices_t     mScannedDevices[mBleAppMaxScannedDevicesCount_c];
static uint16_t                mScannedDevicesCount;

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
#define BLE_SCANNER_MINIMUM_SCAN_INTERVAL_SEC		(10)
#define BLE_SCANNER_MAXIMUM_SCAN_INTERVAL_SEC		(600)
#define LORAWAN_MINIMUM_PACKET_INTERVAL_MIN			(1)
#define LORAWAN_MAXIMUM_PACKET_INTERVAL_MIN			(1440) /* 1 day */

/* Battery level BLE report interval in seconds  */
#define mBatteryLevelReportInterval_c   			(30)
/* Every x seconds that the BLE history is wiped to remove hold data  */
#define mBleScannerReportScanInterval_default_c   	(BLE_SCANNER_MINIMUM_SCAN_INTERVAL_SEC)
/* LoRaWAN new data interval in minutes  */
#define mLoRaNewDataReportInterval_default_c   		(1)

/************************************************************
 * Cayenne application configuration
 ************************************************************/
/* This LoRaWAN downlink port can't be changed with Cayenne */
#define mCayenneDefaultPortDownlinkLoRaWAN					99

/* Uplink and downlink Cayenne ports */
#define mCayenneChannelEnableGpsDataInLoRaPacket			10
#define mCayenneChannelGps									11

#define mCayenneChannelEnableBno055DataInLoRaPacket			20
#define mCayenneChannelBno055Accelerometer					21
#define mCayenneChannelBno055Gyroscope						22

#define mCayenneChannelEnableBme680DataInLoRaPacket			30
#define mCayenneChannelBme680Temperature					31
#define mCayenneChannelBme680Humidity						32
#define mCayenneChannelBme680Pressure						33
#define mCayenneChannelBme680Iaq							34

#define mCayenneChannelEnableBatteryLevelInLoRaPacket		40
#define mCayenneChannelBatteryLevel							41

#define mCayenneChannelEnableBleScannerDataInLoRaPacket		50
#define mCayenneChannelBleScanner							51

#define mCayenneChannelLed2DigitalOutput					60

#define mCayenneChannelLoRaPacketIntervalOutput				70

/* Index used by the array mCayenneSensorsEnabled */
enum mCayenneSensorsEnabledIndex
{
	EN_GPS, EN_BME680, EN_BNO055, EN_BLE_SCANNER, EN_BATTERY, EN_COUNT
};

/* By default, all sensors are enabled, they can be disabled with LoRa downlink calls */
bool_t mCayenneSensorsEnabled[EN_COUNT] =
{ TRUE, TRUE, TRUE, TRUE, TRUE };

/************************************************
 * 		AltBeacon Format Wanted
 ***********************************************/
static const uint8_t BEACON_MANUFACTURER_ID[] = {0xFF, 0xFF};
static const uint8_t ALTBEACON_CODE[] = {0xBE, 0xAC};
static const uint8_t ALTBEACON_UUID_BYTES [] = {0x00, 0x00, 0x00,
	0x00, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC,
	0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD};
static const uint8_t ALTBEACON_MAJOR[] = {0x11, 0x11};
static const uint8_t ALTBEACON_MINOR[] = {0x22, 0x22};

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

static scdbBleScannerConfig_t scdbBleScannerConfig =
{ service_smartcanton_devbox_ble, mBleScannerReportScanInterval_default_c};


/* Attribute handles that will be notified through the GATT Server callback
 * when a GATT Client attempts to write the attributes values.*/
static uint16_t writeHandles[10] =
{ value_lora_app_eui, value_lora_app_key, value_lora_device_eui, value_lora_confirm_mode,
		value_lora_device_address, value_lora_network_session_key, value_lora_app_session_key,
		value_lora_validate_new_configuration, value_ble_scan_window, value_bno055_measure_interval};

/* Attribute handles that will be notified through the GATT Server callback
 * when a GATT Client attempts to read the attributes values.*/
static uint16_t readHandles[3] =
{ value_lora_network_join_status, value_bno055_measure_interval, value_ble_scan_window };

/* Application specific data*/
static tmrTimerID_t mAdvTimerId;
static tmrTimerID_t mBatteryMeasurementTimerId;
static tmrTimerID_t mLoRaSendNewFrameTimerId;
static tmrTimerID_t mBleScannerTimerId;

osaMsgQId_t gBleScannerNewMessageMeasureQ;

/* indicate the interval beetwen each LoRa packet
 * This value can be modified by a LoRa Downlink request (*/
static uint16_t mLoRaNewDataReportInterval = mLoRaNewDataReportInterval_default_c;

/* indicate the interval beetwen each Bluetooth Scan update
 * This value can be modified by a Bluetooth write */
static uint16_t mBleScannerReportScanInterval = mBleScannerReportScanInterval_default_c;

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
static void BleScannerReportDevicesFoundCallback(void * pParam);

static void BleApp_Advertise(void);

static void BleApp_SetScanParameters();
static void BleApp_ParseScannedDeviceAsAltBeacon(gapScannedDevice_t* pData);
static void BleApp_ScanningCallback (gapScanningEvent_t* pScanningEvent);

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
 * @brief    Initializes application specific functionality before the BLE stack init.
 *
 ********************************************************************************** */
void BleApp_Init(void)
{
	/* Initialize application support for drivers */
	BOARD_InitPins();
	BOARD_InitAdc();
}

/*! *********************************************************************************
 * @brief    Starts the BLE application.
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

	if(!mScanningState.scnOn) {
		mScanningState.scnOn = TRUE;
		//BleApp_SetScanParameters();
		bleResult_t result = Gap_StartScanning(&gAppScanParams, BleApp_ScanningCallback, TRUE);
		result = result;

		/* Timer that will report the main task how many devices have been found while scanning */
		TMR_StartLowPowerTimer(mBleScannerTimerId,
				gTmrLowPowerIntervalMillisTimer_c, TmrSeconds(mBleScannerReportScanInterval),
				BleScannerReportDevicesFoundCallback, NULL);
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

/*! *********************************************************************************
 * @brief        Parse the read scanned ble device to find if they are beacons to be
 * 				 valid AltBeacon with the format specified on the top of this file.
 *
 * @param[in]    gapScannedDevice_t    Pointer to a device scanned
 ********************************************************************************** */
static void BleApp_ParseScannedDeviceAsAltBeacon(gapScannedDevice_t* pData)
{
	/* /!\ This parser is highly basic, please consult the AltBeacon specification to
	 * see the AltBeacon advertisement packet content
	 * https://github.com/AltBeacon/spec
	 */

    uint8_t rslt = 0;

    /* Check if the device has already been registered as a beacon */
	for (int i = 0; i < mScannedDevicesCount; i++)
	{
		if (memcmp(pData->aAddress, mScannedDevices[i].aAddress,
				sizeof(bleAddressType_t)) == 0)
		{
			return; // Quit the function if the beacon has already been discovered before
		}
	}

	if (
	/* The AltBeacon generated by Android are always random because of the
	 * privacity settings */
	pData->addressType == gBleAddrTypeRandom_c
	/* The complete length of an AltBeacon is always 31 (counting the BLE header)*/
	&& (pData->dataLength == 31)
	/* ! Field 4 and 5 should contain the 0xBEAC value to describe an AltBeacon */
	&& (pData->data[4] == ALTBEACON_CODE[0]) && (pData->data[5] == ALTBEACON_CODE[1]))
	{

		rslt += memcmp(pData->data + 2, BEACON_MANUFACTURER_ID, sizeof(BEACON_MANUFACTURER_ID));
		rslt += memcmp(pData->data + 6, ALTBEACON_UUID_BYTES, sizeof(ALTBEACON_UUID_BYTES));
		rslt += memcmp(pData->data + 22, ALTBEACON_MAJOR, sizeof(ALTBEACON_MAJOR));
		rslt += memcmp(pData->data + 24, ALTBEACON_MINOR, sizeof(ALTBEACON_MINOR));

		/* If it's a valid beacon */
		if (rslt == 0)
		{
			/* Prevent the mScannedDevicesCount to be modified,
			 * TOFIX : Should be replaced by a mutex */
			OSA_InterruptDisable();

			/* Temporary store scanned data to use for connection */
			mScannedDevices[mScannedDevicesCount].addrType = pData->addressType;
			FLib_MemCpy(mScannedDevices[mScannedDevicesCount].aAddress, pData->aAddress,
					sizeof(bleDeviceAddress_t));

			mScannedDevicesCount++;

			OSA_InterruptEnable();
		}
	}
}

/*! *********************************************************************************
 * @brief        Handles BLE Scanning callback from host stack.
 *
 * @param[in]    pScanningEvent    Pointer to gapScanningEvent_t.
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

		BleApp_ParseScannedDeviceAsAltBeacon(&pScanningEvent->eventData.scannedDevice);
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
 * @brief        Handles keyboard events.
 *
 * @param[in]    events    Key event structure.
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
 * @brief        Handles BLE generic callback.
 *
 * @param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
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
 * @brief        Configures BLE Stack after initialization. Usually used for
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
	*scdbLoRaServiceConfig.loRaCtrlConfig = lorawan_controller_get_stored_configuration();
	ScDbLoRa_Start(&scdbLoRaServiceConfig);

	ScDbGPS_Start(&scdbGPSServiceConfig);

	ScDbBno055_Start(&scdbBno055ServiceConfig);

	ScDbBme680_Start(&scdbBme680ServiceConfig);

	ScDbBleScanner_Start(&scdbBleScannerConfig);

	/* Allocate application timers */
	mAdvTimerId = TMR_AllocateTimer();
	mBatteryMeasurementTimerId = TMR_AllocateTimer();
	mBleScannerTimerId = TMR_AllocateTimer();

	/* Create application Queue */
	gBleScannerNewMessageMeasureQ = OSA_MsgQCreate(BLE_SCANNER_MEASURE_QUEUE_SIZE);
	if ( NULL == gBleScannerNewMessageMeasureQ)
	{
		panic(0, 0, 0, 0);
	}

#if (cPWR_UsePowerDownMode)    
	PWR_ChangeDeepSleepMode(3); /* MCU=LLS3, LL=IDLE, wakeup on GPIO/LL */
	PWR_AllowDeviceToSleep();
#endif    
}

/*! *********************************************************************************
 * @brief        Configures GAP Advertise parameters. Advertise will satrt after
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
 * @brief        Handles BLE Advertising callback from host stack.
 *
 * @param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
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
 * @brief        Handles BLE Connection callback from host stack.
 *
 * @param[in]    peerDeviceId        Peer device ID.
 * @param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
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
		ScDbBleScanner_Subscribe(peerDeviceId);

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
		ScDbBleScanner_Unsubscribe();

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
 * @brief        Handles GATT server callback from host stack.
 *
 * @param[in]    deviceId        Peer device ID.
 * @param[in]    pServerEvent    Pointer to gattServerEvent_t.
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

		if (handle == value_ble_scan_window)
		{
			uint16_t intervalWanted = pServerEvent->eventData.attributeWrittenEvent.aValue[0]
					| pServerEvent->eventData.attributeWrittenEvent.aValue[1] << 8;

			if ((intervalWanted >= BLE_SCANNER_MINIMUM_SCAN_INTERVAL_SEC)
					&& (intervalWanted <= BLE_SCANNER_MAXIMUM_SCAN_INTERVAL_SEC))
			{
				mBleScannerReportScanInterval = intervalWanted;

				if (mScanningState.scnOn)
				{
					TMR_StopTimer(mBleScannerTimerId);
				}

				/* Timer that will report the main task how many devices have been found while scanning */
				TMR_StartLowPowerTimer(mBleScannerTimerId,
				gTmrLowPowerIntervalMillisTimer_c, TmrSeconds(mBleScannerReportScanInterval),
						BleScannerReportDevicesFoundCallback, NULL);

				status = gBleSuccess_c;
			}
			else
			{
				status = gBleInvalidParameter_c;
			}
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

		if (handle == value_ble_scan_window) {
			ScDbBleScanner_RecordValueMeasureInterval(service_smartcanton_devbox_ble,
					&mBleScannerReportScanInterval);
		}

		GattServer_SendAttributeReadStatus(deviceId, handle, status);
	}
	break;
	default:
		break;
	}
}

/*! *********************************************************************************
 * @brief        Handles advertising timer callback.
 *
 * @param[in]    pParam        Calback parameters.
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
 * @brief        Handles battery measurement timer callback.
 *
 * @param[in]    pParam        Callback parameters.
 ********************************************************************************** */
static void BatteryMeasurementTimerCallback(void * pParam)
{
	basServiceConfig.batteryLevel = BOARD_GetBatteryLevel();
	Bas_RecordBatteryMeasurement(basServiceConfig.serviceHandle, basServiceConfig.batteryLevel);
}

/*! *********************************************************************************
 * @brief        Handle the report of how many BLE beacons have been found
 *
 * @param[in]    pParam        Calback parameters.
 ********************************************************************************** */
static void BleScannerReportDevicesFoundCallback(void * pParam)
{
	bleScannerData_t* bleScannerData = pvPortMalloc(sizeof(bleScannerData_t));

	bleScannerData->bleBeaconsFound = mScannedDevicesCount;

	if (Gap_StopScanning() == gBleSuccess_c)
	{
		/* Restart a mew scanner */
		Gap_StartScanning(&gAppScanParams, BleApp_ScanningCallback, TRUE);
	}

	OSA_InterruptDisable();

	/* Invalidate already found devices */
	mScannedDevicesCount = 0;

	OSA_InterruptEnable();

	if (OSA_MsgQPut(gBleScannerNewMessageMeasureQ, &bleScannerData) == osaStatus_Success)
	{
		/* Only notify main task if the message can be added successfully to the Queue */
		OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewBleScannerMeasureAvailable_c);
	}
	else
	{
		/* Otherwise, free the reserved memory */
		vPortFree(bleScannerData);
	}
}

/*! *********************************************************************************
 * @brief        Notify the main task that it's time to generate a new LoRaWAN packet
 *
 * @param[in]    pParam        Callback parameters.
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

	/* Last data received from the ble scanner */
	bleScannerData_t bleScannerData;

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
				*scdbLoRaServiceConfig.loRaCtrlConfig = lorawan_controller_get_stored_configuration();
				ScDbLoRa_UpdateAllGattTable(&scdbLoRaServiceConfig);

				/* If the timer is already running, stop if first before restarting it */
				if(TMR_IsTimerActive(mLoRaSendNewFrameTimerId))
					TMR_StopTimer(mLoRaSendNewFrameTimerId);

				/* Start timer to notify application every mLoRaNewDataReportInterval_c
				 * to send a new LoRaWAN frame to the network with the last values available */
				TMR_StartLowPowerTimer(mLoRaSendNewFrameTimerId,
						gTmrLowPowerIntervalMillisTimer_c,
						TmrMinutes(mLoRaNewDataReportInterval),
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
			while (OSA_MsgQGet(gBme680NewMessageMeasureQ, &bme680Data_tmp, 0)
					== osaStatus_Success)
			{
				/* Store value in local in case we want to use it later on */
				FLib_MemCpy(&bme680Data, bme680Data_tmp, sizeof(bme680Data));

				/* Destroy tmp buffer allocated by the bme680_task */
				vPortFree(bme680Data_tmp);

				/* Send data as notification to connected BLE peer if there is one */
				ScDbBme680_InstantValueNotificationAll(service_smartcanton_devbox_bme680,
						&bme680Data);
			}
		}

		/* Event received when a new measure to the BNO055 has been done */
		if (event & gDevBoxTaskEvtNewBno055MeasureAvailable_c)
		{
			/* Last data received from the BNO055 */
			bno055Data_t* bno055Data_tmp;

			/* Retrieve data pointer */
			while (OSA_MsgQGet(gBno055NewMessageMeasureQ, &bno055Data_tmp, 0)
					== osaStatus_Success)
			{
				/* Store value in local in case we want to use it later on */
				FLib_MemCpy(&bno055Data, bno055Data_tmp, sizeof(bno055Data));

				/* Destroy tmp buffer allocated by the bno055_task */
				vPortFree(bno055Data_tmp);

				/* Send data as notification to connected BLE peer if there is one */
				ScDbBno055_InstantValueNotificationAll(service_smartcanton_devbox_bno055,
						&bno055Data);
			}
		}

		/* Event received when a new measure from the Ble Scanner is ready to be collected */
		if (event & gDevBoxTaskEvtNewBleScannerMeasureAvailable_c)
		{
			/* Last data received from the BNO055 */
			bleScannerData_t* bleScannerData_tmp;

			/* Retrieve data pointer */
			while (OSA_MsgQGet(gBleScannerNewMessageMeasureQ, &bleScannerData_tmp, 0)
					== osaStatus_Success)
			{
				/* Store value in local in case we want to use it later on */
				FLib_MemCpy(&bleScannerData, bleScannerData_tmp, sizeof(bleScannerData_t));

				/* Destroy tmp buffer allocated by the bno055_task */
				vPortFree(bleScannerData_tmp);

				/* Send data as notification to connected BLE peer if there is one */
				ScDbBleScanner_RecordNotificationBleDevicesScanned(service_smartcanton_devbox_ble,
						&bleScannerData.bleBeaconsFound);
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

				/* Send data only if new values since last transmission and if the sensor is enabled */
				if (gpsData.frame_rmc.valid && mCayenneSensorsEnabled[EN_GPS])
				{
					cayenneLPPaddGPS(mCayenneChannelGps,
							minmea_tocoord(&gpsData.frame_rmc.latitude), // Latitude
							minmea_tocoord(&gpsData.frame_rmc.longitude), // Longitude
							0.0); // Altitude (not supported with RMC parsing)

					gpsData.frame_rmc = EmptyframeRmcGps; // Invalidate the local data for the next msg
				}

				/* Send data only if new values since last transmission and if the sensor is enabled */
				if((!((bno055Data.accel_xyz.x == 0.0) &&
						(bno055Data.accel_xyz.y == 0.0) &&
						(bno055Data.accel_xyz.z == 0.0) &&
						(bno055Data.gyro_xyz.x == 0.0) &&
						(bno055Data.gyro_xyz.y == 0.0) &&
						(bno055Data.gyro_xyz.x == 0.0)))
						&& mCayenneSensorsEnabled[EN_BNO055])
				{
					cayenneLPPaddAccelerometer(mCayenneChannelBno055Accelerometer,
							bno055Data.accel_xyz.x / 1024.0,
							bno055Data.accel_xyz.y / 1024.0,
							bno055Data.accel_xyz.z / 1024.0);

					cayenneLPPaddGyrometer(mCayenneChannelBno055Gyroscope,
							bno055Data.gyro_xyz.x,
							bno055Data.gyro_xyz.y,
							bno055Data.gyro_xyz.z);
					bno055Data = EmptyBno055; // Invalidate the local data for the next msg
				}

				/* Send data only if new values since last transmission and if the sensor is enabled */
				if((!((bme680Data.temperature == 0.0) &&
						(bme680Data.humidity == 0.0) &&
						(bme680Data.pressure == 0.0)))
						&& mCayenneSensorsEnabled[EN_BME680])
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

				/* Only send the battery state if the sensor is enabled */
				if(mCayenneSensorsEnabled[EN_BATTERY])
				{
					/* Read the current battery level */
					cayenneLPPaddAnalogInput(mCayenneChannelBatteryLevel,
					(float)BOARD_GetBatteryLevel());
				}

				/* Only send the battery state if the sensor is enabled */
				if(mCayenneSensorsEnabled[EN_BLE_SCANNER])
				{
					/* Read the current battery level */
					cayenneLPPaddAnalogInput(mCayenneChannelBleScanner,
								(float)mScannedDevicesCount);
				}

				/* Read the current LED2 state and send it */
				cayenneLPPaddDigitalOutput(mCayenneChannelLed2DigitalOutput,
						!GpioReadOutputPin(&ledPins[1]));

				/* Offer the possibility to control the interval beetwen each LoRa packets */
				cayenneLPPaddAnalogOutput(mCayenneChannelLoRaPacketIntervalOutput,
										mLoRaNewDataReportInterval);

				/* Offer the possibility to enable the data that the user want to receive */
				cayenneLPPaddDigitalOutput(mCayenneChannelEnableGpsDataInLoRaPacket,
										mCayenneSensorsEnabled[EN_GPS]);
				cayenneLPPaddDigitalOutput(mCayenneChannelEnableBme680DataInLoRaPacket,
										mCayenneSensorsEnabled[EN_BME680]);
				cayenneLPPaddDigitalOutput(mCayenneChannelEnableBno055DataInLoRaPacket,
										mCayenneSensorsEnabled[EN_BNO055]);
				cayenneLPPaddDigitalOutput(mCayenneChannelEnableBleScannerDataInLoRaPacket,
										mCayenneSensorsEnabled[EN_BLE_SCANNER]);
				cayenneLPPaddDigitalOutput(mCayenneChannelEnableBatteryLevelInLoRaPacket,
										mCayenneSensorsEnabled[EN_BATTERY]);

				/* Allocate data on the HEAP to send it to the LoRaWAN task */
				lorawanControllerData = pvPortMalloc(sizeof(lorawanControllerDataToSend_t));
				lorawanControllerData->dataLength = cayenneLPPgetSize();

				/* Copy data formatted previously inside the Cayenne functions */
				FLib_MemCpy(lorawanControllerData->data, cayenneLPPgetBuffer(),
						lorawanControllerData->dataLength);

				if (OSA_MsgQPut(gLorawanCtrlSendNewMessageQ, &lorawanControllerData) == osaStatus_Success)
				{
					/* Notify the LoRaWAN controller to read the pending data */
					OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);
				}
				else
				{
					vPortFree(lorawanControllerData);
					/* Notify the LoRaWAN controller to read the pending data */
					OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);
				}
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

					case mCayenneChannelEnableGpsDataInLoRaPacket:
					{
						mCayenneSensorsEnabled[EN_GPS] = lorawanDataReceived->data[2]/100;
					}
						break;

					case mCayenneChannelEnableBno055DataInLoRaPacket:
					{
						mCayenneSensorsEnabled[EN_BNO055] = lorawanDataReceived->data[2]/100;
					}
						break;

					case mCayenneChannelEnableBme680DataInLoRaPacket:
					{
						mCayenneSensorsEnabled[EN_BME680] = lorawanDataReceived->data[2]/100;
					}
						break;

					case mCayenneChannelEnableBatteryLevelInLoRaPacket:
					{
						mCayenneSensorsEnabled[EN_BATTERY] = lorawanDataReceived->data[2]/100;
					}
						break;

					case mCayenneChannelEnableBleScannerDataInLoRaPacket:
					{
						mCayenneSensorsEnabled[EN_BLE_SCANNER] = lorawanDataReceived->data[2]/100;
					}
						break;

					case mCayenneChannelLed2DigitalOutput:
					{
						/* If the data ask to turn on the LED */
						if (lorawanDataReceived->data[2] == 100)
						{
							Led2On();
						}
						else
						{
							Led2Off();
						}
					}
						break;

					case mCayenneChannelLoRaPacketIntervalOutput:
					{
						uint16_t interval = lorawanDataReceived->data[1] << 8 | lorawanDataReceived->data[2];
						interval = interval / 100; // Convert fixed floating point value
						if ((interval >= LORAWAN_MINIMUM_PACKET_INTERVAL_MIN)
								&& (interval <= LORAWAN_MAXIMUM_PACKET_INTERVAL_MIN))
						{
							mLoRaNewDataReportInterval = interval;

							TMR_StopTimer(mLoRaSendNewFrameTimerId);

							/* Start timer to notify application every mLoRaNewDataReportInterval
							 * to send a new LoRaWAN frame to the network with the last values available */
							TMR_StartLowPowerTimer(mLoRaSendNewFrameTimerId,
									gTmrLowPowerIntervalMillisTimer_c,
									TmrMinutes(mLoRaNewDataReportInterval),
									LoRaSendNewDataTimerCallback, NULL);
						}
					}
						break;
					default:
						break;
					}
				}
					break;
				}

				/* Destroy tmp buffer that was allocated by the lorawan controller task */
				vPortFree(lorawanDataReceived);
			}
		}
	}
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
