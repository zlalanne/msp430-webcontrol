/*
 * interface_analog.h
 *
 *  Created on: Sep 16, 2013
 *      Author: a0271518
 */


//*****************************************************************************
//
// interface_analog.h - analog I/O interface.
//
//*****************************************************************************

#ifndef INTERFACE_ANALOG_H_
#define INTERFACE_ANALOG_H_

#include "interface.h"
#include "stdint.h"

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************


extern void interfaceAnalogRead(uint8_t pin, char* input, char* input_length);
extern void interfaceAnalogWrite(uint8_t pin, char* output, char output_length);

#endif /* INTERFACE_analog_H_ */
