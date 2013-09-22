/*
 * interface_digital.h
 *
 *  Created on: Sep 16, 2013
 *      Author: a0271518
 */


//*****************************************************************************
//
// interface_digital.h - Digital Input interface and Digital Output interface.

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



extern void interfaceDigitalReadInit(uint8_t pin);
extern uint16_t interfaceDigitalRead( uint8_t pin, char* input);

extern void interfaceDigitalWriteInit(uint8_t pin);
extern void interfaceDigitalWrite(uint8_t pin, char* output, char output_length);

#endif /* INTERFACE_DIGITAL_H_ */










