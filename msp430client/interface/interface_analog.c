/*
 * interface_analog.c
 *
 *  Created on: Sep 16, 2013
 *      Author: a0271518
 */

//*****************************************************************************
//
// interface_analog.c - analog I/O interface.
//
//*****************************************************************************
#include "interface.h"
#include "interface_analog.h"
#include "stdlib.h"
#include "Energia.h"





uint16_t interfaceAnalogRead(uint8_t pin, char* input){
    uint16_t value, input_length=0;

    value = analogRead(pin);

    input = "   ";
    //!TODO Need to change to a better itoa function
    //This is horrible code, ULP Advisor would disown you if you keep modulo & division in here

    while (value>0)
    {
        input[input_length++] = '0' + value % 10;
        value /= 10;

    }
}
void interfaceAnalogWrite(uint8_t pin, char* output, char output_length){
    uint16_t value;
    value = atoi(output);
    analogWrite(pin, value);
}


