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

int convertBytesArrayToHexStringSeparatedByChar(uint8_t *buffer, uint16_t bufferLength, char* str);
int convertBytesArrayToHexString(uint8_t *buffer, uint16_t bufferLength, char* str);

int convertHexStringToBytesArraySeparatedByChar(char *strHex, uint8_t* bytesArray);
int convertHexStringToBytesArray(const char * str, uint8_t * bytes, size_t blen);

int convertIntStringToInt(char* str);


#endif /* __STRING_UTILS_H__ */
