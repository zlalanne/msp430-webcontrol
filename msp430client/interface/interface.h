/*
 * interface.h
 *
 *  Created on: Sep 16, 2013
 *      Author: a0271518
 */
//*****************************************************************************
//
// interface.h - generic interface.
//
//*****************************************************************************

#ifndef INTERFACE_H_
#define INTERFACE_H_
#include <stdint.h>

typedef struct
{
    void (*const init)(uint8_t pin);
    uint16_t (*const read)(uint8_t pin, char* input);
    void (*const write)(uint8_t pin, char* output, char output_length);
    uint16_t (*const decode)(char* pinStr, uint8_t pin_length);
} genericInterface_t;

typedef struct
{
	const char* interfaceName;
	const int8_t interfaceName_length;
	const uint8_t interfaceNumber;
} interfaceName_t;

extern const genericInterface_t* INTERFACE_list[];

#endif /* INTERFACE_H_ */
