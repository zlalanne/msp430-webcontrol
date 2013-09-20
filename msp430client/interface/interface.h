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
#include "stdint.h"
//*****************************************************************************
//
//! \typedef INTERFACE_generic
//! \brief Type definition for \link _INTERFACE_generic \endlink
//!     structure
//!
//! \struct _INTERFACE_generic
//! \brief Generic type for \b interface.
//
//*****************************************************************************
typedef struct _INTERFACE_generic
{
    void (*init)(uint8_t pin, char* direction, char direction_length);
    void (*read)(uint8_t pin, char* input, char* input_length);
    void (*write)(uint8_t pin, char* output, char output_length);

} INTERFACE_generic;


extern uint16_t interfaceDecode(char *pin, uint16_t pinLength);
extern void interfaceInitializeAll();
#endif /* INTERFACE_H_ */
