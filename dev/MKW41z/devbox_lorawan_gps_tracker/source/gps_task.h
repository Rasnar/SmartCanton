/**
 * @file    gps_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#ifndef __GPS_TASK_H__
#define __GPS_TASK_H__

/* Fwk */
#include "fsl_os_abstraction.h"
#include "minmea/minmea.h"

#define GPS_MEASURE_QUEUE_SIZE	4

extern osaMsgQId_t gGpsNewMessageMeasureQ;

typedef struct gpsData_tag
{
	struct minmea_sentence_rmc frame_rmc;
} gpsData_t;

/*
 * These values should be modified by the application as necessary.
 * They are used by the GPS Task  initialization code from gps_task.c.
 */
/*! Idle Task Stack Size */
#define gGpsTaskStackSize_c (900)

/*! Gps Task OS Abstraction Priority */
#define gGpsTaskPriority_c  (OSA_PRIORITY_NORMAL)

void Gps_Task(osaTaskParam_t argument);
osaStatus_t Gps_TaskInit();

osaStatus_t GpsTask_SetMeasureDelay(uint32_t delay);

uint32_t GpsTask_GetMeasureDelay();

#endif /* __GPS_TASK_H__ */
