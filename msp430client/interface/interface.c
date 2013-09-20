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
INTERFACE_generic digitalInterface;
INTERFACE_generic analogInterface;

uint16_t interfaceDecode(char *pin, uint16_t pinLength){

    uint16_t pinValue;
    return pinValue;
}

void interfaceInitializeAll(){
    digitalInterface.init = interfaceDigitalInit;
    digitalInterface.read = interfaceDigitalRead;
    digitalInterface.write = interfaceDigitalWrite;


    analogInterface.read = interfaceAnalogRead;
    analogInterface.write = interfaceAnalogWrite;

}
