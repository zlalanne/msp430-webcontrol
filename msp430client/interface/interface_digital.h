/*
 * interface_digital.h
 *
 *  Created on: Sep 16, 2013
 *      Author: a0271518
 */


//*****************************************************************************
//
// interface_digital.h - Digital I/O interface.
//
//*****************************************************************************

#ifndef INTERFACE_DIGITAL_H_
#define INTERFACE_DIGITAL_H_

#include "interface.h"
#include "stdint.h"

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************



extern void interfaceDigitalInit(uint8_t pin, char* direction, char direction_length);
extern void interfaceDigitalRead( uint8_t pin, char* input, char* input_length);
extern void interfaceDigitalWrite(uint8_t pin, char* output, char output_length);

#endif /* INTERFACE_DIGITAL_H_ */










