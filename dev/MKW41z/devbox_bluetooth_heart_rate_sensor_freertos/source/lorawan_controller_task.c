
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
 * LoRa authentication app keys and EUI
 */
#define ORBIWISE_APP_EUI 	"70:b3:d5:7e:f0:00:1a:f7"
#define ORBIWISE_APP_KEY	"42:55:ac:76:2e:c1:d4:91:8a:04:e5:94:b2:55:80:40"

#define TTN_APP_EUI			"70:B3:D5:7E:D0:00:80:0D"
#define TTN_APP_KEY 		"42:55:ac:76:2e:c1:d4:91:8a:04:e5:94:b2:55:80:40"

#define LORA_SEND_PACKET_PORT		"1"		// Define port to use in Back-End: from 1 to 223
#define LORA_PACKETS_MODE			"1" 	// 0 : Unconfirmed, 1 : Confirmed messages
#define LORA_NETWORK_JOIN_MODE		"1" 	// 0 : ABP, 1 : OTAA
#define LORA_DUTYCYCLE_SETTINGS		"0" 	// 0 : ETSI duty cycle disable, 1 : Enable
#define LORA_NETWORK_JOINED_STATUS	"1"		// 0 : Network not yet joined, 1 : Network joined
#define LORA_DELAY_BEETWEEN_PACKETS_MS	300000

/**
 * Serial Manager parameters
 */
#define SERIAL_MANAGER_INTERFACE_TYPE gSerialMgrLpuart_c
#define SERIAL_MANAGER_INSTANCE_SERIAL 0 // If the LPUART0 is used, this value should be 0
#define SERIAL_MANAGER_LPUART_BAUDRATE gUARTBaudRate115200_c

/**
 * Serial Manager : pointer to a location where the interface Id will be stored
 */
uint8_t interfaceId;
void *pRxParam;
void *pTxParam;

/**
 * Callback called by the ATCommander library to write bytes
 */
void at_commander_write_byte(void* device, uint8_t byte) {
	Serial_SyncWrite(interfaceId, &byte, sizeof(byte));
}

/**
 * Function called by the AT Commander to read one byte from the Queue
 */
int at_commander_read_byte(void* device) {
	uint8_t data;
	uint16_t bytesRead;

	Serial_RxBufferByteCount(interfaceId, &bytesRead);

	Serial_Read(interfaceId, &data, sizeof(data), &bytesRead);

	if (bytesRead == 0) {
		return -1;
	}
	else
	{
		return data;
	}
}

/**
 * AT Commander function to wait from device to have a response
 */
void at_commander_delayms(long unsigned int delayInMs) {
	OSA_TimeDelay(delayInMs);
}

/**
 *
 */
void Lorawan_Controller_Task(osaTaskParam_t argument)
{
	char data[16];
	uint16_t bytesRead;
	bool rsp_status;
	(void)(rsp_status); // Remove unused warning

	/* AT Commander configuration */
	AtCommanderConfig config = {AT_PLATFORM_I_CUBE_LRWAN};
	config.write_function = at_commander_write_byte;
	config.read_function = at_commander_read_byte;
	config.delay_function = at_commander_delayms;
	config.log_function = NULL;
	config.connected = true;

	/* AT Command to be sent */
	AtCommand cmd;

	/* RESET LoRa MCU to start with a clean state */
	cmd = CMD_MCURESET;
	rsp_status = at_commander_set(&config, &cmd); // Reset LoRa MCU to be sure to have a new configuration
	OSA_TimeDelay(20); // Wait a bit to be sure that the LoRa MCU is reset

	/* Empty reception buffer with glitches from the reset */
	Serial_Read(interfaceId, (uint8_t*)data, sizeof(data), &bytesRead);

	/**
	 * Configuration LoRa Module
	 */
	cmd = CMD_SET_APP_EUI;
	rsp_status = at_commander_set(&config, &cmd,  ORBIWISE_APP_EUI);

	cmd = CMD_SET_APP_KEY;
	rsp_status = at_commander_set(&config, &cmd,  ORBIWISE_APP_KEY);

	cmd = CMD_SET_CONFIRM_MODE;
	rsp_status = at_commander_set(&config, &cmd,  LORA_PACKETS_MODE); // 0 : unconfirmed, 1 : confirmed messages

	cmd = CMD_SET_NETWORK_JOIN_MODE;
	rsp_status = at_commander_set(&config, &cmd,  LORA_NETWORK_JOIN_MODE); // 0 : ABP, 1 : OTAA

	cmd = CMD_SET_DUTYCYCLE_SETTINGS;
	rsp_status = at_commander_set(&config, &cmd,  LORA_DUTYCYCLE_SETTINGS); // 0 : ETSI duty cycle disable, 1 : enable

	/* Join network as configured before */
	cmd = CMD_NETWORK_JOIN;
	rsp_status = at_commander_set(&config, &cmd);

	/* Waiting to receive a join Accept from the server */
	cmd = CMD_GET_NETWORK_JOIN_STATUS;
	do {
		bytesRead = at_commander_get(&config, &cmd, data, sizeof(data));
	} while ((bytesRead != 1) && (strcmp(data, LORA_NETWORK_JOINED_STATUS)) == 0);

	/**
	 * Send data every DELAY_BEETWEEN_LORA_PACKETS_MS
	 */
	cmd = CMD_SEND_BINARYDATA;
    while(1)
    {
    	rsp_status = at_commander_set(&config, &cmd, LORA_SEND_PACKET_PORT, "00823D02C07741005D2E8B");

    	OSA_TimeDelay(LORA_DELAY_BEETWEEN_PACKETS_MS);
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

    return osaStatus_Success;
}


