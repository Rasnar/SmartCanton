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

/**
 * AT commands to communicate with the LoRa MCU
 */
#define CMD_AT 						(AtCommand){"AT\n", "OK", "AT_ERROR"}
#define CMD_MCURESET 				(AtCommand){"ATZ\n", "", ""}
#define CMD_GET_DEVEUI 				(AtCommand){"AT+DEUI=?\n", "OK", "AT_PARAM_ERROR"}
#define CMD_SET_APP_EUI 			(AtCommand){"AT+APPEUI=%s\n", "OK", "AT_PARAM_ERROR"}
#define CMD_SET_APP_KEY 			(AtCommand){"AT+APPKEY=%s\n", "OK", "AT_PARAM_ERROR"}
#define CMD_GET_APP_KEY				(AtCommand){"AT+APPKEY=?\n", "OK", "AT_PARAM_ERROR"
#define CMD_SET_CONFIRM_MODE 		(AtCommand){"AT+CFM=%s\n", "OK", "AT_PARAM_ERROR"} // 0 : unconfirmed, 1 : confirmed messages
#define CMD_SET_CONFIRM_STATUS 		(AtCommand){"AT+CFS=?\n", "OK"}
#define CMD_NETWORK_JOIN 			(AtCommand){"AT+JOIN\n", "OK", "AT_BUSY_ERROR"}
#define CMD_SET_NETWORK_JOIN_MODE 	(AtCommand){"AT+NJM=%s\n", "OK", "AT_PARAM_ERROR"} // 0 : ABP, 1 : OTAA
#define CMD_GET_NETWORK_JOIN_STATUS (AtCommand){"AT+NJS=?\n", "OK", " "}
#define CMD_SET_DUTYCYCLE_SETTINGS 	(AtCommand){"AT+DCS=%s\n", "OK"}  // 0 : ETSI duty cycle disable, 1 : enable
#define CMD_SEND_TEXTDATA 			(AtCommand){"AT+SEND=%s:%s\n", "OK"}
#define CMD_SEND_BINARYDATA 		(AtCommand){"AT+SENDB=%s:%s\n", "OK"}

/*
 * These values should be modified by the application as necessary.
 * They are used by the Lorawan Controller Task  initialization code from lorawan_controller_task.c.
 */
/*! Idle Task Stack Size */
#define gLorawanControllerTaskStackSize_c (1500)

/*! Lorawan Controller Task OS Abstraction Priority */
#define gLorawanControllerTaskPriority_c  (3)

void Lorawan_Controller_Task(osaTaskParam_t argument);
osaStatus_t LorawanController_TaskInit(void);

#endif /* __LORAWAN_CONTROLLER_AT_H__ */
