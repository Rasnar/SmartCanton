/**
 * @file    lorawan_controller.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	This file contains all the functions to communication with the 
 * LoRaWAN module from the initialization to the command set/get. 
 */

#ifndef __LORAWAN_CONTROLLER_H__
#define __LORAWAN_CONTROLLER_H__

/* KSDK */
#include "board.h"

/* Fwk */
#include "fsl_os_abstraction.h"
#include "Panic.h"

#include "SerialManager.h"
#include "atcommander/atcommander.h"

/* Magic word used to store data in NVM.
 * This word can be used to know if the data has been modified or 
 * if the user want to force a new default config from the software 
 * point of view, this value can be changed.
 */
#define LORAWAN_CONTROLLER_MAGIC_WORD	0x1122

/**
 * @brief Structure defining the configuration that will be saved in flash.
 * Every time we restart, this configuration is read and reapplied to the 
 * LoRaWAN module.
 * 
 */
typedef struct lorawanControllerConfiguration_tag
{
	// Used to make the lorawanControllerConfiguration_tag a
	// multiple of 4 and to verify validity
	uint16_t crc;

	uint32_t magicWord;

	char appEui[24];
	char appKey[48];
	char devEui[24];

	char devAddr[12];
	char nwkSessionKey[48];
	char appSessionKey[48];

	char networkJoinMode[2];
	char confirmMode[2];
	char etsiDutyCycleEnable[2];
} lorawanControllerConfiguration_t;

typedef enum lorawanControllerStatus_tag
{
	lorawanController_Success = 0U, /*!< Success */
	lorawanController_Error = 1U, /*!< Failed */
	lorawanController_Error_Invalid_Configuration = 2U, /*!< The configuration is invalid and need to be corrected */
} lorawanControllerStatus_t;

/* Default port used to send data to the network */
#define LORAWAN_CONTROLLER_DATA_DEFAULT_PORT "1"

/**
 * AT commands to communicate with the LoRa MCU
 * 
 * NOTE: WHEN USING SET COMMANDS TO CHANGE MODULE CONFIGURATION, 
 * ALWAYS INVALIDATE THE CURRENT CONFIGURATION BEFORE BY USING THE 
 * FUNCTION lorawan_controller_invalidate_configuration()
 */
#define CMD_AT 						(AtCommand){"AT\n", "OK", "AT_ERROR"}
#define CMD_MCURESET 				(AtCommand){"ATZ\n", "", ""}

#define CMD_GET_DEVEUI 				(AtCommand){"AT+DEUI=?\n", "OK", "AT_PARAM_ERROR"}

#define CMD_SET_APP_EUI 			(AtCommand){"AT+APPEUI=%s\n", "OK", "AT_PARAM_ERROR"}
#define CMD_GET_APP_EUI 			(AtCommand){"AT+APPEUI=?\n", "OK", " "}

#define CMD_SET_APP_KEY 			(AtCommand){"AT+APPKEY=%s\n", "OK", "AT_PARAM_ERROR"}
#define CMD_GET_APP_KEY				(AtCommand){"AT+APPKEY=?\n", "OK", " "}

#define CMD_GET_DEV_ADDR			(AtCommand){"AT+DADDR=?\n", "OK", " "}
#define CMD_GET_NWK_SESSION_KEY		(AtCommand){"AT+NWKSKEY=?\n", "OK", " "}
#define CMD_GET_APP_SESSION_KEY		(AtCommand){"AT+APPSKEY=?\n", "OK", " "}

#define CMD_SET_CONFIRM_MODE 		(AtCommand){"AT+CFM=%s\n", "OK", "AT_PARAM_ERROR"} // 0 : unconfirmed, 1 : confirmed messages
#define CMD_GET_CONFIRM_MODE 		(AtCommand){"AT+CFM=?\n", "OK", " "} // 0 : unconfirmed, 1 : confirmed messages

#define CMD_GET_CONFIRM_STATUS 		(AtCommand){"AT+CFS=?\n", "OK", " "} // Get status of last AT+SEND, 0 : message not yet confirmed

#define CMD_SET_NETWORK_JOIN_MODE 	(AtCommand){"AT+NJM=%s\n", "OK", "AT_PARAM_ERROR"} // 0 : ABP, 1 : OTAA
#define CMD_GET_NETWORK_JOIN_MODE 	(AtCommand){"AT+NJM=?\n", "OK", " "} // 0 : ABP, 1 : OTAA

#define CMD_NETWORK_JOIN 			(AtCommand){"AT+JOIN\n", "OK", "AT_BUSY_ERROR"}
#define CMD_GET_NETWORK_JOIN_STATUS (AtCommand){"AT+NJS=?\n", "OK", " "}

#define CMD_SET_DUTYCYCLE_SETTINGS 	(AtCommand){"AT+DCS=%s\n", "OK"}  // 0 : ETSI duty cycle disable, 1 : enable
#define CMD_GET_DUTYCYCLE_SETTINGS 	(AtCommand){"AT+DCS=?\n", "OK", " "}  // 0 : ETSI duty cycle disable, 1 : enable

#define CMD_SEND_TEXTDATA 			(AtCommand){"AT+SEND=%s:%s\n", "OK", "AT_BUSY_ERROR"}
#define CMD_SEND_BINARYDATA 		(AtCommand){"AT+SENDB=%s:%s\n", "OK", "AT_BUSY_ERROR"}

#define CMD_DELAY_RECEIVE_WINDOW_1 	(AtCommand){"AT+RX1DL=?\n", "OK", " "}
#define CMD_DELAY_RECEIVE_WINDOW_2 	(AtCommand){"AT+RX2DL=?\n", "OK", " "}

#define CMD_LAST_RECEIVED_BIN_DATA 	(AtCommand){"AT+RECVB=?\n", "OK", " "}


/**
 * @brief Initialize the LoRaWAN controller. Serial Manager parameters,
 * like baudrate, callback functions, etc are definined here. 
 * The Flash controller is also done here, with the his first read to 
 * the lorawanControllerConfiguration_tag saved in flash.
 * If the configuration found is corrupted, the default configuration is
 * applied.
 * 
 * @return osaStatus_t osaStatus_Success is everything is done successfull.
 */
osaStatus_t lorawan_controller_init(void);

/**
 * @brief The LoRaWAN is initialized here with all the parameter stored in the
 * current lorawan configuration. To get this configuration, call the function
 * lorawan_controller_get_stored_configuration. 
 * 
 * @return lorawanControllerStatus_t lorawanController_Success if a network
 * has been join successfully. lorawanController_Error_Invalid_Configuration 
 * if the current configuration stored is invalid (parameters malformated). 
 * lorawanController_Error if the network could not be joined.
 */
lorawanControllerStatus_t lorawan_controller_init_module();

/**
 * @brief This function should only be used with GET commands. A buffer
 * will contain the response from the module if the expected first response
 * defined in the AT command is correct (principally "OK").
 * 
 * @param cmd The AT command that will be sent to the module with his expected
 * first response.
 * @param response_buffer Buffer to store the response.
 * @param response_buffer_length The total length of the buffer.
 * @return int The number of bytes that have been stored in the response buffer.
 */
int lorawan_controller_get_cmd(AtCommand cmd, char* response_buffer, int response_buffer_length);

/**
 * @brief This function should only be used with SET commands. An AT
 * command will be provided 
 * 
 * @param cmd The AT command that will be sent to the module with his expected
 * first response. The error response is also tested if the response wasn't followed 
 * by this error.
 * @param ... All the parameters wanted by the AT command. 
 * @return lorawanControllerStatus_t Return lorawanController_Success 
 * if the command received the expected response. Otherwise, 
 * lorawanController_Error
 */
lorawanControllerStatus_t lorawan_controller_set_cmd(AtCommand cmd, ...);

/**
 * @brief Check if the current configuration is valid. Everytime someone start
 * a new initialization of the LoRaWAN module, the configuration is set to false.
 * The configuration is set to true when a new network is joined. 
 * 
 * @return lorawanControllerStatus_t lorawanController_Success if the configuration
 * is valid, otherwise lorawanController_Error
 */
lorawanControllerStatus_t lorawan_controller_get_configuration_validity();

/**
 * @brief Get the current valid configuration stored in local. To get the last 
 * configuration stored inside the LoRaWAN module call first the function 
 * lorawan_controller_read_module_configuration(), then call this one.
 * 
 * @return lorawanControllerConfiguration_t The last stored configuration
 */
lorawanControllerConfiguration_t lorawan_controller_get_stored_configuration(void);

/**
 * @brief Read the LoRaWAN module to retrieve the configuration that is currently applied
 * to him. To retrieve the configuration read, call the function 
 * lorawan_controller_get_stored_configuration.
 * 
 * @return lorawanControllerStatus_t lorawanController_Success if the configuration
 * is correctly stored in local, otherwise lorawanController_Error
 */
lorawanControllerStatus_t lorawan_controller_read_module_configuration(void);

/**
 * @brief Function to apply the default configuration on the local stored configuration.
 * 
 */
void lorawan_controller_apply_default_configuration();
#endif /* __LORAWAN_CONTROLLER_H__ */
