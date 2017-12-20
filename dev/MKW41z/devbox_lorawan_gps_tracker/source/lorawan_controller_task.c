/**
 * @file    lorawan_controller_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include "lorawan_controller_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "string_utils.h"

#define mDelayPacketSentsSeconds	300

OSA_TASK_DEFINE(Lorawan_Controller_Task, gLorawanControllerTaskPriority_c, 1,
		gLorawanControllerTaskStackSize_c, FALSE);
osaTaskId_t gLorawanControllerTaskId = 0;

static tmrTimerID_t mLoraSendId;
osaEventId_t gLoRaControllerEvent;
osaMsgQId_t gLorawanCtrlSendNewMessageQ;

static void TimerLoRaSendCallback(void * pParam);

/**
 * Main LoRaWan controller task
 */
void Lorawan_Controller_Task(osaTaskParam_t argument)
{

	/* Allocate application timers */
	mLoraSendId = TMR_AllocateTimer();

	TMR_StartLowPowerTimer(mLoraSendId, gTmrLowPowerIntervalMillisTimer_c,
			TmrSeconds(mDelayPacketSentsSeconds), TimerLoRaSendCallback, NULL);

	// Start by configuring the LoRa Module
	OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);

//	// Once module configured, send the first message
//	OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);

	osaEventFlags_t event;
	lorawanControllerData_t* lorawanData;
	char dataStr[128];

	/**
	 * Send data every DELAY_BEETWEEN_LORA_PACKETS_MS
	 */
	while (1)
	{

		OSA_EventWait(gLoRaControllerEvent, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &event);

		if (event & gLoRaCtrlTaskEvtConfigure_c)
		{
			Led4Flashing();
			if (lorawan_controller_init_module() == lorawanController_Success)
			{
				OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewLoRaWANConfig_c);
				StopLed4Flashing();
				Led4On();
			}
			else
			{
				// If the configuration is unsuccessful, try again to join the network
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
			}
		}

		if (event & gLoRaCtrlTaskEvtConfigureFromModuleConfig_c)
		{
			Led4Flashing();
			if (lorawan_controller_read_module_configuration() == lorawanController_Success)
			{
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
			}
			else
			{
				StopLed4Flashing();
			}
		}

		if (event & gLoRaCtrlTaskEvtNewMsgToSend_c)
		{
			if (lorawan_controller_get_configuration_validity() == lorawanController_Success)
			{

				/* Retrieve data pointer */
				if (OSA_MsgQGet(gLorawanCtrlSendNewMessageQ, &lorawanData, 10) == osaStatus_Success)
				{

					/* Convert byte array to a string of hex */
					convertBytesArrayToHexString((lorawanData->data),
							lorawanData->dataSize,
							dataStr);

					vPortFree(lorawanData);

					/* Send data to the LoRaWAN module */
					lorawan_controller_set_cmd(CMD_SEND_BINARYDATA,
							"1", // PORT 1 by default
							dataStr);
				}
			}
			else
			{
//				// Since the controller isn't in a valid state -> force a new configuration
//				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
				// Resend a new event to retry the message
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);
			}
		}
	}
}

static void TimerLoRaSendCallback(void * pParam)
{

	//OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);

}

osaStatus_t LorawanController_TaskInit(void)
{
	if (gLorawanControllerTaskId)
	{
		return osaStatus_Error;
	}

	if (lorawan_controller_init() != osaStatus_Success)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Create application event */
	gLoRaControllerEvent = OSA_EventCreate(TRUE);
	if ( NULL == gLoRaControllerEvent)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Create application Queue */
	gLorawanCtrlSendNewMessageQ = OSA_MsgQCreate(LORAWAN_CTRL_TASK_QUEUE_SIZE);
	if ( NULL == gLorawanCtrlSendNewMessageQ)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Task creation */
	gLorawanControllerTaskId = OSA_TaskCreate(OSA_TASK(Lorawan_Controller_Task), NULL);

	if ( NULL == gLorawanControllerTaskId)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	return osaStatus_Success;
}

