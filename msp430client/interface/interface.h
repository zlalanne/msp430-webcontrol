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

#define NUMBER_OF_INTERFACES    39
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
    void (*init)(uint8_t pin);
    uint16_t (*read)(uint8_t pin, char* input);
    void (*write)(uint8_t pin, char* output, char output_length);
    uint16_t (*decode)(char* pinStr, uint8_t pin_length);
} INTERFACE_generic;


typedef struct _INTERFACE_name
{
   char* interfaceName;
  int8_t interfaceName_length;
  uint8_t interfaceNumber;
} INTERFACE_name;


extern INTERFACE_generic digitalReadInterface;
extern INTERFACE_generic digitalWriteInterface;
extern INTERFACE_generic analogReadInterface;
extern INTERFACE_generic analogWriteInterface;

extern uint16_t interfaceDecode(char *pinStr, uint8_t pinLength);
extern void dummyAPI(void);
extern void interfaceInitializeAll();
#endif /* INTERFACE_H_ */
