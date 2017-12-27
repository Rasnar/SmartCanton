/**
 * @file    bno055_task.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include "bno055_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "Panic.h"
#include "bno055_support.h"

#define mDelayNewMsgSentMilliSeconds	100

OSA_TASK_DEFINE(Bno055_Task, gBno055TaskPriority_c, 1, gBno055TaskStackSize_c, FALSE);
osaTaskId_t gBno055TaskId = 0;

struct bno055_t bno055;

struct bno055_accel_t accel_xyz;
struct bno055_mag_t mag_xyz;
struct bno055_gyro_t gyro_xyz;
struct bno055_gravity_t gravity;

static i2c_rtos_handle_t* master_rtos_handle;

osaMsgQId_t gBno055NewMessageMeasureQ;

/**
 * Callback function called when an interruption as been received from the
 *
 */
void bno055_new_data_available_callback(void)
{
	/* Inform the DevBox Task that she can read the data available */

	Led3Toggle();
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

		OSA_TimeDelay(mDelayNewMsgSentMilliSeconds);

		bno055Data = pvPortMalloc(sizeof(bno055Data_t));

		rslt += bno055_convert_float_accel_xyz_mg(&bno055Data->accel_xyz);
		rslt += bno055_convert_float_gyro_xyz_dps(&bno055Data->gyro_xyz);
		rslt += bno055_convert_float_mag_xyz_uT(&bno055Data->mag_xyz);
		rslt += bno055_convert_float_gravity_xyz_msq(&bno055Data->gravity);

		OSA_MsgQPut(gBno055NewMessageMeasureQ, &bno055Data);
		OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewBNO055Measure_c);
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
	gBno055NewMessageMeasureQ = OSA_MsgQCreate(BNO055_MEASURE_QUEUE_SIZE);
	if ( NULL == gBno055NewMessageMeasureQ)
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

