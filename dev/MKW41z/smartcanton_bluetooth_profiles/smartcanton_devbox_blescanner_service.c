/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "smartcanton_devbox_blescanner_interface.h"
#include "gatt_uuid_decl_x.h"
#include "stdio.h"
#include "string_utils.h"

/************************************************************************************
 *************************************************************************************
 * Private constants & macros
 *************************************************************************************
 ************************************************************************************/

/************************************************************************************
 *************************************************************************************
 * Private type definitions
 *************************************************************************************
 ************************************************************************************/

/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/

/*! Smart Canton Dev Box GPS Service - Subscribed Client*/
static deviceId_t mScDbBleScanner_SubscribedClientId;

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/

bleResult_t ScDbBleScanner_SendInstantValueNotifications(bleUuid_t* pCharacteristicUuid,
		uint16_t *serviceHandle, uint8_t* value, uint16_t valueLength);

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/
bleResult_t ScDbBleScanner_Start(scdbBleScannerConfig_t *pServiceConfig)
{
	bleResult_t result;

	/* Clear subscribed client ID (if any) */
	mScDbBleScanner_SubscribedClientId = gInvalidDeviceId_c;

	ScDbBleScanner_RecordValueMeasureInterval(pServiceConfig->serviceHandle,
			&pServiceConfig->bleScanInterval);

	uint16_t defaultDeviceScanned = 0;
	ScDbBleScanner_RecordNotificationBleDevicesScanned(pServiceConfig->serviceHandle,
			&defaultDeviceScanned);

	result = gBleSuccess_c;
	return result;
}

bleResult_t ScDbBleScanner_Stop(scdbBleScannerConfig_t *pServiceConfig)
{
	/* Unsubscribe current client */
	return ScDbBleScanner_Unsubscribe();
}

bleResult_t ScDbBleScanner_Subscribe(deviceId_t deviceId)
{
	/* Subscribe a new client to this service */
	mScDbBleScanner_SubscribedClientId = deviceId;

	return gBleSuccess_c;
}

bleResult_t ScDbBleScanner_Unsubscribe()
{
	/* Clear current subscribed client ID */
	mScDbBleScanner_SubscribedClientId = gInvalidDeviceId_c;
	return gBleSuccess_c;
}

bleResult_t ScDbBleScanner_RecordNotificationBleDevicesScanned(uint16_t serviceHandle, uint16_t* ble_devices_scanned)
{
	uint16_t handle;
	bleResult_t result;

	result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, (bleUuid_t*)
			uuid_ble_devices_scanned, &handle);

	if (result != gBleSuccess_c)
		return result;

	result = GattDb_WriteAttribute(handle, sizeof(uint16_t), (uint8_t*) ble_devices_scanned);
	if (result != gBleSuccess_c)
			return result;

	uint16_t cccdHandle;
	bool_t isNotifActive;

	/* Get handle of the handle CCCD */
	if (GattDb_FindCccdHandleForCharValueHandle(handle, &cccdHandle)
			!= gBleSuccess_c)
		return result;

	if (mScDbBleScanner_SubscribedClientId == gInvalidDeviceId_c)
		return gBleUnexpectedError_c;

	if (gBleSuccess_c == Gap_CheckNotificationStatus(mScDbBleScanner_SubscribedClientId,
			cccdHandle, &isNotifActive) && TRUE == isNotifActive)
	{
		GattServer_SendNotification(mScDbBleScanner_SubscribedClientId, handle);
	}

	return gBleSuccess_c;
}

bleResult_t ScDbBleScanner_RecordValueMeasureInterval(uint16_t serviceHandle, uint16_t* interval)
{
	uint16_t handle;
	bleResult_t result;

	result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, (bleUuid_t*)
			uuid_ble_scan_window, &handle);

	if (result != gBleSuccess_c)
		return result;

	/* Update characteristic value and send notification */
	result = GattDb_WriteAttribute(handle, sizeof(uint16_t), (uint8_t*) interval);
	return result;
}

/************************************************************************************
 *************************************************************************************
 * Private functions
 *************************************************************************************
 ************************************************************************************/

bleResult_t ScDbBleScanner_SendInstantValueNotifications(bleUuid_t* pCharacteristicUuid,
		uint16_t *serviceHandle, uint8_t* value, uint16_t valueLength)
{
	uint16_t cccdHandle;
	bool_t isNotifActive;
	uint16_t handle;
	bleResult_t result;

	result = GattDb_FindCharValueHandleInService(*serviceHandle, gBleUuidType128_c,
			(bleUuid_t*) pCharacteristicUuid, &handle);

	if (result != gBleSuccess_c)
		return result;

	/* Get handle of the handle CCCD */
	result = GattDb_FindCccdHandleForCharValueHandle(handle, &cccdHandle);

	if (result != gBleSuccess_c)
		return result;

	if (mScDbBleScanner_SubscribedClientId == gInvalidDeviceId_c)
		return gBleUnexpectedError_c;

	if (gBleSuccess_c == Gap_CheckNotificationStatus(mScDbBleScanner_SubscribedClientId,
			cccdHandle, &isNotifActive) && TRUE == isNotifActive)
	{
		result = GattServer_SendInstantValueNotification(mScDbBleScanner_SubscribedClientId,
				handle, valueLength, value);
	}

	return result;
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
