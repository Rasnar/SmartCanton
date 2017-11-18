/**
 * @file    lorawan_controller.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
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

#define LORAWAN_CONTROLLER_MAGIC_WORD	0x2222

typedef struct lorawanControllerConfiguration_tag{
	char appEui[24];
	char appKey[48];
	char devEui[24];

	char devAddr[12];
	char nwkSessionKey[48];
	char appSessionKey[48];

	char networkJoinMode[2];
	char confirmMode[2];
	char etsiDutyCycleEnable[2];

	// Used to make the lorawanControllerConfiguration_tag a
	// multiple of 4 and to verify validity
	uint16_t magicWord;
}lorawanControllerConfiguration_t;

typedef enum lorawanControllerStatus_tag
{
    lorawanController_Success = 0U, /*!< Success */
	lorawanController_Error = 1U, /*!< Failed */
}lorawanControllerStatus_t;


/**
 * AT commands to communicate with the LoRa MCU
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

osaStatus_t lorawan_controller_init(void);

lorawanControllerStatus_t lorawan_controller_init_module();

int lorawan_controller_get_cmd(AtCommand cmd, char* response_buffer, int response_buffer_length);

lorawanControllerStatus_t lorawan_controller_set_cmd(AtCommand cmd, ...);

lorawanControllerStatus_t lorawan_controller_get_configuration_validity();

lorawanControllerConfiguration_t lorawan_controller_get_current_configuration(void);

lorawanControllerStatus_t lorawan_controller_read_module_configuration(void);
#endif /* __LORAWAN_CONTROLLER_H__ */
