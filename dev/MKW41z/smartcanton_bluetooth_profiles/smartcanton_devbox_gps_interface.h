/**
 * @file    smartcanton_devbox_gps_interface.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the interface to manage the gps service of the SmartCanton
 * 			DevBox. Contain function to start, stop, subscribe and unsubscribe from 
 *  		the service. Data can be written to the ble database or/and generate 
 * 			notifications.
 */

#ifndef _SMARTCANTON_DEVBOX_GPS_INTERFACE_H_
#define _SMARTCANTON_DEVBOX_GPS_INTERFACE_H_

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
typedef struct scdbGPSConfig_tag
{
	uint16_t serviceHandle;
} scdbGPSConfig_t;

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
bleResult_t ScDbGPS_Start(scdbGPSConfig_t *pServiceConfig);

/*!**********************************************************************************
 * \brief        Stops Smart Canton Dev Box Service functionality
 *
 * \param[in]    pServiceConfig  Pointer to structure that contains server
 *                               configuration information.
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_Stop(scdbGPSConfig_t *pServiceConfig);

/*!**********************************************************************************
 * \brief        Subscribes a GATT client to the Smart Canton Dev Box service
 *
 * \param[in]    clientDeviceId  Client Id in Device DB.
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_Subscribe(deviceId_t clientDeviceId);

/*!**********************************************************************************
 * \brief        Unsubscribes a GATT client from the Smart Canton Dev Box service
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_Unsubscribe();

/*!**********************************************************************************
 * \brief        Record a new GPS position the GATT database and send a notification
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box GPS service
 * \param[in]	 latitude New Latitude value
 * \param[in]	 longitude New Longitude value
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_RecordNotificationGPSPosition(uint16_t serviceHandle, float* latitude, float* longitude);

/*!**********************************************************************************
 * \brief        Record a new GPS speed the GATT database and send a notification
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box GPS service
 * \param[in]	 speed New GPS speed
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_RecordNotificationGPSSpeed(uint16_t serviceHandle, float* speed);

/*!**********************************************************************************
 * \brief        Record a new GPS course the GATT database and send a notification
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box GPS service
 * \param[in]	 course New GPS course
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_RecordNotificationGPSCourse(uint16_t serviceHandle, float* course);

/*!**********************************************************************************
 * \brief        Record a new GPS Time the GATT database and send a notification
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box GPS service
 * \param[in]	 course New GPS time
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_RecordNotificationGPSTime(uint16_t serviceHandle, struct minmea_time *time);

/*!**********************************************************************************
 * \brief        Record a new GPS Date the GATT database and send a notification
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box GPS service
 * \param[in]	 course New GPS Date
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_RecordNotificationGPSDate(uint16_t serviceHandle, struct minmea_date *date);

/*!**********************************************************************************
 * \brief        Record a complete RMC frame to the GATT database and send all
 * 				 the required notifications.
 * \param[in]	 serviceHandle Handle to the Smart Canton Dev Box GPS service
 * \param[in]	 course New GPS Date
 *
 * \return       gBleSuccess_c or error.
 ************************************************************************************/
bleResult_t ScDbGPS_RecordNotificationFrameRmcAll(uint16_t serviceHandle, struct minmea_sentence_rmc *rmcFrame);

#ifdef __cplusplus
}
#endif

#endif /* _SMARTCANTON_DEVBOX_GPS_INTERFACE_H_ */

/*! **********************************************************************************
 * @}
 ************************************************************************************/
