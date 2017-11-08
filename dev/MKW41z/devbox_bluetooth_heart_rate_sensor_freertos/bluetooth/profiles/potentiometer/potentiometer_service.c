/*! *********************************************************************************
 * \addtogroup Potentiometer Profile
 * @{
 ********************************************************************************** */
/*!
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * \file
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include <bluetooth/profiles/potentiometer/potentiometer_interface.h>
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
static deviceId_t mPs_SubscribedClientId;

//uint8_t uuid_service_potentiometer[16] = { 0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x04, 0x56, 0xFF, 0x02};
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
bleResult_t Ps_Start (psConfig_t *pServiceConfig)
{
    /* Clear subscibed clien ID (if any) */
    mPs_SubscribedClientId = gInvalidDeviceId_c;
    
    /* Set the initial value defined in pServiceConfig to the characteristic values */
    return Ps_RecordPotentiometerMeasurement (pServiceConfig->serviceHandle,
                                             pServiceConfig->potentiometerValue);
}

bleResult_t Ps_Stop (psConfig_t *pServiceConfig)
{
  /* Unsubscribe current client */
    return Ps_Unsubscribe();
}

bleResult_t Ps_Subscribe(deviceId_t deviceId)
{
   /* Subscribe a new client to this service */
    mPs_SubscribedClientId = deviceId;

    return gBleSuccess_c;
}

bleResult_t Ps_Unsubscribe()
{
   /* Clear current subscribed client ID */
    mPs_SubscribedClientId = gInvalidDeviceId_c;
    return gBleSuccess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
static void Ps_SendPotentiometerMeasurementNotification
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
        (mPs_SubscribedClientId, hCccd, &isNotificationActive) &&
        TRUE == isNotificationActive)
    {
        GattServer_SendNotification(mPs_SubscribedClientId, handle);
    }
}

bleResult_t Ps_RecordPotentiometerMeasurement (uint16_t serviceHandle, uint8_t newPotentiometerValue)
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

    Ps_SendPotentiometerMeasurementNotification(handle);

    return gBleSuccess_c;
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
