#include <bme680_bsec_support.h>
/**
 * @file    bme680_task.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include "bme680_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "Panic.h"

OSA_TASK_DEFINE(Bme680_Task, gBme680TaskPriority_c, 1, gBme680TaskStackSize_c, FALSE);
osaTaskId_t gBme680TaskId = 0;

static i2c_rtos_handle_t* master_rtos_handle;

struct bme680_dev bme680;


/*!
 * @brief           Handling of the ready outputs
 *
 * @param[in]       timestamp       time in nanoseconds
 * @param[in]       iaq             IAQ signal
 * @param[in]       iaq_accuracy    accuracy of IAQ signal
 * @param[in]       temperature     temperature signal
 * @param[in]       humidity        humidity signal
 * @param[in]       pressure        pressure signal
 * @param[in]       raw_temperature raw temperature signal
 * @param[in]       raw_humidity    raw humidity signal
 * @param[in]       gas             raw gas sensor signal
 * @param[in]       bsec_status     value returned by the bsec_do_steps() call
 *
 * @return          none
 */
void output_ready(int64_t timestamp, float iaq, uint8_t iaq_accuracy, float temperature, float humidity,
     float pressure, float raw_temperature, float raw_humidity, float gas, bsec_library_return_t bsec_status)
{
    // ...
    // Please insert system specific code to further process or display the BSEC outputs
    // ...



	/* Inform the DevBox Task that she can read the data avaible */
	OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewBME680Measure_c);
}

/**
 * Main BME680 task function
 */
void Bme680_Task(osaTaskParam_t argument)
{
	int8_t rslt = 0;
	(void)rslt;

	bme680_bsec_kw41z_I2C_routines_init(&bme680, master_rtos_handle);

	bme680_bsec_kw41z_iot_loop(output_ready);
	// Should never be reached
	while (1)
	{
		OSA_TimeDelay(1000);
	}
}

osaStatus_t Bme680_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle)
{
	if (gBme680TaskId)
	{
		return osaStatus_Error;
	}

	master_rtos_handle = i2c_master_rtos_handle;

	/* Task creation */
	gBme680TaskId = OSA_TaskCreate(OSA_TASK(Bme680_Task), NULL);

	if ( NULL == gBme680TaskId)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	return osaStatus_Success;
}

