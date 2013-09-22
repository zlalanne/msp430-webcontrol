/*
 * interface_digital.c
 *
 *  Created on: Sep 16, 2013
 *      Author: a0271518
 */

//*****************************************************************************
//
// interface_digital.c - Digital I/O interface.
//
//*****************************************************************************
#include "interface.h"
#include "interface_digital.h"
#include "Energia.h"
#include "stdlib.h"
#include "string.h"

void interfaceDigitalReadInit(uint8_t pin){
    pinMode( pin, INPUT);
}



uint16_t interfaceDigitalRead( uint8_t pin, char* input, char* input_length){
    uint8_t value;
    uint16_t input_length;
    value = digitalRead(pin);
    if (value == HIGH)
    {
        strncpy(input, "HIGH", 4);
        *input_length = 4;
    }
    else
    {
        strncpy(input, "LOW", 3);
        *input_length = 3;
    }
    return input_length;
}
void interfaceDigitalWrite(uint8_t pin, char* output, char output_length){


    if (output_length==3)
        if (strcmp(output, "LOW")==0)
            digitalWrite( pin, LOW);
    if (output_length==4)
        if (strcmp(output, "HIGH")==0)
            digitalWrite( pin, HIGH);
    //!TODO: Exception handling

}

void interfaceDigitalWriteInit(uint8_t pin){
    pinMode( pin, OUTPUT);
}


