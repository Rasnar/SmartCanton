/**
 * @file    bme680_task.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the bme680 task. Functions to init and manage the task.
 */

#include <bme680_bsec_support.h>
#include "bme680_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "Panic.h"

/* Define the task with the given parameters */
OSA_TASK_DEFINE(Bme680_Task, gBme680TaskPriority_c, 1, gBme680TaskStackSize_c, FALSE);

/* Holding the current Task id */
osaTaskId_t gBme680TaskId = 0;

/* Handle to hold the i2c peripheral that is connected to the Bme680 */
static i2c_rtos_handle_t* master_rtos_handle;

/* Bme680 struct holding the device configuration */
struct bme680_dev bme680;

/* Queue to hold the data to be send to the task consumming it */
osaMsgQId_t gBme680NewMessageMeasureQ;

/**
 * @brief Handling of the ready outputs
 * 
 * @param timestamp time in nanoseconds
 * @param iaq IAQ signal
 * @param iaq_accuracy accuracy of IAQ signal
 * @param temperature temperature signal
 * @param humidity humidity signal
 * @param pressure pressure signal
 * @param raw_temperature raw temperature signal
 * @param raw_humidity raw humidity signal
 * @param gas raw gas sensor signal
 * @param bsec_status value returned by the bsec_do_steps() call
 */
void output_ready(int64_t timestamp, float iaq, uint8_t iaq_accuracy, float temperature, float humidity,
     float pressure, float raw_temperature, float raw_humidity, float gas, bsec_library_return_t bsec_status)
{
	// Data to be sent to the main task
	static bme680Data_t* bme680Data;

	bme680Data = pvPortMalloc(sizeof(bme680Data_t));

	bme680Data->iaq = iaq;
	bme680Data->iaq_accuracy = iaq_accuracy;
	bme680Data->temperature = temperature;
	bme680Data->humidity = humidity;
	bme680Data->pressure = pressure / 100; // Convert Pa to hPa
	bme680Data->raw_temperature = raw_temperature;
	bme680Data->raw_humidity = raw_humidity;
	bme680Data->gas = gas;

	if(OSA_MsgQPut(gBme680NewMessageMeasureQ, &bme680Data) == osaStatus_Success)
	{
		/* Only notify main task if the message can be added successfully to the Queue */
		/* Inform the DevBox Task that she can read the data available */
		OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewBme680MeasureAvailable_c);
	}
	else
	{
		/* Otherwise, free the reserved memory */
		vPortFree(bme680Data);
	}
}


/**
 * Main BME680 task function
 */
void Bme680_Task(osaTaskParam_t argument)
{
	int8_t rslt = 0;
	(void)rslt;

	while (1)
	{
		bme680_bsec_kw41z_I2C_routines_init(&bme680, master_rtos_handle);

		/**
		 * By default this function is an infinite loop but there is a bug
		 * with the current implementation.
		 * It will quit when the Get MSEC is too small for the library
		 * After 1h, we restart the library with a clean configuration.
		 * TODO: Try to find a better way to do this. The ideal will be to
		 * have a Get MSEC in 64bits instead of 32 bits for now.
		 * See inside the function to have a peak where the calculus fails.
		 */
		bme680_bsec_kw41z_iot_loop(output_ready);
	}
}

osaStatus_t Bme680_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle)
{
	if (gBme680TaskId)
	{
		return osaStatus_Error;
	}

	master_rtos_handle = i2c_master_rtos_handle;

	/* Create application Queue */
	gBme680NewMessageMeasureQ = OSA_MsgQCreate(BME680_MEASURE_QUEUE_SIZE);
	if ( NULL == gBme680NewMessageMeasureQ)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Task creation */
	gBme680TaskId = OSA_TaskCreate(OSA_TASK(Bme680_Task), NULL);

	if ( NULL == gBme680TaskId)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	return osaStatus_Success;
}

