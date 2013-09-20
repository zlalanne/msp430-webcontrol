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





void interfaceAnalogRead(uint8_t pin, char* input, char* input_length){
    uint16_t value;
    value = analogRead(pin);
    *input_length=0;
    input = "   ";
    //!TODO Need to change to a better itoa function
    //This is horrible code, ULP Advisor would disown you if you keep modulo & division in here

    while (value>0)
    {
        input[*input_length] = '0' + value % 10;
        *input_length = (*input_length) + 1;
        value /= 10;

    }
}
void interfaceAnalogWrite(uint8_t pin, char* output, char output_length){
    uint16_t value;
    value = atoi(output);
    analogWrite(pin, value);
}


