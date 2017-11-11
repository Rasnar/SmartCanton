

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "smartcanton_devbox_interface.h"
#include "gatt_uuid_decl_x.h"
#include "lorawan_controller.h"
#include "stdio.h"

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
static deviceId_t mScDb_SubscribedClientId;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
bleResult_t ScDb_Start (scdbConfig_t *pServiceConfig)
{
//    bleResult_t result;

    /* Clear subscibed clien ID (if any) */
    mScDb_SubscribedClientId = gInvalidDeviceId_c;
    
//    /* Set the initial value defined in pServiceConfig to the characteristic values */
//    return ScDb_RecordPotentiometerMeasurement (pServiceConfig->serviceHandle,
//                                             pServiceConfig->potentiometerValue);

    return gBleSuccess_c;
}

bleResult_t ScDb_Stop (scdbConfig_t *pServiceConfig)
{
  /* Unsubscribe current client */
    return ScDb_Unsubscribe();
}

bleResult_t ScDb_Subscribe(deviceId_t deviceId)
{
   /* Subscribe a new client to this service */
    mScDb_SubscribedClientId = deviceId;

    return gBleSuccess_c;
}

bleResult_t ScDb_Unsubscribe()
{
   /* Clear current subscribed client ID */
    mScDb_SubscribedClientId = gInvalidDeviceId_c;
    return gBleSuccess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

int convertBytesArrayToHexString(uint8_t *buffer, uint16_t bufferLength, char* str){

	int i;

	if (bufferLength > 1) {
		for (i = 0; i < bufferLength - 1; i++) {
			sprintf(&str[3*i], "%02X:", buffer[i]);
		};
	}

	sprintf(&str[3*i], "%02X", buffer[i]);
    return 3*i + 2; // String length
}

uint8_t ScDb_SetAppEui (scdbConfig_t *pScdbConfig, uint8_array_t appEui)
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

uint8_t ScDb_SetAppKey (scdbConfig_t *pScdbConfig, uint8_array_t appKey)
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


/*! *********************************************************************************
 * @}
 ********************************************************************************** */
