/**
 * @file    gps_task.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the GPS task. Function to hold the init the task, to manage the task
 * and a callback to notify to task when data are ready to be read from the device.
 */

#include "gps_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "Panic.h"
#include "neo-m8.h"

/* Define the task with the given parameters */
OSA_TASK_DEFINE(Gps_Task, gGpsTaskPriority_c, 1, gGpsTaskStackSize_c, FALSE);

/* Holding the current Task id */
osaTaskId_t gGpsTaskId = 0;

/* Queue to hold the data to be send to the task consumming it */
osaMsgQId_t gGpsNewMeasureQ;

/* Event used to notify the GPS task when a data is ready to be parsed
 * to a valid GPS sentence */
osaEventId_t mGpsEvent;

/* <Notify the task that a new data is available to be read> */
#define mGpsTaskEvtNewDataAvailable_c       		(1 << 0)

/**
 * Callback function called when new data have been read from the GPS and can
 * be converted to NMEA data using the parsing function
 */
int cnt_seconds = 0;
void gps_neo_m8_new_data_available_callback(void)
{
	/* Inform the DevBox Task that she can read the data available */
	OSA_EventSet(mGpsEvent, mGpsTaskEvtNewDataAvailable_c);

	Led3Toggle();
}

/**
 * Main Gps task function
 */
void Gps_Task(osaTaskParam_t argument)
{
	if(gps_neo_m8_init(gps_neo_m8_new_data_available_callback) != osaStatus_Success)
		return;

	/* Data to be sent to the main task */
	static gpsData_t* gpsData;

	/* Contain the last event reveiced */
	osaEventFlags_t event;

	while (1)
	{
		/* Wait to receive an event from a task to do an action */
		OSA_EventWait(mGpsEvent, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &event);

		if (event & mGpsTaskEvtNewDataAvailable_c)
		{
			gpsData = pvPortMalloc(sizeof(gpsData_t));

			if (gps_neo_m8_read_rmc(&gpsData->frame_rmc) == gpsNeo_Success)
			{
				if (OSA_MsgQPut(gGpsNewMeasureQ, &gpsData) == osaStatus_Success)
				{
					/* Only notify main task if the message can be added successfully to the Queue */
					OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewGgpMeasureAvailable_c);
				}
				else
				{
					/* Otherwise, free the reserved memory */
					vPortFree(gpsData);
				}
			}
			else
			{
				/* Otherwise, free the reserved memory */
				vPortFree(gpsData);
			}
		}
	}
}

osaStatus_t Gps_TaskInit()
{
	if (gGpsTaskId)
	{
		return osaStatus_Error;
	}

	/* Create application Queue */
	gGpsNewMeasureQ = OSA_MsgQCreate(GPS_MEASURE_QUEUE_SIZE);
	if ( NULL == gGpsNewMeasureQ)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Create application event */
	mGpsEvent = OSA_EventCreate(TRUE);
	if ( NULL == gDevBoxAppEvent)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Task creation */
	gGpsTaskId = OSA_TaskCreate(OSA_TASK(Gps_Task), NULL);

	if ( NULL == gGpsTaskId)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	return osaStatus_Success;
}


