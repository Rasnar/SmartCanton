

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "smartcanton_devbox_interface.h"
#include "gatt_uuid_decl_x.h"

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

uint8_t ScDb_SetAppEui (scdbConfig_t *pScdbConfig, utf8s_t appEui)
{
    uint16_t  handle;
    bleResult_t result;

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(pScdbConfig->serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)&uuid_lora_app_eui, &handle);

    if (result != gBleSuccess_c)
        return result;

    /* Update characteristic value*/
    return GattDb_WriteAttribute(handle, appEui.stringLength, (void*)appEui.pUtf8s);
}

uint8_t ScDb_SetAppKey (scdbConfig_t *pScdbConfig, utf8s_t appKey)
{
    uint16_t  handle;
    bleResult_t result;

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(pScdbConfig->serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)&uuid_lora_app_key, &handle);

    if (result != gBleSuccess_c)
        return result;

    /**
     * TODO: Sent data to LoRa MCU and check response.
     */
    return gBleInvalidParameter_c;

    /* Update characteristic value*/
    return GattDb_WriteAttribute(handle, appKey.stringLength, (void*)appKey.pUtf8s);
}


/*! *********************************************************************************
 * @}
 ********************************************************************************** */
