/**
 * @file    smartcanton_devbox_gps_interface.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the interface to manage the gps service of the SmartCanton
 * 			DevBox. Contain function to start, stop, subscribe and unsubscribe from 
 *  		the service. Data can be written to the ble database or/and generate 
 * 			notifications.
 */

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "smartcanton_devbox_gps_interface.h"
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
static deviceId_t mScDbGPS_SubscribedClientId;

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/
static void ScDbGPS_SendNotifications(uint16_t *charHandle);

static inline bleResult_t ScDbGPS_UpdateGPSCharacteristicUTF8s(uint16_t *handle, char *data);

static bleResult_t ScDbGPS_RecordNotificationCharacteristicFloatToString(uint16_t serviceHandle, bleUuid_t* uuid_char,
		float* floatVal);

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/
bleResult_t ScDbGPS_Start(scdbGPSConfig_t *pServiceConfig)
{
	bleResult_t result;

	/* Clear subscribed client ID (if any) */
	mScDbGPS_SubscribedClientId = gInvalidDeviceId_c;

	//result = ScDbGPS_UpdateAllGattTable(pServiceConfig);

	result = gBleSuccess_c;
	return result;
}

bleResult_t ScDbGPS_Stop(scdbGPSConfig_t *pServiceConfig)
{
	/* Unsubscribe current client */
	return ScDbGPS_Unsubscribe();
}

bleResult_t ScDbGPS_Subscribe(deviceId_t deviceId)
{
	/* Subscribe a new client to this service */
	mScDbGPS_SubscribedClientId = deviceId;

	return gBleSuccess_c;
}

bleResult_t ScDbGPS_Unsubscribe()
{
	/* Clear current subscribed client ID */
	mScDbGPS_SubscribedClientId = gInvalidDeviceId_c;
	return gBleSuccess_c;
}

bleResult_t ScDbGPS_RecordNotificationGPSPosition(uint16_t serviceHandle, float* latitude, float* longitude)
{

	uint16_t handle;
	bleResult_t result;
	char strFloat[32];

	/* Get handle of GPS latitude characteristic */
	result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c,
			(bleUuid_t*) uuid_gps_position, &handle);

	if (result != gBleSuccess_c)
		return result;

	snprintf(strFloat, sizeof(strFloat), "%f,%f", *latitude, *longitude);

	/* Update characteristic value and send notification */
	if (!ScDbGPS_UpdateGPSCharacteristicUTF8s(&handle, strFloat))
	{
		ScDbGPS_SendNotifications(&handle);
	}
	return gBleSuccess_c;
}

bleResult_t ScDbGPS_RecordNotificationGPSSpeed(uint16_t serviceHandle, float* speed)
{
	return ScDbGPS_RecordNotificationCharacteristicFloatToString(serviceHandle, (bleUuid_t*) uuid_gps_speed, speed);
}

bleResult_t ScDbGPS_RecordNotificationGPSCourse(uint16_t serviceHandle, float* course)
{
	return ScDbGPS_RecordNotificationCharacteristicFloatToString(serviceHandle, (bleUuid_t*) uuid_gps_course, course);
}

bleResult_t ScDbGPS_RecordNotificationGPSTime(uint16_t serviceHandle, struct minmea_time *time)
{
	uint16_t handle;
	bleResult_t result;
	char strFloat[32];

	/* Get handle of GPS latitude characteristic */
	result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, (bleUuid_t*) uuid_gps_time,
			&handle);

	if (result != gBleSuccess_c)
		return result;

	snprintf(strFloat, sizeof(strFloat), "%02d:%02d:%02d", time->hours, time->minutes, time->seconds);

	/* Update characteristic value and send notification */
	if (!ScDbGPS_UpdateGPSCharacteristicUTF8s(&handle, strFloat))
	{
		ScDbGPS_SendNotifications(&handle);
	}
	return gBleSuccess_c;
}

bleResult_t ScDbGPS_RecordNotificationGPSDate(uint16_t serviceHandle, struct minmea_date *date)
{
	uint16_t handle;
	bleResult_t result;
	char strFloat[32];

	/* Get handle of GPS latitude characteristic */
	result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, (bleUuid_t*) uuid_gps_date,
			&handle);

	if (result != gBleSuccess_c)
		return result;

	snprintf(strFloat, sizeof(strFloat), "%02d/%02d/%02d", date->day, date->month, date->year);

	/* Update characteristic value and send notification */
	if (!ScDbGPS_UpdateGPSCharacteristicUTF8s(&handle, strFloat))
	{
		ScDbGPS_SendNotifications(&handle);
	}
	return gBleSuccess_c;
}

bleResult_t ScDbGPS_RecordNotificationFrameRmcAll(uint16_t serviceHandle, struct minmea_sentence_rmc *rmcFrame){
	bleResult_t result;
	float tmp_float1, tmp_float2;

	tmp_float1 = minmea_tocoord(&rmcFrame->latitude);
	tmp_float2 = minmea_tocoord(&rmcFrame->longitude);
	result = ScDbGPS_RecordNotificationGPSPosition(serviceHandle, &tmp_float1, &tmp_float2);
	if(result != gBleSuccess_c)
		return result;

	tmp_float1 = minmea_tofloat(&rmcFrame->course);
	if (isnanf(tmp_float1))
	{
		tmp_float1 = 0.0;
	}
	result = ScDbGPS_RecordNotificationGPSCourse(serviceHandle, &tmp_float1);
	if(result != gBleSuccess_c)
		return result;

	tmp_float1 = minmea_tofloat(&rmcFrame->speed);
	result = ScDbGPS_RecordNotificationGPSSpeed(serviceHandle, &tmp_float1);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbGPS_RecordNotificationGPSTime(serviceHandle, &rmcFrame->time);
	if(result != gBleSuccess_c)
		return result;

	result = ScDbGPS_RecordNotificationGPSDate(serviceHandle, &rmcFrame->date);
	return result;
}

/************************************************************************************
 *************************************************************************************
 * Private functions
 *************************************************************************************
 ************************************************************************************/

static bleResult_t ScDbGPS_RecordNotificationCharacteristicFloatToString(uint16_t serviceHandle, bleUuid_t* uuid_char,
		float *floatVal)
{
	uint16_t handle;
	bleResult_t result;
	char strFloat[16];

	/* Get handle of GPS latitude characteristic */
	result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, (bleUuid_t*) uuid_char,
			&handle);

	if (result != gBleSuccess_c)
		return result;

	snprintf(strFloat, sizeof(strFloat), "%f", *floatVal);

	/* Update characteristic value and send notification */
	if (!ScDbGPS_UpdateGPSCharacteristicUTF8s(&handle, strFloat))
	{
		ScDbGPS_SendNotifications(&handle);
	}
	return gBleSuccess_c;
}

static inline bleResult_t ScDbGPS_UpdateGPSCharacteristicUTF8s(uint16_t *handle, char *data)
{
	return GattDb_WriteAttribute(*handle, strlen(data), (uint8_t*) data);
}

static void ScDbGPS_SendNotifications(uint16_t *charHandle)
{
	uint16_t cccdHandle;
	bool_t isNotifActive;

	/* Get handle of the handle CCCD */
	if (GattDb_FindCccdHandleForCharValueHandle(*charHandle, &cccdHandle) != gBleSuccess_c)
		return;

	if (mScDbGPS_SubscribedClientId == gInvalidDeviceId_c)
		return;

	if (gBleSuccess_c == Gap_CheckNotificationStatus(mScDbGPS_SubscribedClientId, cccdHandle, &isNotifActive)
			&& TRUE == isNotifActive)
	{
		GattServer_SendNotification(mScDbGPS_SubscribedClientId, *charHandle);
	}
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
