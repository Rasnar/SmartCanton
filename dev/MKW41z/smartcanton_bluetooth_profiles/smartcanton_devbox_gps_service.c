
#include <smartcanton_devbox_gps_interface.h>

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
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

/*! Potentiometer Service - Subscribed Client*/
static deviceId_t mScDbGPS_SubscribedClientId;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

static bleResult_t ScDbGPS_SetCharacteristicValUTF8s (uint16_t *serviceHandle, bleUuid_t* uuid_char, char* strVal);
static void ScDbGPS_SendNotifications(uint16_t *charHandle);
static inline bleResult_t ScDbGPS_UpdateGPSFloatCharacteristic(uint16_t *handle,float *data);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
bleResult_t ScDbGPS_Start (scdbGPSConfig_t *pServiceConfig)
{
    bleResult_t result;

    /* Clear subscribed client ID (if any) */
    mScDbGPS_SubscribedClientId = gInvalidDeviceId_c;
    
    result = ScDbGPS_UpdateAllGattTable(pServiceConfig);

    return result;
}

bleResult_t ScDbGPS_Stop (scdbGPSConfig_t *pServiceConfig)
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

bleResult_t ScDbGPS_RecordGPSLatitude (uint16_t serviceHandle, float latitude)
{
    uint16_t  handle;
    bleResult_t result;

    /* Get handle of Heart Rate Measurement characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)uuid_gps_latitude, &handle);

    if (result != gBleSuccess_c)
        return result;

    /* Update characteristic value and send notification */
    if (!ScDbGPS_UpdateGPSFloatCharacteristic(&handle, &latitude))
    {
    	ScDbGPS_SendNotifications(&handle);
    }
    return gBleSuccess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

bleResult_t ScDbGPS_UpdateAllGattTable (scdbGPSConfig_t *pServiceConfig) {

	bleResult_t result;
	uint16_t  handle;

//	/* AppEUI*/
//	result = ScDbGPS_SetCharacteristicValUTF8s(pServiceConfig->serviceHandle,
//			(bleUuid_t*)&uuid_lora_app_eui,
//			pServiceConfig->loRaCtrlConfig->appEui);
//	if (result != gBleSuccess_c)
//		return result;


	return gBleSuccess_c;
}

static inline bleResult_t ScDbGPS_UpdateGPSFloatCharacteristic
(
    uint16_t *handle,
    float *data
)
{
	union {
		float floatType;
		uint8_t floatArray[4];
	} dataConverter;

	dataConverter.floatType = *data;

	return GattDb_WriteAttribute(*handle, 4, dataConverter.floatArray);
}

static void ScDbGPS_SendNotifications
(
	uint16_t	*charHandle
)
{
    uint16_t  cccdHandle;
    bool_t isNotifActive;

    /* Get handle of the handle CCCD */
    if (GattDb_FindCccdHandleForCharValueHandle(*charHandle, &cccdHandle) != gBleSuccess_c)
        return;

    if (mScDbGPS_SubscribedClientId == gInvalidDeviceId_c)
      return;

    if (gBleSuccess_c == Gap_CheckIndicationStatus
        (mScDbGPS_SubscribedClientId, cccdHandle, &isNotifActive) &&
        TRUE == isNotifActive)
    {
        GattServer_SendIndication(mScDbGPS_SubscribedClientId, *charHandle);
    }
}


static bleResult_t ScDbGPS_SetCharacteristicValUTF8s (uint16_t *serviceHandle, bleUuid_t* uuid_char, char* strVal) {

    uint16_t  handle;
    uint8_t data[32];
    int arrayLength = 0;

   	bleResult_t result = GattDb_FindCharValueHandleInService(*serviceHandle,
   		gBleUuidType128_c, uuid_char, &handle);

   	if (result != gBleSuccess_c)
   		return result;

   	arrayLength = convertHexStringToBytesArray(strVal, data);

   	/* Update characteristic value */
   	result = GattDb_WriteAttribute(handle, arrayLength, data);

   	if (result != gBleSuccess_c)
   		return result;

   	return gBleSuccess_c;
}


/*! *********************************************************************************
 * @}
 ********************************************************************************** */
