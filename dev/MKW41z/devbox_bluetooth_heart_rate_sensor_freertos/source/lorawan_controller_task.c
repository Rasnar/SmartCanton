
/**
 * @file    lorawan_controller_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include "lorawan_controller_task.h"


#define LORA_DELAY_BEETWEEN_PACKETS_MS	300000

OSA_TASK_DEFINE( Lorawan_Controller_Task, gLorawanControllerTaskPriority_c, 1, gLorawanControllerTaskStackSize_c, FALSE );
osaTaskId_t gLorawanControllerTaskId = 0;

/**
 *
 */
void Lorawan_Controller_Task(osaTaskParam_t argument)
{
	lorawan_controller_init_module();

	/**
	 * Send data every DELAY_BEETWEEN_LORA_PACKETS_MS
	 */
    while(1)
    {
    	lorawan_controller_set_cmd(CMD_SEND_BINARYDATA, "1", "00823D02C07741005D2E8B");

    	OSA_TimeDelay(LORA_DELAY_BEETWEEN_PACKETS_MS);
    }
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

    /* Task creation */
    gLorawanControllerTaskId = OSA_TaskCreate(OSA_TASK(Lorawan_Controller_Task), NULL);

    if( NULL == gLorawanControllerTaskId )
    {
        panic(0,0,0,0);
        return osaStatus_Error;
    }

    return osaStatus_Success;
}


