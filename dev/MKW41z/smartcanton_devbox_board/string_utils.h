/**
 * @file    string_utils.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Set of functions that can be used to convert data to String or the
 * reverse.
 */

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include "string.h"
#include "stdio.h"

/**
 * @brief Convert a byte array to a hexadecimal string. The string will have the bytes
 * separated by the char ':'.
 * 
 * @param buffer Byte array to convert
 * @param bufferLength Byte array length
 * @param str String to contain the byte array conversion. The buffer needs to be 3 times
 * bigger thant the initial byte array.
 * @return int The length of the converter string
 */
int convertBytesArrayToHexStringSeparatedByChar(uint8_t *buffer, uint16_t bufferLength, char* str);

/**
 * @brief Convert a byte array to a hexadecimal string. Same as 
 * convertBytesArrayToHexStringSeparatedByChar but without a character
 * separation.
 * 
 * @param buffer Byte array to convert
 * @param bufferLength Byte array length
 * @param str String to contain the byte array conversion. The buffer needs to be 2 times
 * bigger thant the initial byte array.
 * @return int The length of the converter string
 */
int convertBytesArrayToHexString(uint8_t *buffer, uint16_t bufferLength, char* str);

/**
 * @brief Convert a hex string to a bytes array. The string has bytes
 * separated by the char ':'.
 * 
 * @param strHex String to be converted to byte array
 * @param bytesArray Byte array to store the final conversion
 * @return int Numbers of bytes written to the byte array
 */
int convertHexStringToBytesArraySeparatedByChar(char *strHex, uint8_t* bytesArray);

/**
 * @brief Convert a hex string to a bytes array. 
 * 
 * @param strHex String to be converted to byte array
 * @param bytesArray Byte array to store the final conversion
 * @return int Numbers of bytes written to the byte array
 */
int convertHexStringToBytesArray(const char * str, uint8_t * bytes, size_t blen);


/**
 * @brief Convert a integer string to a bytes array.
 * 
 * @param strHex String to be converted to integer
 * @return int Integer converted from the string in parameter
 */
int convertIntStringToInt(char* str);


#endif /* __STRING_UTILS_H__ */
