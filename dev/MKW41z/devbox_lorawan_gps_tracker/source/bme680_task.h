/**
 * @file    bme680_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#ifndef __BME680_TASK_H__
#define __BME680_TASK_H__

/* Fwk */
#include "fsl_os_abstraction.h"
#include "fsl_i2c_freertos.h"

#define BME680_MEASURE_QUEUE_SIZE	8

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
/*! Idle Task Stack Size */
#define gBme680TaskStackSize_c (4096)

/*! Bno55 Task OS Abstraction Priority */
#define gBme680TaskPriority_c  (3)

void Bme680_Task(osaTaskParam_t argument);
osaStatus_t Bme680_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle);

#endif /* __BME680_TASK_H__ */
