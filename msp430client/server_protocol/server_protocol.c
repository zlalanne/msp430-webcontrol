#include "server.h"

#include "jsmn.h"

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	char name[MAX_NAME];
	uint8_t nameLength;
	uint8_t opcode;
	uint16_t pin;
} config_t;

config_t Interfaces[MAX_INTERFACES];
uint8_t CurrentInterfaces = 0;


static uint8_t decodeOpcode(char *opcode, uint8_t opcodeLength) {
	uint8_t value = 0;
	uint8_t i;
	for(i = 0; i < opcodeLength; i++) {
		// Essentialy multiple the current value by 10 and add the new character
		value = (value << 3) + (value << 1) + (opcode[i] - '0');
	}
	return value;
}


static bool checkString(char *jsonBuffer, jsmntok_t *jsonToken, const char *expectedString, uint8_t expectedLength) {

	uint16_t length;
	char tokenBuffer[64];

	if(jsonToken->type == JSMN_STRING) {

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

	} else {
		return false;
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
	if(checkString(buffer, &key, CMD, 3) == false) {
		return false;
	}

	// Checking that the cmd is 'config'
	key = tokens[2];
	if(checkString(buffer, &key, CONFIG, 6) == false) {
		return false;
	}

	return true;
}


bool SERVER_setConfig(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[3];
	jsmntok_t nameKey;
	uint16_t i;
	uint16_t end;
	uint16_t pinLength;
	uint16_t opcodeLength;
	uint8_t opcode;
	char pin[MAX_PIN];
	char opcodeStr[MAX_OPCODE];

	// Checking for 'payload'
	if(checkString(buffer, &key, PAYLOAD, 7) == false) {
		return false;
	}

	key = tokens[4];
	if(key.type != JSMN_OBJECT) {
		return false;
	}

	// Reset the number of configurations
	CurrentInterfaces = 0;

	// Make sure there is a configuration to parse
	if((key.end - key.start) <= 1) {
		return true;
	}
	end = key.end;

	i = 5;
	do {

		// Save the name key, only copy the name if the config is valid
		nameKey = tokens[i++];

		i++;
		key = tokens[i++];
		// Check if first value is opcode
		if(checkString(buffer, &key, OPCODE, 6) == true) {

			// Copy the opcode
			key = tokens[i++];
			opcodeLength = key.end - key.start;
			if(key.type == JSMN_PRIMITIVE) {
				memcpy(opcodeStr, &buffer[key.start], opcodeLength);
			}

			// Check if second value is pin
			key = tokens[i++];
			if(checkString(buffer, &key, PIN, 3) == true) {

				// Copy the pin
				key = tokens[i++];
				pinLength = key.end - key.start;
				if(key.type == JSMN_STRING) {
					memcpy(pin, &buffer[key.start], pinLength);
				}

			} else {

				// Failed second decode
				CurrentInterfaces = 0;
				return false;
			}

		} else if(checkString(buffer, &key, PIN, 3) == true) {

			// Copy the pin
			key = tokens[i++];
			pinLength = key.end - key.start;
			if(key.type == JSMN_STRING) {
				memcpy(pin, &buffer[key.start], pinLength);
			}

			// Check if second value is opcode
			key = tokens[i++];
			if(checkString(buffer, &key, OPCODE, 6) == true) {

				// Copy the opcode
				key = tokens[i++];
				opcodeLength = key.end - key.start;
				if(key.type == JSMN_PRIMITIVE) {
					memcpy(opcodeStr, &buffer[key.start], opcodeLength);
				}

			} else {

				// Failed second decode
				CurrentInterfaces = 0;
				return false;
			}

		} else {

			// Failed first decode
			CurrentInterfaces = 0;
			return false;
		}

		opcode = decodeOpcode(opcodeStr, opcodeLength);

		Interfaces[CurrentInterfaces].nameLength = nameKey.end - nameKey.start;
		memcpy(Interfaces[CurrentInterfaces].name, &buffer[nameKey.start],
			   Interfaces[CurrentInterfaces].nameLength);

		Interfaces[CurrentInterfaces].opcode = opcode;
		Interfaces[CurrentInterfaces].pin = pinLength;

		CurrentInterfaces++;

	} while((key.end + 3) != end);

	// TODO: Remove for real configuration
	P4DIR |= BIT7;
	P4OUT &= ~(BIT7);

	return true;
}

bool SERVER_resumeStream(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[1];

	// Checking for 'cmd'
	if(checkString(buffer, &key, CMD, 3) == false) {
		return false;
	}

	// Checking that the cmd is 'resume_s'
	key = tokens[2];
	if(checkString(buffer, &key, RESUME_STREAM, 6) == false) {
		return false;
	}

	return true;
}

void SERVER_initInterfaces(void) {

	uint8_t i;

	for(i = 0; i < CurrentInterfaces; i++) {

		//Interfaces[Configs[i].opcode].init(Configs[i].pin, Configs[i].pinLength)
	}
}
