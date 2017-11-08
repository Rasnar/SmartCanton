

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
    bleResult_t result;

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

uint8_t ScDb_AppEuiHandler (scdbConfig_t *pScdbUserData, utf8s_t value)
{
    uint8_t retStatus = gAttErrCodeNoError_c;

//    switch (value)
//    {
//        case gHrs_CpResetEnergyExpended_c:
//        {
//            Hrs_ResetExpendedEnergy(pHrsUserData);
//            retStatus = gAttErrCodeNoError_c;
//        }
//        break;
//
//        default:
//        {
//            retStatus = gAttErrCodeCtrlPointValueNotSupported_c;
//        }
//        break;
//    }
    return retStatus;
}

uint8_t ScDb_AppKeyHandler (scdbConfig_t *pScdbUserData, utf8s_t value)
{
    uint8_t retStatus = gAttErrCodeNoError_c;

//    switch (value)
//    {
//        case gHrs_CpResetEnergyExpended_c:
//        {
//            Hrs_ResetExpendedEnergy(pHrsUserData);
//            retStatus = gAttErrCodeNoError_c;
//        }
//        break;
//
//        default:
//        {
//            retStatus = gAttErrCodeCtrlPointValueNotSupported_c;
//        }
//        break;
//    }
    return retStatus;
}


/*! *********************************************************************************
 * @}
 ********************************************************************************** */
