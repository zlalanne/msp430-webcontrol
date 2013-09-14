#include "server.h"

#include "jsmn.h"

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct{
	uint8_t *jsonBuffer;
	jsmntok_t jsonToken;
	uint8_t *expectedString;
	uint8_t *expectedLength;
} stringCheck_t;


static bool checkString(char *jsonBuffer, jsmntok_t *jsonToken, const char *expectedString, uint8_t expectedLength) {

	uint16_t length;
	char tokenBuffer[64];

	// First check if strings are the same length
	length = jsonToken->end - jsonToken->start;
	if(length != expectedLength) {
		return false;
	}

	memcpy(tokenBuffer, &jsonBuffer[jsonToken->start], length);
	if(strncmp(expectedString, tokenBuffer, length) != 0) {
		return false;
	} else {
		return true;
	}
}


/**
 * Parse json hoping for a confugration.
 *
 * @param buffer Buffer containing the raw json string
 * @param tokens Buffer containg the json token information
 * @return true if parsed correctly, false otherwise
 */
bool SERVER_parseConfig(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[1];

	// Checking for 'cmd'
	if(key.type == JSMN_STRING) {
		if(checkString(buffer, &key, CMD, 3) == false) {
			return false;
		}
	} else {
		return false;
	}

	// Checking that the cmd is 'config'
	key = tokens[2];
	if(key.type == JSMN_STRING) {
		if(checkString(buffer, &key, CONFIG, 6) == false) {
			return false;
		}
	} else {
		return false;
	}

	return true;
}

bool SERVER_setConfig(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[3];
	uint16_t i;
	uint16_t end;

	// Checking for 'payload'
	if(key.type == JSMN_STRING) {
		if(checkString(buffer, &key, PAYLOAD, 7) == false) {
			return false;
		}
	} else {
		return false;
	}

	key = tokens[4];
	if(key.type != JSMN_OBJECT) {
		return false;
	}

	// Checking for 'read'
	key = tokens[5];
	if(key.type == JSMN_STRING) {
		if(checkString(buffer, &key, READ, 4) == false) {
			return false;
		}
	} else {
		return false;
	}

	// Need to skip the read object
	key = tokens[6];
	if(key.type == JSMN_OBJECT){
		end = key.end;
		i = 7;

		// Loop until we know we are done with the read object
		// TODO: Replace this with handling th reads
		while(i < 256){
			key = tokens[i];
			if(key.start > end) {
				break;
			} else{
				i++;
			}
		}
	} else {
		return false;
	}

	key = tokens[i];
	i++;
	if(key.type == JSMN_STRING) {
		if(checkString(buffer, &key, WRITE, 5) == false) {
			return false;
		}
	} else {
		return false;
	}

	P4DIR |= BIT7;
	P4OUT &= ~(BIT7);

	return true;
}

bool SERVER_resumeStream(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[1];

	// Checking for 'cmd'
	if(key.type == JSMN_STRING) {
		if(checkString(buffer, &key, CMD, 3) == false) {
			return false;
		}
	} else {
		return false;
	}

	// Checking that the cmd is 'config'
	key = tokens[2];
	if(key.type == JSMN_STRING) {
		if(checkString(buffer, &key, RESUME_STREAM, 8) == false) {
			return false;
		}
	} else {
		return false;
	}

	return true;
}
