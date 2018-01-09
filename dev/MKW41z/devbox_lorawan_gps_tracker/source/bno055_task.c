/**
 * @file    bno055_task.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the bno055 task. Functions to init and manage the task. 
 */

#include "bno055_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "Panic.h"
#include "bno055_support.h"

/* Define the task with the given parameters */
OSA_TASK_DEFINE(Bno055_Task, gBno055TaskPriority_c, 1, gBno055TaskStackSize_c, FALSE);

/* Holding the current Task id */
osaTaskId_t gBno055TaskId = 0;

/* Bno055 informations and configuration */
struct bno055_t bno055;

/* Handle to hold the i2c peripheral that is connected to the Bno055 */
static i2c_rtos_handle_t* master_rtos_handle;

/* Queue to hold the data to be send to the task consumming it */
osaMsgQId_t gBno055NewMeasureQ;

/* Current interval beetwen measures */
uint32_t mIntervalNewMsgSentMilliSeconds = BNO055_MINIMUM_MEASURE_INTERVAL_MS * 10;

/**
 * @brief Callback function called when an interruption as been received from the bno055
 * 
 */
static void bno055_new_data_available_callback(void)
{
	/*
	 * Use this callback if the BNO055 is configured to generate an interruption
	 * when a event is detected or when a capture is finished
	 */
}

/**
 * Main BNO055 task function
 */
void Bno055_Task(osaTaskParam_t argument)
{
	int8_t rslt = 0;

	bno055_kw41z_I2C_routines_init(&bno055, master_rtos_handle, bno055_new_data_available_callback);
	rslt = bno055_init(&bno055);

	// Data to be sent to the main task
	static bno055Data_t* bno055Data;

	rslt += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
	while (1)
	{
		rslt = BNO055_SUCCESS;

		OSA_TimeDelay(mIntervalNewMsgSentMilliSeconds);

		bno055Data = pvPortMalloc(sizeof(bno055Data_t));

		rslt += bno055_convert_float_accel_xyz_mg(&bno055Data->accel_xyz);
		rslt += bno055_convert_float_gyro_xyz_dps(&bno055Data->gyro_xyz);
		rslt += bno055_convert_float_mag_xyz_uT(&bno055Data->mag_xyz);
		rslt += bno055_convert_float_gravity_xyz_msq(&bno055Data->gravity);

		/* Direcrtly transfert the accelerometer values with g unit */
		bno055Data->accel_xyz.x /= 1024.0;
		bno055Data->accel_xyz.y /= 1024.0;
		bno055Data->accel_xyz.z /= 1024.0;

		if(OSA_MsgQPut(gBno055NewMeasureQ, &bno055Data) == osaStatus_Success) {
			/* Only notify main task if the message can be added successfully to the Queue */
			OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewBno055MeasureAvailable_c);
		} else {
			/* Otherwise, free the reserved memory */
			vPortFree(bno055Data);
		}
	}
}

osaStatus_t Bno055_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle)
{
	if (gBno055TaskId)
	{
		return osaStatus_Error;
	}

	master_rtos_handle = i2c_master_rtos_handle;

	/* Create application Queue */
	gBno055NewMeasureQ = OSA_MsgQCreate(BNO055_MEASURE_QUEUE_SIZE);
	if ( NULL == gBno055NewMeasureQ)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Task creation */
	gBno055TaskId = OSA_TaskCreate(OSA_TASK(Bno055_Task), NULL);

	if ( NULL == gBno055TaskId)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	return osaStatus_Success;
}

osaStatus_t Bno055Task_SetMeasureInterval(uint32_t interval_ms)
{
	if ((interval_ms >= BNO055_MINIMUM_MEASURE_INTERVAL_MS)
			&& (interval_ms <= BNO055_MAXIMUM_MEASURE_INTERVAL_MS))
	{
		mIntervalNewMsgSentMilliSeconds = interval_ms;
		return osaStatus_Success;
	}
	return osaStatus_Error;
}

uint32_t Bno055Task_GetMeasureInterval()
{
	return mIntervalNewMsgSentMilliSeconds;
}

