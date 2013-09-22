/*
 * main_unit_test.c
 *
 *  Created on: Sep 19, 2013
 *      Author: a0271518
 */


#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"
#include "spi.h"

#include "wlan.h"
#include "hci.h"
#include "netapp.h"
#include "socket.h"
#include "nvmem.h"

#include "driverlib.h"
#include "jsmn.h"

#include "server.h"
#include "server_protocol.h"


#include "interface.h"
#include "interface_digital.h"
#include "interface_analog.h"

#include "Energia.h"

INTERFACE_generic* interfaces[40];
extern INTERFACE_generic digitalReadInterface;
extern INTERFACE_generic digitalWriteInterface;
extern INTERFACE_generic analogReadInterface;
extern INTERFACE_generic analogWriteInterface;

static void initHardware(void) {

    pio_init();
    init_spi();



    turnLedOff(LED1);

}

void main(void){
    char *buffer, buffer_length;

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    initHardware();
    interfaceInitializeAll();
    interfaces[0] = &digitaWritelInterface;
    interfaces[1] = &digitalWriteInterface;
    interfaces[2] = &analogReadInterface;
    interfaces[3] = &analogReadInterface;

    // Test digital interfaces
    interfaces[0]->init(GREEN_LED);
    interfaces[0]->write(GREEN_LED, "HIGH",4);
    interfaces[1]->init(RED_LED);
    interfaces[0]->write(RED_LED, "HIGH",4);

    // Test analog interface

    while (1){

        buffer_length = interfaces[2]->read(A10,buffer);
        //!TODO: check if pin 19 is actually a PWM pin, verify functionality on a scope/LSA
        interfaces[3]->write(19,"123",3);

        __delay_cycles(100000);
        interfaces[0]->write(GREEN_LED, "LOW", 4);


        buffer_length = interfaces[2]->read(A10,buffer);
        //!TODO: check if pin 19 is actually a PWM pin, verify functionality on a scope/LSA
        interfaces[3]->write(19,"500",3);

        __delay_cycles(100000);
        interfaces[0]->write(GREEN_LED, "HIGH", 4);



    }



    while(1);
}
