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
#include "fsl_flash.h"

/**
 * LoRa authentication app keys and EUI
 */
#define ORBIWISE_APP_EUI 	"70:b3:d5:07:80:00:00:04"
#define ORBIWISE_APP_KEY	"f1:fb:27:1f:3f:f2:d7:3b:c1:f5:a1:45:7c:56:c0:00"

#define TTN_APP_EUI			"70:B3:D5:7E:D0:00:8E:8C"
#define TTN_APP_KEY 		"2B:62:B2:6C:09:52:27:36:8B:07:7D:D3:54:81:75:33"

#define LORA_DEFAULT_CONFIG_APP_EUI			TTN_APP_EUI
#define LORA_DEFAULT_CONFIG_APP_KEY			TTN_APP_KEY
#define LORA_DEFAULT_CONFIG_DEV_ADDR     	"00:00:00:00"
#define LORA_DEFAULT_CONFIG_NWK_SES_KEY     "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
#define LORA_DEFAULT_CONFIG_APP_SES_KEY     "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
#define LORA_DEFAULT_CONFIG_DEV_EUI			"00:00:00:00:00:00:00:00"

#define LORA_DEFAULT_CONFIG_CONFIRM_MODE			"1" 	// 0 : Unconfirmed, 1 : Confirmed messages
#define LORA_DEFAULT_CONFIG_NETWORK_JOIN_MODE		"1" 	// 0 : ABP, 1 : OTAA
#define LORA_DEFAULT_CONFIG_DUTYCYCLE_SETTINGS		"0" 	// 0 : ETSI duty cycle disable, 1 : Enable

#define LORA_SEND_PACKET_PORT		"1"		// Define port to use in Back-End: from 1 to 223
#define LORA_NETWORK_JOINED_STATUS	"1"		// 0 : Network not yet joined, 1 : Network joined

#define DELAY_BEETWEEN_CHECK_MS 	100
#define ATTEMPTS_TO_CHECK_NETWORK_STATUS 	200		// Try X Attempts

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
static AtCommanderConfig config;

#define AT_COMMANDER_MAX_REQUEST_LENGTH 256

/**
 * Serial Manager : pointer to a location where the interface Id will be stored
 */
static uint8_t interfaceId;

/* Mutex to lock the get and set commands */
osaMutexId_t cmdMutexId;

/**
 * Flash programmer
 */
/* Flash driver Structure */
static flash_config_t s_flashDriver;

uint32_t flashDestAdrss; /* Address of the target location */

static uint32_t pflashBlockBase = 0;
static uint32_t pflashTotalSize = 0;
static uint32_t pflashSectorSize = 0;

/**
 * Module configuration
 */
static bool lorawan_configuration_valid = false;
lorawanControllerConfiguration_t currentLoRaWanConfig;

lorawanControllerStatus_t lorawan_controller_read_module_configuration();
static lorawanControllerStatus_t lorawan_controller_flash_init(void);
static lorawanControllerConfiguration_t lorawan_controller_read_configuration_from_flash(void);
static lorawanControllerStatus_t lorawan_controller_write_configuration_to_flash(void);
void lorawan_controller_apply_default_configuration();
static uint16_t crc16(const uint8_t* data_p, uint32_t length);

/**
 * Callback called by the ATCommander library to write bytes
 */
void at_commander_write_byte(void* device, uint8_t byte)
{
	Serial_SyncWrite(interfaceId, &byte, sizeof(byte));
}

/**
 * Function called by the AT Commander to read one byte from the Queue
 */
int at_commander_read_byte(void* device)
{
	uint8_t data;
	uint16_t bytesRead;

	Serial_RxBufferByteCount(interfaceId, &bytesRead);

	Serial_Read(interfaceId, &data, sizeof(data), &bytesRead);

	if (bytesRead == 0)
	{
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
void at_commander_delayms(long unsigned int delayInMs)
{
	OSA_TimeDelay(delayInMs);
}

lorawanControllerStatus_t lorawan_controller_set_cmd(AtCommand cmd, ...)
{

	va_list args;
	va_start(args, cmd);

	char request[AT_COMMANDER_MAX_REQUEST_LENGTH];
	vsnprintf(request, AT_COMMANDER_MAX_REQUEST_LENGTH, cmd.request_format, args);

	va_end(args);

	cmd.request_format = request;

	OSA_MutexLock(cmdMutexId, 1000);
	if (!at_commander_set(&config, &cmd))
	{
		OSA_MutexUnlock(cmdMutexId);
		return lorawanController_Error;

	}
	else
	{
		OSA_MutexUnlock(cmdMutexId);
		return lorawanController_Success;
	}
}

inline int lorawan_controller_get_cmd(AtCommand cmd, char* response_buffer, int response_buffer_length)
{
	int status;

	OSA_MutexLock(cmdMutexId, 1000);
	status = at_commander_get(&config, &cmd, response_buffer, response_buffer_length);
	OSA_MutexUnlock(cmdMutexId);

	return status;
}

lorawanControllerStatus_t lorawan_controller_init_module()
{
	char data[16];
	uint16_t bytesRead;

	lorawan_configuration_valid = false;

	/* RESET LoRa MCU to start with a clean state */
	lorawan_controller_set_cmd(CMD_MCURESET);
	OSA_TimeDelay(20); // Wait a bit to be sure that the LoRa MCU is reset

	/* Empty reception buffer with glitches from the reset */
	Serial_Read(interfaceId, (uint8_t*) data, sizeof(data), &bytesRead);

	/**
	 * Configuration LoRa Module with new configuration
	 */
	if (lorawan_controller_set_cmd(CMD_SET_APP_EUI,
			currentLoRaWanConfig.appEui) != lorawanController_Success)
		return lorawanController_Error_Invalid_Configuration;

	if (lorawan_controller_set_cmd(CMD_SET_APP_KEY,
			currentLoRaWanConfig.appKey) != lorawanController_Success)
		return lorawanController_Error_Invalid_Configuration;

	if (lorawan_controller_set_cmd(CMD_SET_CONFIRM_MODE,
			currentLoRaWanConfig.confirmMode) != lorawanController_Success) // 0 : unconfirmed, 1 : confirmed messages
		return lorawanController_Error_Invalid_Configuration;

	if (lorawan_controller_set_cmd(CMD_SET_NETWORK_JOIN_MODE, currentLoRaWanConfig.networkJoinMode) != lorawanController_Success) // 0 : ABP, 1 : OTAA
		return lorawanController_Error;

	if (lorawan_controller_set_cmd(CMD_SET_DUTYCYCLE_SETTINGS,
			currentLoRaWanConfig.etsiDutyCycleEnable) != lorawanController_Success) // 0 : ETSI duty cycle disable, 1 : enable
		return lorawanController_Error_Invalid_Configuration;

	/* Join network as configured before */
	if (lorawan_controller_set_cmd(CMD_NETWORK_JOIN) != lorawanController_Success)
		return lorawanController_Error_Invalid_Configuration;

	uint8_t nbAttempts = 0;

	/* Waiting to receive a join Accept from the server */
	do
	{
		bytesRead = lorawan_controller_get_cmd(CMD_GET_NETWORK_JOIN_STATUS
		, data, sizeof(data));
		OSA_TimeDelay(DELAY_BEETWEEN_CHECK_MS);

		if ((nbAttempts++) > ATTEMPTS_TO_CHECK_NETWORK_STATUS)
			return lorawanController_Error;
	} while (strcmp(data, LORA_NETWORK_JOINED_STATUS) != 0);

	lorawan_configuration_valid = true;

	/* Read the configuration from the module to be sure that the one store is correct */
	lorawan_controller_read_module_configuration();

//	/* Save the configuration to flash */
//	if (lorawan_controller_write_configuration_to_flash() != lorawanController_Success)
//	{
//		return lorawanController_Error;
//	}

	return lorawanController_Success;
}

lorawanControllerStatus_t lorawan_controller_get_configuration_validity()
{
	if (lorawan_configuration_valid == true)
	{
		return lorawanController_Success;
	}
	else
	{
		return lorawanController_Error;
	}
}

osaStatus_t lorawan_controller_init(void)
{
	serialStatus_t status = Serial_InitInterface(&interfaceId, SERIAL_MANAGER_INTERFACE_TYPE,
	SERIAL_MANAGER_INSTANCE_SERIAL);
	if (gSerial_Success_c != status)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	status = Serial_SetBaudRate(interfaceId, SERIAL_MANAGER_LPUART_BAUDRATE);
	if (gSerial_Success_c != status)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	cmdMutexId = OSA_MutexCreate();
	if (NULL == cmdMutexId)
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

	/* Initilise the possibility to write to flash */
	lorawan_controller_flash_init();

	/* Read data from flash memory */
	lorawan_controller_read_configuration_from_flash();


	/* If the CRC calculated does not match the CRC read, it means that
	 * the data has been corrupted (eg. by turning off the board when the
	 * configuration was been written).
	 */
	if(currentLoRaWanConfig.crc == crc16((uint8_t*)&currentLoRaWanConfig,
			sizeof(currentLoRaWanConfig) - sizeof(currentLoRaWanConfig.crc)))
	{
		/* If the magic word is not correct it means that the configuration has been
		 * modified. This modification can only occur if it was done programmaticly.
		 * It means that the user want to restore the default confiugration but the
		 * old data stored was correct.
		 */
		if (currentLoRaWanConfig.magicWord != LORAWAN_CONTROLLER_MAGIC_WORD)
		{
			lorawan_controller_apply_default_configuration();
		}
	}
	else
	{
		/* Configuration corrupted, apply default configuration */
		lorawan_controller_apply_default_configuration();
	}

	return osaStatus_Success;
}

void lorawan_controller_apply_default_configuration()
{
	char data[32];
	uint16_t bytesRead;

	strcpy(currentLoRaWanConfig.appEui, LORA_DEFAULT_CONFIG_APP_EUI);
	strcpy(currentLoRaWanConfig.appKey, LORA_DEFAULT_CONFIG_APP_KEY);
	strcpy(currentLoRaWanConfig.devAddr, LORA_DEFAULT_CONFIG_DEV_ADDR);
	strcpy(currentLoRaWanConfig.nwkSessionKey, LORA_DEFAULT_CONFIG_NWK_SES_KEY);
	strcpy(currentLoRaWanConfig.appSessionKey, LORA_DEFAULT_CONFIG_APP_SES_KEY);
	strcpy(currentLoRaWanConfig.confirmMode, LORA_DEFAULT_CONFIG_CONFIRM_MODE);

	strcpy(currentLoRaWanConfig.etsiDutyCycleEnable, LORA_DEFAULT_CONFIG_DUTYCYCLE_SETTINGS);
	strcpy(currentLoRaWanConfig.networkJoinMode, LORA_DEFAULT_CONFIG_NETWORK_JOIN_MODE);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_DEVEUI
	, data, sizeof(data));
	if (bytesRead < 0)
	{
		return;
	}
	strcpy(currentLoRaWanConfig.devEui, data);

	currentLoRaWanConfig.magicWord = LORAWAN_CONTROLLER_MAGIC_WORD;

	/* Calculate the crc on all the data except the crc value itsef */
	currentLoRaWanConfig.crc = crc16((uint8_t*)&currentLoRaWanConfig,
			sizeof(currentLoRaWanConfig) - sizeof(currentLoRaWanConfig.crc));

}

lorawanControllerStatus_t lorawan_controller_read_module_configuration(void)
{
	char data[64];
	uint16_t bytesRead;

	bytesRead = lorawan_controller_get_cmd(CMD_GET_DEVEUI,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.devEui, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_APP_EUI,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.appEui, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_APP_KEY,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.appKey, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_DEV_ADDR,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.devAddr, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_NWK_SESSION_KEY,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.nwkSessionKey, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_APP_SESSION_KEY,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.appSessionKey, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_CONFIRM_MODE,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.confirmMode, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_NETWORK_JOIN_MODE,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.networkJoinMode, data);

	bytesRead = lorawan_controller_get_cmd(CMD_GET_DUTYCYCLE_SETTINGS,
			data, sizeof(data));
	if (bytesRead < 0)
	{
		return lorawanController_Error;
	}

	strcpy(currentLoRaWanConfig.etsiDutyCycleEnable, data);

	return lorawanController_Success;
}

lorawanControllerConfiguration_t lorawan_controller_get_current_configuration(void)
{
	return currentLoRaWanConfig;
}

static lorawanControllerConfiguration_t lorawan_controller_read_configuration_from_flash(void)
{

	memcpy((uint32_t *) &currentLoRaWanConfig, (uint32_t *) flashDestAdrss,
			sizeof(lorawanControllerConfiguration_t));

	return currentLoRaWanConfig;
}

static lorawanControllerStatus_t lorawan_controller_write_configuration_to_flash(void)
{
	uint32_t failAddr, failDat;

	// Disable hardware interrupts while accessing memory flash
	OSA_DisableIRQGlobal();

	// Equivalent to taskENTER_CRITICAL in freertos. It's means to disable all context
	// switch in this code.
	OSA_InterruptDisable();

	status_t result = FLASH_Erase(&s_flashDriver, flashDestAdrss, pflashSectorSize, kFLASH_ApiEraseKey);
	if (kStatus_FLASH_Success != result)
	{
		OSA_InterruptEnable();
		OSA_EnableIRQGlobal();
		return lorawanController_Error;
	}

	/* Verify sector if it's been erased. */
	result = FLASH_VerifyErase(&s_flashDriver, flashDestAdrss, pflashSectorSize, kFLASH_MarginValueUser);
	if (kStatus_FLASH_Success != result)
	{
		OSA_InterruptEnable();
		OSA_EnableIRQGlobal();
		return lorawanController_Error;
	}

	/* Program user buffer into flash*/
	result = FLASH_Program(&s_flashDriver, flashDestAdrss, (uint32_t *) &currentLoRaWanConfig,
			sizeof(lorawanControllerConfiguration_t));
	if (kStatus_FLASH_Success != result)
	{
		OSA_InterruptEnable();
		OSA_EnableIRQGlobal();
		return lorawanController_Error;
	}

	/* Verify programming by Program Check command with user margin levels */
	result = FLASH_VerifyProgram(&s_flashDriver, flashDestAdrss, sizeof(currentLoRaWanConfig),
			(uint32_t *) &currentLoRaWanConfig, kFLASH_MarginValueUser, &failAddr, &failDat);
	if (kStatus_FLASH_Success != result)
	{
		OSA_InterruptEnable();
		OSA_EnableIRQGlobal();
		return lorawanController_Error;
	}

	OSA_InterruptEnable();
	OSA_EnableIRQGlobal();
	return lorawanController_Success;
}

static lorawanControllerStatus_t lorawan_controller_flash_init(void)
{
	/* Clean up Flash driver Structure*/
	memset(&s_flashDriver, 0, sizeof(flash_config_t));

	status_t result = FLASH_Init(&s_flashDriver);
	if (kStatus_FLASH_Success != result)
	{
		return lorawanController_Error;
	}

	/* Get flash properties*/
	FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyPflashBlockBaseAddr, &pflashBlockBase);
	FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyPflashTotalSize, &pflashTotalSize);
	FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyPflashSectorSize, &pflashSectorSize);

	flash_security_state_t securityStatus = kFLASH_SecurityStateNotSecure; /* Return protection status */

	/* Check security status. */
	result = FLASH_GetSecurityState(&s_flashDriver, &securityStatus);
	if (kStatus_FLASH_Success != result)
	{
		return lorawanController_Error;
	}

	/* Print security status. */
	switch (securityStatus)
	{
	case kFLASH_SecurityStateNotSecure:
		break;
	case kFLASH_SecurityStateBackdoorEnabled:
		return lorawanController_Error;
		break;
	case kFLASH_SecurityStateBackdoorDisabled:
		return lorawanController_Error;
		break;
	default:
		break;
	}

	flashDestAdrss = pflashBlockBase + (pflashTotalSize - pflashSectorSize);

	return lorawanController_Success;
}

static uint16_t crc16(const uint8_t* data_p, uint32_t length)
{
	unsigned char x;
	unsigned short crc = 0xFFFF;

	while (length--)
	{
		x = crc >> 8 ^ *data_p++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((unsigned short) (x << 12)) ^ ((unsigned short) (x << 5)) ^ ((unsigned short) x);
	}
	return crc;
}
