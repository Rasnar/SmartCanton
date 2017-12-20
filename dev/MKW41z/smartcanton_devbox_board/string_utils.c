#include "string_utils.h"
#include "ctype.h"

int convertHexStringToBytesArray(char *strHex, uint8_t* bytesArray)
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
