/**
 * @file    lorawan_controller.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include <lorawan_controller.h>
#include "fsl_lpuart.h"
#include <stdio.h>

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


#define DELAY_BEETWEEN_ATTEMPTS_MS 	100		// Try 100 Attempts
#define MAX_ATTEMPTS_TO_CONFIGURE 	100		// Try 100 Attempts



/**
 * Serial Manager parameters
 */
#define SERIAL_MANAGER_INTERFACE_TYPE gSerialMgrLpuart_c
#define SERIAL_MANAGER_INSTANCE_SERIAL 0 // If the LPUART0 is used, this value should be 0
#define SERIAL_MANAGER_LPUART_BAUDRATE gUARTBaudRate115200_c

/**
 * AT Commander configuration
 */
extern const AtCommanderPlatform AT_PLATFORM_I_CUBE_LRWAN;
AtCommanderConfig config;

#define AT_COMMANDER_MAX_REQUEST_LENGTH 128

/**
 * Serial Manager : pointer to a location where the interface Id will be stored
 */
uint8_t interfaceId;
void *pRxParam;
void *pTxParam;


bool lorawan_configuration_valid = false;

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

lorawanControllerStatus_t lorawan_controller_set_cmd(AtCommand cmd, ...){

	va_list args;
	va_start(args, cmd);

	char request[AT_COMMANDER_MAX_REQUEST_LENGTH];
	vsnprintf(request, AT_COMMANDER_MAX_REQUEST_LENGTH, cmd.request_format, args);

	va_end(args);

	cmd.request_format = request;
	if (!at_commander_set(&config, &cmd))
		return lorawanController_Error;
	else
		return lorawanController_Success;
}

inline int lorawan_controller_get_cmd(AtCommand cmd, char* response_buffer, int response_buffer_length) {
	return at_commander_get(&config,
			&cmd,
			response_buffer,
			response_buffer_length);
}

lorawanControllerStatus_t lorawan_controller_init_module(){
	char data[16];
	uint16_t bytesRead;

	lorawan_configuration_valid = false;

	/* RESET LoRa MCU to start with a clean state */
	lorawan_controller_set_cmd(CMD_MCURESET); // Reset LoRa MCU to be sure to have a new configuration
	OSA_TimeDelay(20); // Wait a bit to be sure that the LoRa MCU is reset

	/* Empty reception buffer with glitches from the reset */
	Serial_Read(interfaceId, (uint8_t*) data, sizeof(data), &bytesRead);

	/**
	 * Configuration LoRa Module with new configuration
	 */
	if (lorawan_controller_set_cmd(CMD_SET_APP_EUI, ORBIWISE_APP_EUI)
			!= lorawanController_Success)
		return lorawanController_Error;

	if (lorawan_controller_set_cmd(CMD_SET_APP_KEY, ORBIWISE_APP_KEY)
			!= lorawanController_Success)
		return lorawanController_Error;

	if (lorawan_controller_set_cmd(CMD_SET_CONFIRM_MODE, LORA_PACKETS_MODE)
			!= lorawanController_Success) // 0 : unconfirmed, 1 : confirmed messages
		return lorawanController_Error;

	if (lorawan_controller_set_cmd(CMD_SET_NETWORK_JOIN_MODE,
			LORA_NETWORK_JOIN_MODE) != lorawanController_Success) // 0 : ABP, 1 : OTAA
		return lorawanController_Error;

	if (lorawan_controller_set_cmd(CMD_SET_DUTYCYCLE_SETTINGS,
			LORA_DUTYCYCLE_SETTINGS) != lorawanController_Success) // 0 : ETSI duty cycle disable, 1 : enable
		return lorawanController_Error;

	/* Join network as configured before */
	if (lorawan_controller_set_cmd(CMD_NETWORK_JOIN)
			!= lorawanController_Success)
		return lorawanController_Error;

	uint8_t nb_attempts = 0;

	/* Waiting to receive a join Accept from the server */
	do {
		bytesRead = lorawan_controller_get_cmd(CMD_GET_NETWORK_JOIN_STATUS, data, sizeof(data));
		OSA_TimeDelay(100);

		if((nb_attempts++) > MAX_ATTEMPTS_TO_CONFIGURE)
			return lorawanController_Error;
	} while (strcmp(data, LORA_NETWORK_JOINED_STATUS) != 0);

	lorawan_configuration_valid = true;

	return lorawanController_Success;
}

lorawanControllerStatus_t lorawan_controller_get_configuration_validity(){
	if(lorawan_configuration_valid == true) {
		return lorawanController_Success;
	} else {
		return lorawanController_Error;
	}
}

osaStatus_t lorawan_controller_init(void)
{
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

	/* AT Commander configuration */
	config.platform = AT_PLATFORM_I_CUBE_LRWAN;
	config.write_function = at_commander_write_byte;
	config.read_function = at_commander_read_byte;
	config.delay_function = at_commander_delayms;
	config.log_function = NULL;
	config.connected = true;

    return osaStatus_Success;
}


