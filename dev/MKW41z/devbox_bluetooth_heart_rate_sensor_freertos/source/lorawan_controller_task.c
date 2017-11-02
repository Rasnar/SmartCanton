
/**
 * @file    lorawan_controller_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include "lorawan_controller_task.h"
#include "fsl_lpuart.h"

OSA_TASK_DEFINE( Lorawan_Controller_Task, gLorawanControllerTaskPriority_c, 1, gLorawanControllerTaskStackSize_c, FALSE );
osaTaskId_t gLorawanControllerTaskId = 0;


/**
 * Serial Manager parameters
 */
#define SERIAL_MANAGER_INTERFACE_TYPE gSerialMgrLpuart_c
#define SERIAL_MANAGER_INSTANCE_SERIAL 0 // If the LPUART0 is used, this value should be 0
#define SERIAL_MANAGER_LPUART_BAUDRATE gUARTBaudRate9600_c

/**
 * Serial Manager : pointer to a location where the interface Id will be stored
 */
uint8_t interfaceId;
void *pRxParam;
void *pTxParam;



void serial_manager_rx_callback(void* param){
	param = param;
}

void serial_manager_tx_callback(void* param){
	param = param;
}

void Lorawan_Controller_Task(osaTaskParam_t argument)
{
	uint8_t dataWrite[] = "AT\n";
	uint8_t dataRead[100];
	uint16_t bytesRead;

	OSA_TimeDelay(1000);
    OSA_TaskYield();

    while(1)
    {
    	Serial_SyncWrite(interfaceId, dataWrite, sizeof(dataWrite)-1);

    	Serial_Read(interfaceId, dataRead, sizeof(dataRead)-1, &bytesRead);

    	OSA_TimeDelay(1000);
    	OSA_TaskYield();
    }
}

osaStatus_t LorawanController_TaskInit(void)
{
    if(gLorawanControllerTaskId)
    {
        return osaStatus_Error;
    }

    /* Task creation */
    gLorawanControllerTaskId = OSA_TaskCreate(OSA_TASK(Lorawan_Controller_Task), NULL);

    if( NULL == gLorawanControllerTaskId )
    {
        panic(0,0,0,0);
        return osaStatus_Error;
    }

    serialStatus_t status = Serial_InitInterface(&interfaceId, SERIAL_MANAGER_INTERFACE_TYPE, SERIAL_MANAGER_INSTANCE_SERIAL);
    if( gSerial_Success_c != status )
	{
		panic(0,0,0,0);
		return osaStatus_Error;
	}

    status = Serial_SetBaudRate(interfaceId, gUARTBaudRate9600_c);
    if( gSerial_Success_c != status )
	{
		panic(0,0,0,0);
		return osaStatus_Error;
	}

    status = Serial_SetRxCallBack(interfaceId, serial_manager_rx_callback, pRxParam);
    if( gSerial_Success_c != status )
	{
		panic(0,0,0,0);
		return osaStatus_Error;
	}

    return osaStatus_Success;
}


