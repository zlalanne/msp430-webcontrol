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

void interfaceDigitalInit(uint8_t pin, char* direction, char direction_length){

    if (direction_length==6)
        if (strcmp(direction, "OUTPUT")==0)
            pinMode( pin, OUTPUT);
    if (direction_length==5)
            if (strcmp(direction, "INTPUT")==0)
                pinMode( pin, INPUT);
    //!TODO: Exception handling
}



void interfaceDigitalRead( uint8_t pin, char* input, char* input_length){
    uint8_t value;
    value = digitalRead(pin);
    if (value == HIGH)
    {
        input = "HIGH";
        *input_length = 4;
    }
    else
    {
        input = "LOW";
        *input_length = 3;
    }
}
void interfaceDigitalWrite(uint8_t pin, char* output, char output_length){
    uint8_t value;

    if (output_length==3)
        if (strcmp(output, "LOW")==0)
            digitalWrite( pin, LOW);
    if (output_length==4)
        if (strcmp(output, "HIGH")==0)
            digitalWrite( pin, HIGH);
    //!TODO: Exception handling

}


