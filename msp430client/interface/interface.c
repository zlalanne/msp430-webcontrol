/*
 * interface.c
 *
 *  Created on: Sep 19, 2013
 *      Author: a0271518
 */

#include "interface.h"
#include "interface_analog.h"
#include "interface_digital.h"
#include "util/util.h"

// Include interface for selected board
#ifdef __MSP430F5529__
#include "msp430f5529lp_interface.h"
#endif

#include <stdint.h>
#include <string.h>

/**
 * Used to decode the pin string to a pin number
 * @param pin string containing the pin
 * @param pinLength length of the string
 * @return the integer mapping of the pin
 */
static uint16_t interfaceDecode(char *pin, uint8_t pinLength){
    uint8_t i;
    for (i=0;i < NUMBER_OF_INTERFACES; i++)
    {
        if (strncmp(boardInterfaceNames[i].interfaceName, pin, boardInterfaceNames[i].interfaceName_length)==0)
            return boardInterfaceNames[i].interfaceNumber;
    }
    return 0xFFFF;
}


// Dummy functions to get rid of compiler warnings. These functions
// can be used when an interface does not need to implement certain
// functions
static void dummyInit(uint8_t pin) {
}

uint16_t dummyRead(uint8_t pin, char *input) {
	return 0;
}

void dummyWrite(uint8_t pin, char *output, char output_length) {

}


const genericInterface_t digitalReadInterface = {
		interfaceDigitalReadInit,
		interfaceDigitalRead,
		dummyWrite,
		UTIL_atoi
};

const genericInterface_t digitalOutputInterface = {
		interfaceDigitalOutputInit,
		interfaceDigitalOutputRead,
		interfaceDigitalOutputWrite,
		UTIL_atoi
};

const genericInterface_t analogReadInterface = {
		dummyInit,
		interfaceAnalogRead,
		dummyWrite,
		UTIL_atoi
};

const genericInterface_t analogWriteInterface = {
		dummyInit,
		dummyRead,
	    interfaceAnalogWrite,
	    UTIL_atoi
};

/**
 * Holds the list of interfaces. The index of this
 * array corresponds to the opcode when communicating
 * with the server.
 */
const genericInterface_t* INTERFACE_list[] = {
		&digitalOutputInterface,
		&digitalReadInterface,
		&analogReadInterface,
		&analogWriteInterface
};
