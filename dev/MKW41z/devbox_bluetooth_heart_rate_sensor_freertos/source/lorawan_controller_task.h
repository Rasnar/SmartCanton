/**
 * @file    lorawan_controller_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#ifndef __LORAWAN_CONTROLLER_AT_H__
#define __LORAWAN_CONTROLLER_AT_H__

/* KSDK */
#include "board.h"

/* Drv */
#include "LED.h"
#include "Keyboard.h"

/* Fwk */
#include "fsl_os_abstraction.h"
#include "MemManager.h"
#include "TimersManager.h"
#include "RNG_Interface.h"
#include "Messaging.h"
#include "Flash_Adapter.h"
#include "SecLib.h"
#include "Panic.h"

#include "lorawan_at.h"


/*
 * These values should be modified by the application as necessary.
 * They are used by the Lorawan Controller Task  initialization code from lorawan_controller_task.c.
 */
/*! Idle Task Stack Size */
#ifndef gLorawanControllerTaskStackSize_c
#define gLorawanControllerTaskStackSize_c (500)
#endif

/*! Lorawan Controller Task OS Abstraction Priority */
#ifndef gLorawanControllerTaskPriority_c
#define gLorawanControllerTaskPriority_c  (15)
#endif

void Lorawan_Controller_Task(osaTaskParam_t argument);
osaStatus_t LorawanController_TaskInit(void);

#endif /* __LORAWAN_CONTROLLER_AT_H__ */
