/**
 * @file    bme680_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the bme680 task. Functions to init 
 * and manage the task. 
 */

#ifndef __BME680_TASK_H__
#define __BME680_TASK_H__

/* Fwk */
#include "fsl_os_abstraction.h"
#include "fsl_i2c_freertos.h"

/* Number of maximum elements handled by the queue */
#define BME680_MEASURE_QUEUE_SIZE	8

/* Queue to hold the data to be send to the task consumming it */
extern osaMsgQId_t gBme680NewMessageMeasureQ;

typedef struct bme680Data_tag
{
	float iaq;
	uint8_t iaq_accuracy;
	float temperature;
	float humidity;
	float pressure;
	float raw_temperature;
	float raw_humidity;
	float gas;
} bme680Data_t;

/*
 * These values should be modified by the application as necessary.
 * They are used by the BME680 Task  initialization code from bme680_task.c.
 */
/* Bme680 Task Stack Size */
#define gBme680TaskStackSize_c (4096)

/* Bme680 Task OS Abstraction Priority */
#define gBme680TaskPriority_c  (OSA_PRIORITY_BELOW_NORMAL)

/**
 * @brief Main task function, infinite loop capturing data and sending them 
 * through the gBme680NewMessageMeasureQ.
 * 
 * @param argument Parameters that can be sent to the task.
 */
void Bme680_Task(osaTaskParam_t argument);

/**
 * @brief Inititalize the Bme680 parameters and the data queue.
 * 
 * @param i2c_master_rtos_handle Handle to the I2C used by the Bme680
 * @return osaStatus_t osaStatus_Success if the initialization is a success,
 * otherwise osaStatus_error
 */
osaStatus_t Bme680_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle);

#endif /* __BME680_TASK_H__ */
