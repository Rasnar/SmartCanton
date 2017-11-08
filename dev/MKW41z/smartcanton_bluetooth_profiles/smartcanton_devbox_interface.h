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
 * This file is the interface file for the Heart Rate Service
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

#ifndef _POTENTIOMETER_INTERFACE_H_
#define _POTENTIOMETER_INTERFACE_H_

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "FunctionLib.h"

#include "ble_general.h"
#include "gatt_db_app_interface.h"
#include "gatt_server_interface.h"
#include "gap_interface.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! Potentiometer Service - Configuration */
typedef struct scdbConfig_tag
{
    uint16_t    serviceHandle;                 /*!<Service handle */
    uint8_t     potentiometerValue;            /*!<Input report field */
} scdbConfig_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!**********************************************************************************
* \brief        Starts Heart Rate Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server 
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ScDb_Start(scdbConfig_t  *pServiceConfig);

/*!**********************************************************************************
* \brief        Stops Heart Rate Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server 
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ScDb_Stop(scdbConfig_t  *pServiceConfig);

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the Heart Rate service
*
* \param[in]    clientDeviceId  Client Id in Device DB.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ScDb_Subscribe(deviceId_t clientDeviceId);

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the Heart Rate service
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ScDb_Unsubscribe();

/*!**********************************************************************************
* \brief        Records heart rate measurement on a specified service handle.
*
* \param[in]    serviceHandle   Service handle.
* \param[in]    newPotentiometerValue           Potentiometer Value.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ScDb_RecordPotentiometerMeasurement (uint16_t serviceHandle, uint8_t newPotentiometerValue);


#ifdef __cplusplus
}
#endif

#endif /* _POTENTIOMETER_INTERFACE_H_ */

/*! **********************************************************************************
 * @}
 ************************************************************************************/
