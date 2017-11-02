
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

extern const AtCommanderPlatform AT_PLATFORM_I_CUBE_LRWAN;

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



static void serial_manager_rx_callback(void* param){
	param = param;
	uint8_t dataRead;
	uint16_t bytesRead;

	Serial_Read(interfaceId, &dataRead, sizeof(dataRead), &bytesRead);
}

void serial_manager_tx_callback(void* param){
	param = param;
}

void at_commander_configureUart(void* device, int baud) {
    //status = Serial_SetBaudRate(interfaceId, gUARTBaudRate9600_c);
}

void at_commander_write_byte(void* device, uint8_t byte) {
	Serial_SyncWrite(interfaceId, &byte, sizeof(byte));
}

int at_commander_read_byte(void* device) {
//    if(!QUEUE_EMPTY(uint8_t, &receive_queue)) {
//        return QUEUE_POP(uint8_t, &receive_queue);
//    }
    return -1;
}

void at_commander_delayms(long unsigned int delayInMs) {
	OSA_TimeDelay(delayInMs);
}

void Lorawan_Controller_Task(osaTaskParam_t argument)
{
	uint8_t dataWrite[] = "AT\n";

	AtCommanderConfig config = {AT_PLATFORM_I_CUBE_LRWAN};

	config.baud_rate_initializer = at_commander_configureUart;
	config.write_function = at_commander_write_byte;
	config.read_function = at_commander_read_byte;
	config.delay_function = at_commander_delayms;
	config.log_function = NULL;


	AtCommand command = {"AT", "\r\nOK\r\n", "\r\nAT_ERROR\r\n"};
	//at_commander_get(&config, command, response_buffer, response_buffer_length)

    while(1)
    {
    	at_commander_set(&config, &command, "\n");
    	Serial_SyncWrite(interfaceId, dataWrite, sizeof(dataWrite)-1);

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

    status = Serial_SetBaudRate(interfaceId, SERIAL_MANAGER_LPUART_BAUDRATE);
	if (gSerial_Success_c != status)
	{
		panic(0, 0, 0, 0);
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


