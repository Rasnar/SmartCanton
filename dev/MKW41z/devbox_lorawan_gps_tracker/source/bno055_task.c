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

#define mDelayPacketSentsSeconds	300

OSA_TASK_DEFINE(Bno055_Task, gBno055TaskPriority_c, 1, gBno055TaskStackSize_c, FALSE);
osaTaskId_t gBno055TaskId = 0;

struct bno055_t bno055;

struct bno055_accel_t accel_xyz;
struct bno055_mag_t mag_xyz;
struct bno055_gyro_t gyro_xyz;
struct bno055_gravity_t gravity;

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

	//	osaEventFlags_t event;
	int8_t rslt = 0;

	bno055_kw41z_I2C_routines_init(&bno055, bno055_new_data_available_callback);
	rslt = bno055_init(&bno055);

	while (1)
	{
		rslt = BNO055_SUCCESS;

		rslt += bno055_set_operation_mode(BNO055_OPERATION_MODE_AMG);
		rslt += bno055_read_accel_xyz(&accel_xyz);
		rslt += bno055_read_mag_xyz(&mag_xyz);
		rslt += bno055_read_gyro_xyz(&gyro_xyz);

		rslt += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
		rslt += bno055_read_gravity_xyz(&gravity);
		OSA_TimeDelay(1000);
	}
}

osaStatus_t Bno055_TaskInit(void)
{
	if (gBno055TaskId)
	{
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

