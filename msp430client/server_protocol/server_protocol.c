#include "server.h"
#include "interface.h"

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

config_t Configs[MAX_CONFIGS];
uint8_t CurrentConfigs = 0;


static uint8_t decodeOpcode(char *opcode, uint8_t opcodeLength) {
	uint8_t value = 0;
	uint8_t i;
	for(i = 0; i < opcodeLength; i++) {
		// Essentially multiple the current value by 10 and add the new character
		value = (value * 10) + (opcode[i] - '0');
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
		if(memcmp(expectedString, tokenBuffer, length) != 0) {
			return false;
		} else {
			return true;
		}

	} else {
		return false;
	}

}

/**
 * Parse json hoping for a configuration.
 *
 * @param buffer Buffer containing the raw json string
 * @param tokens Buffer containing the json token information
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

uint8_t SERVER_getACKs(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[2];

	// Checking for 'ackdata'
	if(checkString(buffer, &key, ACKDATA, 7) == false) {
		return 0;
	}

	key = tokens[3];
	// Checking for 'ackcount'
	if(checkString(buffer, &key, COUNT, 5) == false) {
		return 0;
	}

	key = tokens[4];
	if((key.type == JSMN_PRIMITIVE) && (key.end - key.start == 1)) {
		return buffer[key.start] - '0';
	} else {
		// TODO: Change this to a sentinel
		return 0;
	}
}


char SERVER_parseCommand(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[1];

	// Checking for 'cmd'
	if(checkString(buffer, &key, CMD, 3) == false) {
		return '\0';
	}

	// Determine type of command
	key = tokens[2];
	return buffer[key.start];
}

bool SERVER_writeData(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[2];
	jsmntok_t pinKey;
	jsmntok_t valueKey;
	uint8_t opcode;
	uint16_t pin;

	// Checking for 'writedata'
	if(checkString(buffer, &key, WRITEDATA, 9) == false) {
		return false;
	}

	// Checking for 'payload'
	key = tokens[3];
	if(checkString(buffer, &key, PAYLOAD, 7) == false) {
		return false;
	}

	key = tokens[4];
	if(key.type != JSMN_OBJECT) {
		return false;
	}

	key = tokens[5];
	switch(buffer[key.start]) {
	case 'o':
		if(checkString(buffer, &key, OPCODE, 6) == false) {
			return false;
		}

		key = tokens[6];
		opcode = decodeOpcode(&buffer[key.start], key.end - key.start);

		key = tokens[7];
		if(buffer[key.start] == 'p') {
			if(checkString(buffer, &key, PIN, 3) == false) {
				return false;
			}
			pinKey = tokens[8];

			key = tokens[9];
			if(checkString(buffer, &key, VALUE, 5) == false) {
				return false;
			}

			valueKey = tokens[10];
		} else {

			if(checkString(buffer, &key, VALUE, 5) == false) {
				return false;
			}
			valueKey = tokens[8];

			key = tokens[9];
			if(checkString(buffer, &key, PIN, 3) == false) {
				return false;
			}
			pinKey = tokens[10];
		}

		break;
	case 'v':
		if(checkString(buffer, &key, VALUE, 5) == false) {
			return false;
		}

		valueKey = tokens[6];
		key = tokens[7];
		if(buffer[key.start] == 'o') {
			if(checkString(buffer, &key, OPCODE, 6) == false) {
				return false;
			}
			key = tokens[8];
			opcode = decodeOpcode(&buffer[key.start], key.end - key.start);

			key = tokens[9];
			if(checkString(buffer, &key, PIN, 3) == false) {
				return false;
			}
			pinKey = tokens[10];
		} else {

			if(checkString(buffer, &key, PIN, 3) == false) {
				return false;
			}
			pinKey = tokens[8];

			key = tokens[9];
			if(checkString(buffer, &key, OPCODE, 6) == false) {
				return false;
			}
			key = tokens[10];
			opcode = decodeOpcode(&buffer[key.start], key.end - key.start);
		}

		break;
	case 'p':
		if(checkString(buffer, &key, PIN, 3) == false) {
			return false;
		}
		pinKey = tokens[6];
		key = tokens[7];

		if(buffer[key.start] == 'o') {
			if(checkString(buffer, &key, OPCODE, 6) == false) {
				return false;
			}
			key = tokens[8];
			opcode = decodeOpcode(&buffer[key.start], key.end - key.start);

			key = tokens[9];
			if(checkString(buffer, &key, VALUE, 5) == false) {
				return false;
			}

			valueKey = tokens[10];
		} else {

			if(checkString(buffer, &key, VALUE, 5) == false) {
				return false;
			}
			valueKey = tokens[8];

			key = tokens[9];
			if(checkString(buffer, &key, OPCODE, 6) == false) {
				return false;
			}
			key = tokens[10];
			opcode = decodeOpcode(&buffer[key.start], key.end - key.start);
		}
		break;
	}

	pin = INTERFACE_list[opcode]->decode(&buffer[pinKey.start], pinKey.end - pinKey.start);
	INTERFACE_list[opcode]->write(pin, &buffer[valueKey.start], valueKey.end - valueKey.start);

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
	CurrentConfigs = 0;

	// Make sure there is a configuration to parse
	if((key.end - key.start) <= 2) {
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
				CurrentConfigs = 0;
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
				CurrentConfigs = 0;
				return false;
			}

		} else {

			// Failed first decode
			CurrentConfigs = 0;
			return false;
		}

		opcode = decodeOpcode(opcodeStr, opcodeLength);

		Configs[CurrentConfigs].nameLength = nameKey.end - nameKey.start;
		memcpy(Configs[CurrentConfigs].name, &buffer[nameKey.start],
			   Configs[CurrentConfigs].nameLength);
		Configs[CurrentConfigs].opcode = opcode;
		Configs[CurrentConfigs].pin = INTERFACE_list[opcode]->decode(pin, pinLength);

		CurrentConfigs++;

	} while((key.end + 3) != end);

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

uint16_t SERVER_sendData(char *data) {

	uint8_t i;
	uint16_t currentIndex = DATA_HEADER_LEN;
	uint16_t dataLength;

	for(i = 0; i < CurrentConfigs; i++) {

		// Writing the name of the configuration
		data[currentIndex++] = '"';
		memcpy(&data[currentIndex], Configs[i].name, Configs[i].nameLength);
		currentIndex += Configs[i].nameLength;
		data[currentIndex++] = '"';
		data[currentIndex++] = ':';

		// Writing the data for the configuration
		data[currentIndex++] = '"';
		dataLength = INTERFACE_list[Configs[i].opcode]->read(Configs[i].pin, &data[currentIndex]);
		currentIndex += dataLength;
		data[currentIndex++] = '"';

		// Separating the configuration
		data[currentIndex++] = ',';
	}

	// Take off the last ,
	currentIndex--;
	memcpy(&data[currentIndex], DATA_FOOTER, DATA_FOOTER_LEN);
	currentIndex += DATA_FOOTER_LEN;

	return currentIndex;
}

void SERVER_initInterfaces(void) {

	uint8_t i;

	for(i = 0; i < CurrentConfigs; i++) {
		INTERFACE_list[Configs[i].opcode]->init(Configs[i].pin);
	}
}

bool SERVER_pauseStreaming(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[2];

	// Checking for 'pause'
	if(checkString(buffer, &key, PAUSE_STREAM, 5) == false) {
		return false;
	} else {
		return true;
	}
}

bool SERVER_dropToConfig(char *buffer, jsmntok_t *tokens) {

	jsmntok_t key = tokens[2];

	// Checking for 'dropconfig'
	if(checkString(buffer, &key, DROPCONFIG, 10) == false) {
		return false;
	} else {
		return true;
	}
}
