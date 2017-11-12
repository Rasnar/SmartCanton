
/**
 * @file    lorawan_controller_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include "lorawan_controller_task.h"
#include "LED.h"


#define mDelayPacketSentsSeconds	300

OSA_TASK_DEFINE( Lorawan_Controller_Task, gLorawanControllerTaskPriority_c, 1, gLorawanControllerTaskStackSize_c, FALSE );
osaTaskId_t gLorawanControllerTaskId = 0;

static tmrTimerID_t mLoraSendId;
static osaEventId_t  mLoRaControllerEvent;

/* Task Events */
#define gLoRaCtrlTaskEvtNewMsgToSend_c       	(1 << 0)
#define gLoRaCtrlTaskEvtConfigure_c			(1 << 1)

//static anchor_t gLoRaCtrlTaskNewLoRaMsg;

static void TimerLoRaSendCallback(void * pParam);

/**
 *
 */
void Lorawan_Controller_Task(osaTaskParam_t argument)
{

	/* Allocate application timers */
	mLoraSendId = TMR_AllocateTimer();

	TMR_StartLowPowerTimer(mLoraSendId, gTmrLowPowerIntervalMillisTimer_c,
	                       TmrSeconds(mDelayPacketSentsSeconds), TimerLoRaSendCallback, NULL);

	// Start by configuring the LoRa Module
	OSA_EventSet(mLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);

	// Once module configured, send the first message
	OSA_EventSet(mLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);

    osaEventFlags_t event;

	/**
	 * Send data every DELAY_BEETWEEN_LORA_PACKETS_MS
	 */
    while(1)
    {

    	OSA_EventWait(mLoRaControllerEvent, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &event);

        if (event & gLoRaCtrlTaskEvtConfigure_c) {
        	Led4Flashing();
        	if(lorawan_controller_init_module() != lorawanController_Success){
        		OSA_EventSet(mLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
        	}
        	StopLed4Flashing();
        	Led4On();
    	}

        if (event & gLoRaCtrlTaskEvtNewMsgToSend_c) {
        	if(lorawan_controller_get_configuration_validity() == lorawanController_Success){
            	lorawan_controller_set_cmd(CMD_SEND_BINARYDATA, "1", "00823D02C07741005D2E8B");
        	} else {
        		// Since the controller isn't in a valid state -> force a new configuration
        		OSA_EventSet(mLoRaControllerEvent, gLoRaCtrlTaskEvtConfigure_c);
        		// Resend a new event to send a message
        		OSA_EventSet(mLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);
        	}
        }
    }
}

static void TimerLoRaSendCallback(void * pParam) {

	OSA_EventSet(mLoRaControllerEvent, gLoRaCtrlTaskEvtNewMsgToSend_c);

}


osaStatus_t LorawanController_TaskInit(void)
{
    if(gLorawanControllerTaskId)
    {
        return osaStatus_Error;
    }

    if(lorawan_controller_init() != osaStatus_Success)
    {
		panic(0,0,0,0);
		return osaStatus_Error;
	}

    /* Create application event */
    mLoRaControllerEvent = OSA_EventCreate(TRUE);
	if( NULL == mLoRaControllerEvent )
	{
		panic(0,0,0,0);
        return osaStatus_Error;
	}

    /* Task creation */
    gLorawanControllerTaskId = OSA_TaskCreate(OSA_TASK(Lorawan_Controller_Task), NULL);

    if( NULL == gLorawanControllerTaskId )
    {
        panic(0,0,0,0);
        return osaStatus_Error;
    }

    return osaStatus_Success;
}


