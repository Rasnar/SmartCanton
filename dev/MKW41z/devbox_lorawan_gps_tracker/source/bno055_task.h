/**
 * @file    bno055_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the bno055 task. Functions to init and manage the task. 
 */

#ifndef __BNO055_TASK_H__
#define __BNO055_TASK_H__

/* Fwk */
#include "fsl_os_abstraction.h"
#include "fsl_i2c_freertos.h"
#include "BNO055_driver/bno055.h"


/* Intervals authorized for the Bno055 task */
#define BNO055_MAXIMUM_MEASURE_INTERVAL_MS 10000
#define BNO055_MINIMUM_MEASURE_INTERVAL_MS 300

/* Number of maximum elements handled by the queue */
#define BNO055_MEASURE_QUEUE_SIZE	8

/* Queue to hold the data to be send to the task consumming it */
extern osaMsgQId_t gBno055NewMeasureQ;

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
/*! Bno055 Task Stack Size */
#define gBno055TaskStackSize_c (500)

/*! Bno55 Task OS Abstraction Priority */
#define gBno055TaskPriority_c  (OSA_PRIORITY_LOW)

/**
 * @brief Main task function, infinite loop capturing data and sending them 
 * through the gBno055NewMessageMeasureQ.
 * 
 * @param argument Parameters that can be sent to the task.
 */
void Bno055_Task(osaTaskParam_t argument);

/**
 * @brief Inititalize the Bno055 parameters and the data queue 
 * 
 * @param i2c_master_rtos_handle Handle to the I2C used by the Bno055
 * @return osaStatus_t osaStatus_Success if the initialization is a success,
 * otherwise osaStatus_error
 */
osaStatus_t Bno055_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle);

/**
 * @brief Set the interval beetween each measure of the Bno055
 * 
 * @param interval_ms Interval in milliseconds
 * @return osaStatus_t osaStatus_Success if the interval is valid. Otherwise
 * osaStatus_error. Check BNO055_MAXIMUM_MEASURE_INTERVAL_MS and 
 * BNO055_MINIMUM_MEASURE_INTERVAL_MS for valid parameters.
 */
osaStatus_t Bno055Task_SetMeasureInterval(uint32_t interval_ms);

/**
 * @brief Get the interval measure applied
 * 
 * @return uint32_t Interval in milliseconds
 */
uint32_t Bno055Task_GetMeasureInterval();

#endif /* __BNO055_TASK_H__ */
