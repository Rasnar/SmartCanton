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
osaMsgQId_t gLorawanCtrlReceiveNewMessageQ;

static void TimerCheckLoRaWANCallback(void * pParam);

/**
 * Main LoRaWan controller task
 */
void Lorawan_Controller_Task(osaTaskParam_t argument)
{

	/* Allocate application timers */
	mLoraSendId = TMR_AllocateTimer();

	TMR_StartLowPowerTimer(mLoraSendId, gTmrLowPowerIntervalMillisTimer_c,
			TmrSeconds(mDelayPacketSentsSeconds), TimerCheckLoRaWANCallback, NULL);

	// Start by configuring the LoRa Module
	OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);

	// Once module configured, send the first message
//	OSA_EventSet(gLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);

	osaEventFlags_t event;
	lorawanControllerDataToSend_t* lorawanDataToSend;
	char dataStr[128];

	// Data to be sent to the main task
	lorawanControllerDataReceived_t* lorawanDataReceived;

	/**
	 * Send data every DELAY_BEETWEEN_LORA_PACKETS_MS
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

			/* The configuration is successfull because we joined a new network */
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
				while (OSA_MsgQGet(gLorawanCtrlSendNewMessageQ, &lorawanDataToSend, 1) == osaStatus_Success)
				{
					char data[16];
					uint16_t bytesRead;
					uint8_t nbAttempts = 0;
					int delayReceiveWindow2 = 0;

					/* Convert byte array to a string of hex */
					convertBytesArrayToHexString((lorawanDataToSend->data),
							lorawanDataToSend->dataLength,
							dataStr);

					vPortFree(lorawanDataToSend);

					bytesRead = lorawan_controller_get_cmd(CMD_DELAY_RECEIVE_WINDOW_2,
														data, sizeof(data));

					/* Send data to the LoRaWAN module */
					while (lorawan_controller_set_cmd(CMD_SEND_BINARYDATA,
							LORAWAN_CONTROLLER_DATA_DEFAULT_PORT, dataStr)
							== lorawanController_Error)
					{
						/* Error when the last frame hasn't been proceeded correctly sent
						 * yet. Try again one second later*/
						OSA_TimeDelay(1000);
					}


					/* If the message is a confirmed one, wait for the confirmation */
					if (lorawan_controller_get_current_configuration().confirmMode[0] == '1')
					{
						do
						{
							OSA_TimeDelay(1000);

							bytesRead = lorawan_controller_get_cmd(CMD_GET_CONFIRM_STATUS
							, data, sizeof(data));

							if ((nbAttempts++) > 10)
								break;
						} while (data[0] != '1');
					}


					/* Wait the end of the receive window to check if data has been received*/
					OSA_TimeDelay(delayReceiveWindow2 + 1000);

					/* Read from the module Downlink data buffer */
					bytesRead = lorawan_controller_get_cmd(CMD_LAST_RECEIVED_BIN_DATA,
							data, sizeof(data));

					/* Data are received with the following format : '<port>:<new_data>'
					 * Check if there is any data after the ':'. If that is the case,
					 * it means that new data are available. Otherwise, just ignore it.  */
					uint8_t idxSeparator = strchr(data, ':') - data;
					if (bytesRead > (idxSeparator + 1))
					{

						lorawanDataReceived = pvPortMalloc(sizeof(lorawanControllerDataReceived_t));

						/* Extract the data from the module Downlink data */
						lorawanDataReceived->dataLength = convertHexStringToBytesArray(
								data + idxSeparator + 1,
								lorawanDataReceived->data,
								sizeof(lorawanDataReceived->data));

						/* Add end of string where the ':' is located to be able to retrieve
						 * the port number */
						data[idxSeparator] = '\0';

						/* Extract the port from the module Downlink data */
						lorawanDataReceived->port = convertIntStringToInt(data);

						/* Add message to the Queue */
						if(OSA_MsgQPut(gLorawanCtrlReceiveNewMessageQ, &lorawanDataReceived)
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

static void TimerCheckLoRaWANCallback(void * pParam)
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

	/* Create Queue to send message through the LoRaWAN module */
	gLorawanCtrlSendNewMessageQ = OSA_MsgQCreate(LORAWAN_CTRL_TASK_QUEUE_SIZE);
	if ( NULL == gLorawanCtrlSendNewMessageQ)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	/* Create Queue to receive message through the LoRaWAN module */
	gLorawanCtrlReceiveNewMessageQ = OSA_MsgQCreate(LORAWAN_CTRL_TASK_QUEUE_SIZE);
	if ( NULL == gLorawanCtrlReceiveNewMessageQ)
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

