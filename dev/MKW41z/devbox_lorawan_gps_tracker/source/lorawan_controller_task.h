/**
 * @file    lorawan_controller_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#ifndef __LORAWAN_CONTROLLER_TASK_H__
#define __LORAWAN_CONTROLLER_TASK_H__

/* Fwk */
#include "fsl_os_abstraction.h"
#include "MemManager.h"
#include "TimersManager.h"
#include "RNG_Interface.h"
#include "Messaging.h"
#include "Flash_Adapter.h"
#include "SecLib.h"
#include "Panic.h"

#include "lorawan_controller.h"

typedef struct lorawanControllerDataToSend_tag
{
	uint8_t data[128];
	uint8_t dataLength;
} lorawanControllerDataToSend_t;

typedef struct lorawanControllerDataReceived_tag
{
	uint8_t port;
	uint8_t data[128];
	uint8_t dataLength;
} lorawanControllerDataReceived_t;

/*
 * These values should be modified by the application as necessary.
 * They are used by the LoRaWAN Controller Task  initialization code from lorawan_controller_task.c.
 */
/*! Idle Task Stack Size */
#define gLorawanControllerTaskStackSize_c (2048)

/*! LoRaWAN Controller Task OS Abstraction Priority */
#define gLorawanControllerTaskPriority_c  (OSA_PRIORITY_ABOVE_NORMAL)

/* Task Events */
#define gLoRaCtrlTaskEvtNewMsgToSend_c       		(1 << 0)
#define gLoRaCtrlTaskEvtConfigure_c					(1 << 1)
#define gLoRaCtrlTaskEvtConfigureFromModuleConfig_c	(1 << 2)

extern osaEventId_t gLoRaControllerEvent;
extern osaMsgQId_t gLorawanCtrlSendNewMessageQ;
extern osaMsgQId_t gLorawanCtrlReceiveNewMessageQ;

/* This parameter should not exceed osNumberOfMessages defined in app_preinclude.h */
#define LORAWAN_CTRL_TASK_QUEUE_SIZE 8

void Lorawan_Controller_Task(osaTaskParam_t argument);
osaStatus_t LorawanController_TaskInit(void);

#endif /* __LORAWAN_CONTROLLER_TASK_H__ */
