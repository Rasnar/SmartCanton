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
 * This file is the interface file for the BME680 Service
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

#ifndef _SMARTCANTON_DEVBOX_BME_680_INTERFACE_H_
#define _SMARTCANTON_DEVBOX_BME_680_INTERFACE_H_

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
#include "lorawan_controller.h"
#include "minmea/minmea.h"

/**
 * This include is only used to be able to use the structure utf8s_t.
 * TODO: Maybe we can implement a ifndef with on both file to define structure in both
 * files.
 */
#include "device_info_interface.h"

/**
 * Only included to have access to the bme680Data_t structure. In the future we should
 * implement a commun interface to provide this structure to all needed files.
 * TODO: Implement a commun interface with the implementation of bme680Data_t
 */
#include "bme680_task.h"

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

/*! Smart Canton Dev Box Service - Configuration */
typedef struct scdbBme680Config_tag
{
	uint16_t serviceHandle;
} scdbBme680Config_t;

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
extern "C"
{
#endif

/*!**********************************************************************************
 * \brief        Starts Smart Canton Dev Box Service functionality
 *
 * \param[in]    pServiceConfig  Pointer to structure that contains server
 *                               configuration information.
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_Start(scdbBme680Config_t *pServiceConfig);

/*!**********************************************************************************
 * \brief        Stops Smart Canton Dev Box Service functionality
 *
 * \param[in]    pServiceConfig  Pointer to structure that contains server
 *                               configuration information.
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_Stop(scdbBme680Config_t *pServiceConfig);

/*!**********************************************************************************
 * \brief        Subscribes a GATT client to the Smart Canton Dev Box service
 *
 * \param[in]    clientDeviceId  Client Id in Device DB.
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_Subscribe(deviceId_t clientDeviceId);

/*!**********************************************************************************
 * \brief        Unsubscribes a GATT client from the Smart Canton Dev Box service
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_Unsubscribe();

/*!**********************************************************************************
 * \brief        Sends Indoor air quality value notification to a peer GATT
 * 				 Client with data given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 iaq Indoor air quality value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValueIaq(uint16_t serviceHandle, float* iaq);

/*!**********************************************************************************
 * \brief        Sends Indoor air quality accuracy value notification to a peer GATT
 * 				 Client with data given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 iaq_accuracy Indoor air quality accuracy value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValueIaqAccuracy(uint16_t serviceHandle, uint8_t* iaq_accuracy);

/*!**********************************************************************************
 * \brief        Sends temperature value notification to a peer GATT Client with data
 * 				 given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 temperature Temperature value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValueTemperature(uint16_t serviceHandle, float* temperature);

/*!**********************************************************************************
 * \brief        Sends humidity value notification to a peer GATT Client with data
 * 				 given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 humidity Humidity value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstanHumidity(uint16_t serviceHandle, float* humidity);

/*!**********************************************************************************
 * \brief        Sends pressure value notification to a peer GATT Client with data
 * 				 given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 pressure Pressure value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValuePressure(uint16_t serviceHandle, float* pressure);

/*!**********************************************************************************
 * \brief        Sends raw temperature value notification to a peer GATT Client with data
 * 				 given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 raw_temperature Raw temperature value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValueRawTemperature(uint16_t serviceHandle, float* raw_temperature);

/*!**********************************************************************************
 * \brief        Sends raw humidity value notification to a peer GATT Client with data
 * 				 given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 raw_humidity Raw humidity value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValueRawHumidity(uint16_t serviceHandle, float* raw_humidity);

/*!**********************************************************************************
 * \brief        Sends raw gas value notification to a peer GATT Client with data
 * 				 given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 raw_gas Gas value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValueRawGas(uint16_t serviceHandle, float* raw_gas);

/*!**********************************************************************************
 * \brief        Send all BME680 data to the connected peer. Will call all the Instant
 * 				 Value functions implemented to provide all the notifications.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BME680 service
 * \param[in]	 bme680Data Struct with all the BME680 data
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBme680_InstantValueAll(uint16_t serviceHandle, bme680Data_t* bme680Data);

#ifdef __cplusplus
}
#endif

#endif /* _SMARTCANTON_DEVBOX_BME_680_INTERFACE_H_ */

/*! **********************************************************************************
 * @}
 ************************************************************************************/