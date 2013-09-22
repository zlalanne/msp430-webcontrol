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

#include "string.h"

INTERFACE_generic digitalReadInterface;
INTERFACE_generic digitalWriteInterface;
INTERFACE_generic analogReadInterface;
INTERFACE_generic analogWriteInterface;


extern INTERFACE_name boardInterfaceNames[];

uint16_t interfaceDecode(char *pin, uint8_t pinLength){
    uint8_t i;
    for (i=0;i < NUMBER_OF_INTERFACES; i++)
    {
        if (strncmp(boardInterfaceNames[i].interfaceName, pin, boardInterfaceNames[i].interfaceName_length)==0)
            return boardInterfaceNames[i].interfaceNumber;
    }
    return 0xFFFF;


}

void dummyAPI(void){

}

void interfaceInitializeAll(){
    digitalReadInterface.init = interfaceDigitalReadInit;
    digitalReadInterface.read = interfaceDigitalRead;
    digitalReadInterface.write = dummyAPI;
    digitalReadInterface.decode = interfaceDecode;

    digitalWriteInterface.init = interfaceDigitalWriteInit;
    digitalWriteInterface.read = dummyAPI;
    digitalWriteInterface.write = interfaceDigitalWrite;
    digitalWriteInterface.decode = interfaceDecode;

    analogReadInterface.init = dummyAPI;
    analogReadInterface.read = interfaceAnalogRead;
    analogReadInterface.write = dummyAPI;
    analogReadInterface.decode = interfaceDecode;

    analogWriteInterface.init = dummyAPI;
    analogWriteInterface.read = dummyAPI;
    analogWriteInterface.write = interfaceAnalogWrite;
    analogWriteInterface.decode = interfaceDecode;
}
