/**
 * @file    lorawan_controller_task.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Define the lorawan controller task. Functions to init and manage the task. 
 */

#include "lorawan_controller_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "string_utils.h"

/* */
#define mDelayPacketSentsSeconds				300		/* s */

#define mConfirmStatusReadDelayBeetweenRetry	250 	/* ms */
#define mConfirmStatusReadNumberOfAttempts		8

#define mResendFrameDelayBeetweenRetry			10000 	/* ms */
#define mResendFrameNumberOfAttempts			50

/* Define the task with the given parameters */
OSA_TASK_DEFINE(Lorawan_Controller_Task, gLorawanControllerTaskPriority_c, 1,
		gLorawanControllerTaskStackSize_c, FALSE);

/* Holding the current Task id */
osaTaskId_t gLorawanControllerTaskId = 0;

/* Event to be used when a task when to send data or init the lora module */
osaEventId_t gLoRaControllerEvent;

/* Queue to hold the data to be send to the LoRaWAN module */
osaMsgQId_t gLorawanCtrlSendNewMsgQ;

/* Queue to hold the data received from the the LoRaWAN module */
osaMsgQId_t gLorawanCtrlReceiveNewMsgQ;

/**
 * Main LoRaWan controller task
 */
void Lorawan_Controller_Task(osaTaskParam_t argument)
{
	/* Start by configuring the LoRa Module */
	OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);

	/* Hold the current event read from the gLoRaControllerEvent */
	osaEventFlags_t event;

	/* Buffer to store the data from the module */
	char dataStr[256];

	/* Variable to store the length of a byte array when converting data from dataStr */
	uint16_t bytesRead;

	/* Pointer to hold the data received from the main task */
	lorawanControllerDataToSend_t* lorawanDataToSend;

	/* Pointer to hold the data to send to the main task */
	lorawanControllerDataReceived_t* lorawanDataReceived;

	/**
	 * Handle the LoRaWAN controller
	 */
	while (1)
	{

		OSA_EventWait(gLoRaControllerEvent, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &event);


		/**
		 * This event force a new config from the configuration that is stored in local
		 */
		if (event & gLoRaCtrlTaskEvtConfigure_c)
		{
			Led1Flashing();
			lorawanControllerStatus_t status = lorawan_controller_init_module();

			/* The configuration is successful because we joined a new network */
			if (status == lorawanController_Success)
			{
				/* Notify the main app that a the LoRaWAN module is ready to be used */
				OSA_EventSet(gDevBoxAppEvent, gDevBoxTaskEvtNewLoRaWANConfig_c);
				StopLed1Flashing();
				Led1On();
			}
			/* The configuration is corrupted, load the default configuration */
			else if (status  == lorawanController_Error_Invalid_Configuration){
				lorawan_controller_apply_default_configuration();
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
			}
			else
			{
				/* If the configuration is unsuccessful, try again to join the network */
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
			}
		}

		/**
		 * This event force a new config from the configuration that is stored inside the module
		 * (eg. when someone write new parameters from a bluetooth service)
		 */
		if (event & gLoRaCtrlTaskEvtConfigureFromModuleConfig_c)
		{
			Led1Flashing();
			/* Read the current configuration stored inside the module */
			if (lorawan_controller_read_module_configuration() == lorawanController_Success)
			{
				/* Restart a new module configuration sequence with the config read before */
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
			}
			else
			{
				StopLed1Flashing();
			}
		}

		/* Event to send a new message on the network */
		if (event & gLoRaCtrlTaskEvtNewMsgToSend_c)
		{
			/* Only send the message if the LoRaWAN configuration is valid */
			if (lorawan_controller_get_configuration_validity() == lorawanController_Success)
			{

				/* Retrieve all messages from the queue */
				while (OSA_MsgQGet(gLorawanCtrlSendNewMsgQ, &lorawanDataToSend, 0) == osaStatus_Success)
				{
					uint8_t nbAttempts = 0;
					int delayReceiveWindow2 = 0;

					/* Convert byte array to a string of hex */
					convertBytesArrayToHexString((lorawanDataToSend->data),
							lorawanDataToSend->dataLength,
							dataStr);

					vPortFree(lorawanDataToSend);

					/* Send data to the LoRaWAN module */
					while (lorawan_controller_set_cmd(CMD_SEND_BINARYDATA,
							LORAWAN_CONTROLLER_DATA_DEFAULT_PORT, dataStr)
							== lorawanController_Error)
					{
						/* This error is when the last frame hasn't been proceeded correctly yet.
						 * Try again one x ms */
						OSA_TimeDelay(mResendFrameDelayBeetweenRetry);

						if ((nbAttempts++) > mResendFrameNumberOfAttempts)
							break;
					}

					/* Number maximum of attempt reached for this frame, discard content and try
					 * with a new one
					 * TODO : We can maybe inform the main APP through an error of this error.
					 */
					if ((nbAttempts++) > mResendFrameNumberOfAttempts)
						continue;

					/* If the message is a confirmed one, wait for the confirmation */
					if (lorawan_controller_get_stored_configuration().confirmMode[0] == '1')
					{
						nbAttempts = 0;
						do
						{

							bytesRead = lorawan_controller_get_cmd(CMD_GET_CONFIRM_STATUS,
									dataStr, sizeof(dataStr));

							if(dataStr[0] == '1'){
								break;
							}

							/* Wait again x ms for the confirm status */
							OSA_TimeDelay(mConfirmStatusReadDelayBeetweenRetry);

							if ((nbAttempts++) > mConfirmStatusReadNumberOfAttempts)
								break;
						} while (dataStr[0] != '1');
					}

					/* Read the delay configured for the receive window 2 */
					bytesRead = lorawan_controller_get_cmd(CMD_DELAY_RECEIVE_WINDOW_2,
							dataStr, sizeof(dataStr));

					delayReceiveWindow2 = convertIntStringToInt(dataStr);

					/* Wait the end of the receive window to check if data have been received */
					if(delayReceiveWindow2 > 0) {
						OSA_TimeDelay(delayReceiveWindow2);
					}

					/* Read from the module Downlink data buffer */
					bytesRead = lorawan_controller_get_cmd(CMD_LAST_RECEIVED_BIN_DATA,
							dataStr, sizeof(dataStr));

					/* Data are received with the following format : '<port>:<new_data>'
					 * Check if there is any data after the ':'. If that is the case,
					 * it means that new data are available. Otherwise, just ignore it.  */
					uint8_t idxSeparator = strchr(dataStr, ':') - dataStr;
					if (bytesRead > (idxSeparator + 1))
					{

						lorawanDataReceived = pvPortMalloc(sizeof(lorawanControllerDataReceived_t));

						/* Extract the data from the module Downlink data */
						lorawanDataReceived->dataLength = convertHexStringToBytesArray(
								dataStr + idxSeparator + 1,
								lorawanDataReceived->data,
								sizeof(lorawanDataReceived->data));

						/* Add end of string where the ':' is located to be able to retrieve
						 * the port number */
						dataStr[idxSeparator] = '\0';

						/* Extract the port from the module Downlink data */
						lorawanDataReceived->port = convertIntStringToInt(dataStr);

						/* Add message to the Queue */
						if(OSA_MsgQPut(gLorawanCtrlReceiveNewMsgQ, &lorawanDataReceived)
								== osaStatus_Success) {
							/* Only notify main task if the message can be added successfully to the Queue */
							OSA_EventSet(gDevBoxAppEvent, gDevBoxEvtNewLoRaDataReceived_c);
						} else {
							/* Otherwise, free the reserved memory */
							vPortFree(lorawanDataReceived);
						}
					}
				}
			}
			else
			{
				/* Since the controller isn't in a valid state -> force a new configuration */
				// OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);

				/* Resend a new event to retry the message */
				OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);
			}
		}
	}
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

	/* Create Queue to send message through the LoRaWAN module */
	gLorawanCtrlSendNewMsgQ = OSA_MsgQCreate(LORAWAN_CTRL_TASK_QUEUE_SIZE);
	if ( NULL == gLorawanCtrlSendNewMsgQ)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Create Queue to receive message through the LoRaWAN module */
	gLorawanCtrlReceiveNewMsgQ = OSA_MsgQCreate(LORAWAN_CTRL_TASK_QUEUE_SIZE);
	if ( NULL == gLorawanCtrlReceiveNewMsgQ)
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

