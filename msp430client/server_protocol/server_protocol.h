#ifndef _server_protocol_h
#define _server_protocol_h

bool SERVER_parseConfig(char *buffer, jsmntok_t *tokens);
bool SERVER_setConfig(char *buffer, jsmntok_t *tokens);
bool SERVER_resumeStream(char *buffer, jsmntok_t *tokens);
#endif
