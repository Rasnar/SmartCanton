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
 * This file is the interface file for the BNO055 Service
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

#ifndef _SMARTCANTON_DEVBOX_BNO_055_INTERFACE_H_
#define _SMARTCANTON_DEVBOX_BNO_055_INTERFACE_H_

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

/**
 * This include is only used to be able to use the structure utf8s_t.
 * TODO: Maybe we can implement a ifndef with on both file to define structure in both
 * files.
 */
#include "device_info_interface.h"

/**
 * Only included to have access to the bno055Data_t structure. In the future we should
 * implement a commun interface to provide this structure to all needed files.
 * TODO: Implement a commun interface with the implementation of bno055Data_t
 */
#include "bno055_task.h"
#include "BNO055_driver/bno055.h"

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
typedef struct scdbBno055Config_tag
{
	uint16_t serviceHandle;
} scdbBno055Config_t;

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
bleResult_t ScDbBno055_Start(scdbBno055Config_t *pServiceConfig);

/*!**********************************************************************************
 * \brief        Stops Smart Canton Dev Box Service functionality
 *
 * \param[in]    pServiceConfig  Pointer to structure that contains server
 *                               configuration information.
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_Stop(scdbBno055Config_t *pServiceConfig);

/*!**********************************************************************************
 * \brief        Subscribes a GATT client to the Smart Canton Dev Box service
 *
 * \param[in]    clientDeviceId  Client Id in Device DB.
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_Subscribe(deviceId_t clientDeviceId);

/*!**********************************************************************************
 * \brief        Unsubscribes a GATT client from the Smart Canton Dev Box service
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_Unsubscribe();

/*!**********************************************************************************
 * \brief        Sends Indoor air quality value notification to a peer GATT
 * 				 client with data given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BNO055 service
 * \param[in]	 accel Accelerometer 3 axis
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_InstantValueNotificationAccel(uint16_t serviceHandle, struct bno055_accel_float_t* accel);

/*!**********************************************************************************
 * \brief        Sends Indoor air quality value notification to a peer GATT
 * 				 client with data given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BNO055 service
 * \param[in]	 gyro Gyroscope 3 axis
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_InstantValueNotificationGyro(uint16_t serviceHandle, struct bno055_gyro_float_t* gyro);

/*!**********************************************************************************
 * \brief        Sends magnetometer values notification to a peer GATT
 * 				 client with data given as parameter, ignoring the GATT Database.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BNO055 service
 * \param[in]	 mag Magnetometer 3 axis
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_InstantValueNotificationMag(uint16_t serviceHandle, struct bno055_mag_float_t* mag);


/*!**********************************************************************************
 * \brief        Send all BNO055 data to the connected peer. Will call all the Instant
 * 				 Value functions implemented to provide all the notifications.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BNO055 service
 * \param[in]	 Bno055 Struct with all the BNO055 data
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_InstantValueNotificationAll(uint16_t serviceHandle, bno055Data_t* bno055Data);

/*!**********************************************************************************
 * \brief        Update the value of the measure interval applied to the BNO055
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box BNO055 service
 * \param[in]	 delay Delay in ms applied to the BNO055
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbBno055_RecordValueMeasureInterval(uint16_t serviceHandle, uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif /* _SMARTCANTON_DEVBOX_BNO_055_INTERFACE_H_ */

/*! **********************************************************************************
 * @}
 ************************************************************************************/
