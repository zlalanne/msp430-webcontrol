#ifndef _server_protocol_h
#define _server_protocol_h

#include "jsmn.h"

#include <stdint.h>
#include <stdbool.h>

bool SERVER_parseConfig(char *buffer, jsmntok_t *tokens);
bool SERVER_setConfig(char *buffer, jsmntok_t *tokens);
bool SERVER_resumeStream(char *buffer, jsmntok_t *tokens);
void SERVER_initInterfaces(void);
uint16_t SERVER_sendData(char *data);
char SERVER_parseCommand(char *buffer, jsmntok_t *tokens);
bool SERVER_writeData(char *buffer, jsmntok_t *tokens);
uint8_t SERVER_getACKs(char *rxBuffer, jsmntok_t *tokens);
bool SERVER_dropToConfig(char *rxBuffer, jsmntok_t *tokens);
bool SERVER_pauseStreaming(char *buffer, jsmntok_t *tokens);
#endif
