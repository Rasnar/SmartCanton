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

/*
 * These values should be modified by the application as necessary.
 * They are used by the BNO055 Task  initialization code from bno55_task.c.
 */
/*! Idle Task Stack Size */
#define gBno055TaskStackSize_c (1500)

/*! Bno55 Task OS Abstraction Priority */
#define gBno055TaskPriority_c  (3)

void Bno055_Task(osaTaskParam_t argument);
osaStatus_t Bno055_TaskInit(void);

#endif /* __BNO055_TASK_H__ */
