

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
    /* Clear subscibed clien ID (if any) */
    mScDb_SubscribedClientId = gInvalidDeviceId_c;
    
    /* Set the initial value defined in pServiceConfig to the characteristic values */
    return ScDb_RecordPotentiometerMeasurement (pServiceConfig->serviceHandle,
                                             pServiceConfig->potentiometerValue);
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
static void ScDb_SendPotentiometerMeasurementNotification
(
  uint16_t handle
)
{
    uint16_t  hCccd;
    bool_t isNotificationActive;

    /* Get handle of CCCD */
    if (GattDb_FindCccdHandleForCharValueHandle(handle, &hCccd) != gBleSuccess_c)
        return;

    if (gBleSuccess_c == Gap_CheckNotificationStatus
        (mScDb_SubscribedClientId, hCccd, &isNotificationActive) &&
        TRUE == isNotificationActive)
    {
        GattServer_SendNotification(mScDb_SubscribedClientId, handle);
    }
}

bleResult_t ScDb_RecordPotentiometerMeasurement (uint16_t serviceHandle, uint8_t newPotentiometerValue)
{
    uint16_t  handle;
    bleResult_t result;


    /* Get handle of Potentiometer characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
        gBleUuidType128_c, (bleUuid_t*)&uuid_characteristic_potentiometer_relative_value, &handle);

    if (result != gBleSuccess_c)
        return result;

    /* Update characteristic value */
    result = GattDb_WriteAttribute(handle, sizeof(uint8_t), (uint8_t*)&newPotentiometerValue);

    if (result != gBleSuccess_c)
        return result;

    ScDb_SendPotentiometerMeasurementNotification(handle);

    return gBleSuccess_c;
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
