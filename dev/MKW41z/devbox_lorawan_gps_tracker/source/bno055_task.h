/**
 * @file    bno055_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#ifndef __BNO055_TASK_H__
#define __BNO055_TASK_H__

/* Fwk */
#include "fsl_os_abstraction.h"
#include "fsl_i2c_freertos.h"
#include "BNO055_driver/bno055.h"


#define BNO055_MEASURE_QUEUE_SIZE	8

extern osaMsgQId_t gBno055NewMessageMeasureQ;

typedef struct bno055Data_tag
{
	struct bno055_accel_float_t accel_xyz;
	struct bno055_mag_float_t mag_xyz;
	struct bno055_gyro_float_t gyro_xyz;
	struct bno055_gravity_float_t gravity;
} bno055Data_t;

/*
 * These values should be modified by the application as necessary.
 * They are used by the BNO055 Task  initialization code from bno55_task.c.
 */
/*! Idle Task Stack Size */
#define gBno055TaskStackSize_c (500)

/*! Bno55 Task OS Abstraction Priority */
#define gBno055TaskPriority_c  (OSA_PRIORITY_LOW)

void Bno055_Task(osaTaskParam_t argument);
osaStatus_t Bno055_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle);

osaStatus_t Bno055Task_SetMeasureDelay(uint32_t delay);

uint32_t Bno055Task_GetMeasureDelay();

#endif /* __BNO055_TASK_H__ */
