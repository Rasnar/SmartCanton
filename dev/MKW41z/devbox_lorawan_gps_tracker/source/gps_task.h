/**
 * @file    gps_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the GPS task. Function to hold the init the task, to manage the task
 * and a callback to notify to task when data are ready to be read from the device.
 */

#ifndef __GPS_TASK_H__
#define __GPS_TASK_H__

/* Fwk */
#include "fsl_os_abstraction.h"
#include "minmea/minmea.h"

/* Number of maximum elements handled by the queue */
#define GPS_MEASURE_QUEUE_SIZE	4

/* Queue to hold the data to be send to the task consumming it */
extern osaMsgQId_t gGpsNewMessageMeasureQ;

typedef struct gpsData_tag
{
	struct minmea_sentence_rmc frame_rmc;
} gpsData_t;

/*
 * These values should be modified by the application as necessary.
 * They are used by the GPS Task  initialization code from gps_task.c.
 */
/* GPS Task Stack Size */
#define gGpsTaskStackSize_c (900)

/* GPS Task OS Abstraction Priority */
#define gGpsTaskPriority_c  (OSA_PRIORITY_NORMAL)

/**
 * @brief Main task function, infinite loop capturing data and sending them 
 * through the gGpsNewMessageMeasureQ.
 * 
 * @param argument Parameters that can be sent to the task.
 */
void Gps_Task(osaTaskParam_t argument);

/**
 * @brief Inititalize the Gps parameters, event and the data queue. 
 * 
 * @return osaStatus_t osaStatus_Success if the initialization is a success,
 * otherwise osaStatus_error
 */
osaStatus_t Gps_TaskInit();

#endif /* __GPS_TASK_H__ */
