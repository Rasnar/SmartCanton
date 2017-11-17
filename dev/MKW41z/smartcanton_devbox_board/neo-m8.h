/**
 * @file    neo-m8.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
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
}gpsNeoStatus_t;

osaStatus_t gps_neo_m8_init(void);

gpsNeoStatus_t gps_neo_m8_read_rmc(struct minmea_sentence_rmc *frame);

#endif
