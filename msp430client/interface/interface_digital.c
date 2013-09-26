/*
 * interface_digital.c
 *
 *  Created on: Sep 16, 2013
 *      Author: a0271318
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
    pinMode(pin, INPUT_PULLUP);
}

uint16_t interfaceDigitalRead(uint8_t pin, char* input) {
	uint8_t value;
	uint16_t input_length;
	value = digitalRead(pin);
	if (value == HIGH) {
		strncpy(input, "HIGH", 4);
		input_length = 4;
	} else {
		strncpy(input, "LOW", 3);
		input_length = 3;
	}
	return input_length;
}

void interfaceDigitalOutputInit(uint8_t pin) {
	pinMode(pin, OUTPUT);
}

void interfaceDigitalOutputWrite(uint8_t pin, char* output, char output_length) {
	if (output_length == 5) {
		if (strncmp(output, "False", 5) == 0) {
			digitalWrite(pin, LOW);
		}
	} else if (output_length == 4) {
		if (strncmp(output, "True", 4) == 0) {
			digitalWrite(pin, HIGH);
		}
	}
}

uint16_t interfaceDigitalOutputRead(uint8_t pin, char* input) {
	uint8_t value;
	uint16_t input_length;

	value = digitalPresentOutput(pin);
	if (value == HIGH) {
		strncpy(input, "HIGH", 4);
		input_length = 4;
	} else {
		strncpy(input, "LOW", 3);
		input_length = 3;
	}
	return input_length;
}
