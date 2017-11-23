
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

bleResult_t ScDbGPS_SetCharacteristicValUTF8s (uint16_t serviceHandle, bleUuid_t* uuid_char, char* strVal);

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


bleResult_t ScDbGPS_SetCharacteristicValUTF8s (uint16_t serviceHandle, bleUuid_t* uuid_char, char* strVal) {

    uint16_t  handle;
    uint8_t data[32];
    int arrayLength = 0;

   	bleResult_t result = GattDb_FindCharValueHandleInService(serviceHandle,
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
