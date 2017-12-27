

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "smartcanton_devbox_bme680_interface.h"
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
static deviceId_t mScDbBme680_SubscribedClientId;

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/

bleResult_t ScDbBme680_SendInstantValueNotifications(bleUuid_t* pCharacteristicUuid,
		uint16_t *serviceHandle, float* value);

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/
bleResult_t ScDbBme680_Start(scdbBme680Config_t *pServiceConfig)
{
	bleResult_t result;

	/* Clear subscribed client ID (if any) */
	mScDbBme680_SubscribedClientId = gInvalidDeviceId_c;

	//result = ScDbGPS_UpdateAllGattTable(pServiceConfig);

	result = gBleSuccess_c;
	return result;
}

bleResult_t ScDbBme680_Stop(scdbBme680Config_t *pServiceConfig)
{
	/* Unsubscribe current client */
	return ScDbBme680_Unsubscribe();
}

bleResult_t ScDbBme680_Subscribe(deviceId_t deviceId)
{
	/* Subscribe a new client to this service */
	mScDbBme680_SubscribedClientId = deviceId;

	return gBleSuccess_c;
}

bleResult_t ScDbBme680_Unsubscribe()
{
	/* Clear current subscribed client ID */
	mScDbBme680_SubscribedClientId = gInvalidDeviceId_c;
	return gBleSuccess_c;
}

bleResult_t ScDbBme680_InstantValueAll(uint16_t serviceHandle, bme680Data_t* bme680Data){
	bleResult_t result;

	result = ScDbBme680_InstantValueIaq(serviceHandle, &bme680Data->iaq);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbBme680_InstantValueIaqAccuracy(serviceHandle, &bme680Data->iaq_accuracy);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbBme680_InstantValueTemperature(serviceHandle, &bme680Data->temperature);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbBme680_InstanHumidity(serviceHandle, &bme680Data->humidity);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbBme680_InstantValuePressure(serviceHandle, &bme680Data->pressure);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbBme680_InstantValueRawHumidity(serviceHandle, &bme680Data->raw_humidity);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbBme680_InstantValueRawTemperature(serviceHandle, &bme680Data->raw_temperature);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbBme680_InstantValueRawGas(serviceHandle, &bme680Data->gas);

	return result;
}

bleResult_t ScDbBme680_InstantValueIaq(uint16_t serviceHandle, float* iaq){

	return ScDbBme680_SendInstantValueNotifications((bleUuid_t*) uuid_bme680_iaq,
			&serviceHandle, iaq);
}

bleResult_t ScDbBme680_InstantValueIaqAccuracy(uint16_t serviceHandle, uint8_t* iaq_accuracy){

	uint16_t cccdHandle;
	bool_t isNotifActive;
	uint16_t handle;
	bleResult_t result;

	/* Get handle of GPS latitude characteristic */
	result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c,
			(bleUuid_t*) uuid_bme680_iaq_accuracy, &handle);

	if (result != gBleSuccess_c)
		return result;

	/* Get handle of the handle CCCD */
	result = GattDb_FindCccdHandleForCharValueHandle(handle, &cccdHandle);

	if (result != gBleSuccess_c)
		return result;

	if (mScDbBme680_SubscribedClientId == gInvalidDeviceId_c)
		return gBleUnexpectedError_c;

	if (gBleSuccess_c == Gap_CheckNotificationStatus(mScDbBme680_SubscribedClientId, cccdHandle,
			&isNotifActive)&& TRUE == isNotifActive)
	{
		result = GattServer_SendInstantValueNotification(mScDbBme680_SubscribedClientId, handle,
				sizeof(uint8_t), iaq_accuracy);
	}

	return result;
}

bleResult_t ScDbBme680_InstantValueTemperature(uint16_t serviceHandle, float* temperature){

	return ScDbBme680_SendInstantValueNotifications((bleUuid_t*) uuid_bme680_temperature,
			&serviceHandle, temperature);
}

bleResult_t ScDbBme680_InstanHumidity(uint16_t serviceHandle, float* humidity){

	return ScDbBme680_SendInstantValueNotifications((bleUuid_t*) uuid_bme680_humidity,
			&serviceHandle, humidity);
}

bleResult_t ScDbBme680_InstantValuePressure(uint16_t serviceHandle, float* pressure){

	return ScDbBme680_SendInstantValueNotifications((bleUuid_t*) uuid_bme680_pressure,
			&serviceHandle, pressure);
}

bleResult_t ScDbBme680_InstantValueRawHumidity(uint16_t serviceHandle, float* raw_humidity){

	return ScDbBme680_SendInstantValueNotifications((bleUuid_t*) uuid_bme680_raw_humidity,
			&serviceHandle, raw_humidity);
}

bleResult_t ScDbBme680_InstantValueRawGas(uint16_t serviceHandle, float* raw_gas){

	return ScDbBme680_SendInstantValueNotifications((bleUuid_t*) uuid_bme680_raw_gas,
			&serviceHandle, raw_gas);
}

bleResult_t ScDbBme680_InstantValueRawTemperature(uint16_t serviceHandle, float* raw_temperature){

	return ScDbBme680_SendInstantValueNotifications((bleUuid_t*) uuid_bme680_raw_temperature,
			&serviceHandle, raw_temperature);
}

/************************************************************************************
 *************************************************************************************
 * Private functions
 *************************************************************************************
 ************************************************************************************/

bleResult_t ScDbBme680_SendInstantValueNotifications(bleUuid_t* pCharacteristicUuid,
		uint16_t *serviceHandle, float* value)
{
	uint16_t cccdHandle;
	bool_t isNotifActive;
	uint16_t handle;
	bleResult_t result;

	/* Get handle of GPS latitude characteristic */
	result = GattDb_FindCharValueHandleInService(*serviceHandle, gBleUuidType128_c,
			(bleUuid_t*) pCharacteristicUuid, &handle);

	if (result != gBleSuccess_c)
		return result;

	/* Get handle of the handle CCCD */
	result = GattDb_FindCccdHandleForCharValueHandle(handle, &cccdHandle);

	if (result != gBleSuccess_c)
		return result;

	if (mScDbBme680_SubscribedClientId == gInvalidDeviceId_c)
		return gBleUnexpectedError_c;

	if (gBleSuccess_c == Gap_CheckNotificationStatus(mScDbBme680_SubscribedClientId,
			cccdHandle, &isNotifActive) && TRUE == isNotifActive)
	{
		result = GattServer_SendInstantValueNotification(mScDbBme680_SubscribedClientId,
				handle, sizeof(float), (uint8_t*)value);
	}

	return result;
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
