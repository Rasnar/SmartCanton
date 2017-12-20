/**
 * @file    lorawan_controller_task.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include "string.h"
#include "stdio.h"

int convertBytesArrayToHexString(uint8_t *buffer, uint16_t bufferLength, char* str);
int convertBytesArrayToHexStringSeparatedByChar(uint8_t *buffer, uint16_t bufferLength, char* str);
int convertHexStringToBytesArray(char *strHex, uint8_t* bytesArray);

#endif /* __STRING_UTILS_H__ */
