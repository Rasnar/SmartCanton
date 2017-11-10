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

//typedef struct lorawanControllerConfiguration_tag{
//	char appEui[24];
//	char appKey[48];
//	char appSKey[48];
//	char devAddr[12];
//	char devEui[24];
//	char nwkId[12];
//	char nwkskey[48];
//
//	char confirmMode[2];
//	char confirmMode[2];
//}lorawanControllerConfiguration_t;

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
#define CMD_SET_APP_KEY 			(AtCommand){"AT+APPKEY=%s\n", "OK", "AT_PARAM_ERROR"}
#define CMD_GET_APP_KEY				(AtCommand){"AT+APPKEY=?\n", "OK", "AT_PARAM_ERROR"}
#define CMD_SET_CONFIRM_MODE 		(AtCommand){"AT+CFM=%s\n", "OK", "AT_PARAM_ERROR"} // 0 : unconfirmed, 1 : confirmed messages
#define CMD_SET_CONFIRM_STATUS 		(AtCommand){"AT+CFS=?\n", "OK"}
#define CMD_NETWORK_JOIN 			(AtCommand){"AT+JOIN\n", "OK", "AT_BUSY_ERROR"}
#define CMD_SET_NETWORK_JOIN_MODE 	(AtCommand){"AT+NJM=%s\n", "OK", "AT_PARAM_ERROR"} // 0 : ABP, 1 : OTAA
#define CMD_GET_NETWORK_JOIN_STATUS (AtCommand){"AT+NJS=?\n", "OK", " "}
#define CMD_SET_DUTYCYCLE_SETTINGS 	(AtCommand){"AT+DCS=%s\n", "OK"}  // 0 : ETSI duty cycle disable, 1 : enable
#define CMD_SEND_TEXTDATA 			(AtCommand){"AT+SEND=%s:%s\n", "OK", "AT_BUSY_ERROR"}
#define CMD_SEND_BINARYDATA 		(AtCommand){"AT+SENDB=%s:%s\n", "OK", "AT_BUSY_ERROR"}

osaStatus_t lorawan_controller_init(void);

lorawanControllerStatus_t lorawan_controller_init_module();

int lorawan_controller_get_cmd(AtCommand cmd, char* response_buffer, int response_buffer_length);

lorawanControllerStatus_t lorawan_controller_set_cmd(AtCommand cmd, ...);

#endif /* __LORAWAN_CONTROLLER_H__ */
