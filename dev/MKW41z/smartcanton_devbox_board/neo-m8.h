/**
 * @file    neo-m8.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Functions to initialize a GPS UBlox NEO-M8 connected by SPI.
 * This library does not support the GPS configuration using the 
 * UBX protocol. This means that the GPS can only be used to retrieve
 * RMC/GGA frames from the GPS with his default configuration.
 */

#ifndef __NEO_M8_H__
#define __NEO_M8_H__

/* KSDK */
#include "board.h"

/* Fwk */
#include "fsl_os_abstraction.h"
#include "Panic.h"

#include "minmea/minmea.h"

typedef enum gpsNeoStatus_tag
{
	gpsNeo_Success = 0U, /*!< Success */
	gpsNeo_ReadError = 1U, /*!< Failed */
	gpsNeo_ParseError = 1U, /*!< Failed */
	gpsNeo_InitError = 2U, /*!< Failed */
} gpsNeoStatus_t;


/**
 * @brief Initizlize the SPI to communicate with the GPS. Everytime a new
 * data is available for the user, the callback function in parameter 
 * will be called. Then, the user can call a read function provided
 * in this file to convert the data read to a more friendly representation.
 * 
 * @param app_notification_callback Function callback that will called
 * when a new GPS frame is available.
 * @return osaStatus_t osaStatus_Success if the configuration is successfull
 */
osaStatus_t gps_neo_m8_init(void (*app_notification_callback)(void));

/**
 * @brief Call this function to retrieve the last 
 * 
 * @param frame And RMC structure to store the last data stored
 * @return gpsNeoStatus_t gpsNeo_Success if the RMC sentence is 
 * correctly formed (not necessary valid as a gps frame). Otherwise
 * do not use it.
 */
gpsNeoStatus_t gps_neo_m8_read_rmc(struct minmea_sentence_rmc *frame);

#endif
