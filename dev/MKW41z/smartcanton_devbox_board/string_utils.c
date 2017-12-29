#include "string_utils.h"
#include "ctype.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int convertHexStringToBytesArraySeparatedByChar(char *strHex, uint8_t* bytesArray)
{
	const char separator[2] = ":";
	char *token;
	int idx = 0;
	unsigned int data;

	/* get the first token */
	token = strtok(strHex, separator);

	/* walk through other tokens */
	while (token != NULL)
	{
		sscanf(token, "%x", &data);

		bytesArray[idx++] = (uint8_t) data;
		token = strtok(NULL, separator);
	}

	return idx;
}

static int convertIntStringToIntN(char* str, int n)
{
	int sign = 1;
	int place = 1;
	int ret = 0;

	int i;
	for (i = n - 1; i >= 0; i--, place *= 10)
	{
		int c = str[i];
		switch (c)
		{
		case '-':
			if (i == 0)
				sign = -1;
			else
				return -1;
			break;
		default:
			if (c >= '0' && c <= '9')
				ret += (c - '0') * place;
			else
				return -1;
		}
	}

	return sign * ret;
}

int convertIntStringToInt(char* str)
{
	char* temp = str;
	int n = 0;
	while (*temp != '\0')
	{
		n++;
		temp++;
	}
	return convertIntStringToIntN(str, n);
}

// Based on https://stackoverflow.com/a/23898449/266720
int convertHexStringToBytesArray(const char * str, uint8_t * bytes, size_t blen)
{
   uint8_t  pos;
   uint8_t  idx0;
   uint8_t  idx1;

   // mapping of ASCII characters to hex values
   const uint8_t hashmap[] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
     0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
     0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
   };

   memset(bytes, 0, blen);
   for (pos = 0; ((pos < (blen*2)) && (pos < strlen(str))); pos += 2)
   {
      idx0 = ((uint8_t)str[pos+0] & 0x1F) ^ 0x10;
      idx1 = ((uint8_t)str[pos+1] & 0x1F) ^ 0x10;
      bytes[pos/2] = (uint8_t)(hashmap[idx0] << 4) | hashmap[idx1];
   };

   return strlen(str)/2;
}

int convertBytesArrayToHexStringSeparatedByChar(uint8_t *buffer, uint16_t bufferLength, char* str)
{

	int i = 0;

	if (bufferLength > 1)
	{
		for (i = 0; i < bufferLength - 1; i++)
		{
			sprintf(&str[3 * i], "%02X:", buffer[i]);
		};
	}

	sprintf(&str[3 * i], "%02X", buffer[i]);
	return 3 * i + 2; // String length
}

int convertBytesArrayToHexString(uint8_t *buffer, uint16_t bufferLength, char* str)
{

	int i = 0;

	if (bufferLength > 1)
	{
		for (i = 0; i < bufferLength; i++)
		{
			sprintf(&str[2 * i], "%02X", buffer[i]);
		};
	}
	return 2 * i + 2; // String length
}
