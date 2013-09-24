/*
 * msp430f5529lp_interface.c
 *
 *  Created on: Sep 22, 2013
 *      Author: dd430
 */


//*****************************************************************************
//
// msp430f5529lp_interface.c - interface definition for MSP430F5529 LaunchPad.
//
//*****************************************************************************

#include "interface.h"
#include "pins_energia.h"
#include "msp430f5529lp_interface.h"



 const INTERFACE_name boardInterfaceNames[NUMBER_OF_INTERFACES] =
{
        // Default on-board interfaces
        {"GREENLED", 8, GREEN_LED},
        {"REDLED", 6, RED_LED},
        {"TEMPSENSOR", 10, A10},
        {"BATTERY", 7, A11},
        {"PUSH1", 5, PUSH1},
        {"PUSH2", 5, PUSH2},


        // F5529 available interfaces

        // Digital Input Interfaces
        {"P1_6_INPUT", 10, P1_6},
        {"P2_7_INPUT", 10, P2_7},
        {"P8_1_INPUT", 10, P8_1},
        {"P4_3_INPUT", 10, P4_3},
        {"P4_0_INPUT", 10, P4_0},
        {"P3_7_INPUT", 10, P3_7},
        {"P8_2_INPUT", 10, P8_2},

        // Digital Output Interfaces
        {"P1_6_OUTPUT", 11, P1_6},
        {"P2_7_OUTPUT", 11, P2_7},
        {"P8_1_OUTPUT", 11, P8_1},
        {"P4_3_OUTPUT", 11, P4_3},
        {"P4_0_OUTPUT", 11, P4_0},
        {"P3_7_OUTPUT", 11, P3_7},
        {"P8_2_OUTPUT", 11, P8_2},


        // Analog Interfaces
        {"A0", 2, A0},
        {"A1", 2, A1},
        {"A2", 2, A2},
        {"A3", 2, A3},
        {"A4", 2, A4},
        {"A5", 2, A5},
        {"A6", 2, A6},
        {"A12", 2, A12},

        // PWM Interfaces
        {"PWM0", 4, P2_0},
        {"PWM1", 4, P2_5},
        {"PWM2", 4, P1_5},
        {"PWM3", 4, P1_4},

        // Serial Interfaces
        {"BackUART", 8, BACK_CHANNEL_UART},
        {"UART0",5, UART0},
        {"SPI0",4, SPI0},
        {"I2C0",4, I2C0},

        //Specific Component Interfaces
        {"LCD0", 4, LCD0},
        {"RF0", 3, RF0},
        {"SPISENSOR0", 10, SPISENSOR0}

};




