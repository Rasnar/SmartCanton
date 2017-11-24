
#include <smartcanton_devbox_lora_interface.h>

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "gatt_uuid_decl_x.h"
#include "lorawan_controller.h"
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
static deviceId_t mScDbLoRa_SubscribedClientId;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

bleResult_t ScDbLoRa_SetCharacteristicValUTF8s (uint16_t *serviceHandle, bleUuid_t* uuid_char, char* strVal);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
bleResult_t ScDbLoRa_Start (scdbLoRaConfig_t *pServiceConfig)
{
    bleResult_t result;

    /* Clear subscribed client ID (if any) */
    mScDbLoRa_SubscribedClientId = gInvalidDeviceId_c;
    
    result = ScDbLoRa_UpdateAllGattTable(pServiceConfig);

    return result;
}

bleResult_t ScDbLoRa_Stop (scdbLoRaConfig_t *pServiceConfig)
{
  /* Unsubscribe current client */
    return ScDbLoRa_Unsubscribe();
}

bleResult_t ScDbLoRa_Subscribe(deviceId_t deviceId)
{
   /* Subscribe a new client to this service */
    mScDbLoRa_SubscribedClientId = deviceId;

    return gBleSuccess_c;
}

bleResult_t ScDbLoRa_Unsubscribe()
{
   /* Clear current subscribed client ID */
    mScDbLoRa_SubscribedClientId = gInvalidDeviceId_c;
    return gBleSuccess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

bleResult_t ScDbLoRa_UpdateAllGattTable (scdbLoRaConfig_t *pServiceConfig) {

	bleResult_t result;
	uint16_t  handle;

	/* AppEUI*/
	result = ScDbLoRa_SetCharacteristicValUTF8s(&pServiceConfig->serviceHandle,
			(bleUuid_t*)&uuid_lora_app_eui,
			pServiceConfig->loRaCtrlConfig->appEui);
	if (result != gBleSuccess_c)
		return result;

	/* App Key */
	result = ScDbLoRa_SetCharacteristicValUTF8s(&pServiceConfig->serviceHandle,
			(bleUuid_t*)&uuid_lora_app_key,
			pServiceConfig->loRaCtrlConfig->appKey);
	if (result != gBleSuccess_c)
		return result;

	/*  DevEui */
	result = ScDbLoRa_SetCharacteristicValUTF8s(&pServiceConfig->serviceHandle,
			(bleUuid_t*)&uuid_lora_device_eui,
			pServiceConfig->loRaCtrlConfig->devEui);
	if (result != gBleSuccess_c)
		return result;

	/* Confirm Mode */
	result = GattDb_FindCharValueHandleInService(pServiceConfig->serviceHandle,
		gBleUuidType128_c, (bleUuid_t*)&uuid_lora_confirm_mode, &handle);
	if (result != gBleSuccess_c)
		return result;

	uint8_t val = false;
	if(pServiceConfig->loRaCtrlConfig->confirmMode[0] == '1')
		val = true;

	/* Update characteristic value */
	result = GattDb_WriteAttribute(handle, 1, &val);
	if (result != gBleSuccess_c)
		return result;

	/* Dev Addr */
	result = ScDbLoRa_SetCharacteristicValUTF8s(&pServiceConfig->serviceHandle,
			(bleUuid_t*)&uuid_lora_device_address,
			pServiceConfig->loRaCtrlConfig->devAddr);
	if (result != gBleSuccess_c)
		return result;

	/* Nwk Session Key */
	result = ScDbLoRa_SetCharacteristicValUTF8s(&pServiceConfig->serviceHandle,
			(bleUuid_t*)&uuid_lora_network_session_key,
			pServiceConfig->loRaCtrlConfig->nwkSessionKey);
	if (result != gBleSuccess_c)
		return result;

	/* App Session Key */
	result = ScDbLoRa_SetCharacteristicValUTF8s(&pServiceConfig->serviceHandle,
			(bleUuid_t*)&uuid_lora_app_session_key,
			pServiceConfig->loRaCtrlConfig->appSessionKey);
	if (result != gBleSuccess_c)
		return result;

	return gBleSuccess_c;
}


uint8_t ScDbLoRa_SetAppEui (scdbLoRaConfig_t *pScdbConfig, uint8_array_t appEui)
{
    uint16_t  handle;
    bleResult_t result;

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(pScdbConfig->serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)&uuid_lora_app_eui, &handle);

    if (result != gBleSuccess_c)
        return result;

    // The size is tripled because we write 1 byte with 2
    char strAppEui[appEui.arrayLength*3];
    strAppEui[0] = '\0';

    convertBytesArrayToHexString(appEui.pUint8_array, appEui.arrayLength, strAppEui);

    if (lorawan_controller_set_cmd(CMD_SET_APP_EUI, strAppEui) == lorawanController_Success) {
		/* Update characteristic value*/
		return GattDb_WriteAttribute(handle, appEui.arrayLength, (void*) appEui.pUint8_array);
	} else {
		return gBleInvalidParameter_c;
	}
}

uint8_t ScDbLoRa_SetAppKey (scdbLoRaConfig_t *pScdbConfig, uint8_array_t appKey)
{
    uint16_t  handle;
    bleResult_t result;

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(pScdbConfig->serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)&uuid_lora_app_key, &handle);

    if (result != gBleSuccess_c)
        return result;

    // The size is tripled because we write 1 byte with 2
    char strAppKey[appKey.arrayLength*3];
    strAppKey[0] = '\0';

	convertBytesArrayToHexString(appKey.pUint8_array, appKey.arrayLength, strAppKey);

	if (lorawan_controller_set_cmd(CMD_SET_APP_KEY, strAppKey) == lorawanController_Success) {
		/* Update characteristic value*/
		return GattDb_WriteAttribute(handle, appKey.arrayLength, (void*) appKey.pUint8_array);
	} else {
		return gBleInvalidParameter_c;
	}

}

uint8_t ScDbLoRa_SetConfirmMode (scdbLoRaConfig_t *pScdbConfig, uint8_array_t confirmMode)
{
    uint16_t  handle;
    bleResult_t result;

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(pScdbConfig->serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)&uuid_lora_confirm_mode, &handle);

    if (result != gBleSuccess_c)
        return result;

    if(confirmMode.arrayLength != 1){
    	return gBleInvalidParameter_c;
    }

	char * strConfirmMode = "0";
	if (confirmMode.pUint8_array[0] == 1) {
		strConfirmMode[0] = '1';
	} else if (confirmMode.pUint8_array[0] != 0) {
		return gBleInvalidParameter_c;
	}

	if (lorawan_controller_set_cmd(CMD_SET_CONFIRM_MODE, strConfirmMode) == lorawanController_Success) {
		/* Update characteristic value*/
		return GattDb_WriteAttribute(handle, confirmMode.arrayLength, (void*) confirmMode.pUint8_array);
	} else {
		return gBleInvalidParameter_c;
	}
}

uint8_t ScDbLoRa_SetJoinStatus(scdbLoRaConfig_t *pScdbConfig, uint8_array_t joinStatus)
{
    uint16_t  handle;
    bleResult_t result;

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(pScdbConfig->serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)&uuid_lora_network_join_status, &handle);

    if (result != gBleSuccess_c){
        return result;
    }

	if (joinStatus.arrayLength != 1) {
		return gBleInvalidParameter_c;
	}

	return GattDb_WriteAttribute(handle, joinStatus.arrayLength, (void*) joinStatus.pUint8_array);
}

bleResult_t ScDbLoRa_SetCharacteristicValUTF8s (uint16_t *serviceHandle, bleUuid_t* uuid_char, char* strVal) {

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
