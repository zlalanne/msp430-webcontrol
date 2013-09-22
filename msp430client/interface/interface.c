/*
 * interface.c
 *
 *  Created on: Sep 19, 2013
 *      Author: a0271518
 */

#include "interface.h"
#include "stdint.h"
#include "interface_analog.h"
#include "interface_digital.h"
INTERFACE_generic digitalReadInterface;
INTERFACE_generic digitalWriteInterface;
INTERFACE_generic analogReadInterface;
INTERFACE_generic analogWriteInterface;

uint16_t interfaceDecode(char *pin, uint16_t pinLength){

    uint16_t pinValue;
    return pinValue;
}

void interfaceInitializeAll(){
    digitalReadInterface.init = interfaceDigitalReadInit;
    digitalReadInterface.read = interfaceDigitalRead;

    digitalWriteInterface.init = interfaceDigitalWriteInit;
    digitalWriteInterface.write = interfaceDigitalWrite;

    analogReadInterface.init = interfaceAnalogReadInit;
    analogReadInterface.read = interfaceAnalogRead;

    analogWriteInterface.init = interfaceAnalogWriteInit;
    analogWriteInterface.write = interfaceAnalogWrite;

}
