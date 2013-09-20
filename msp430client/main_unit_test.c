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
extern INTERFACE_generic digitalInterface;
extern INTERFACE_generic analogInterface;

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
    interfaces[0] = &digitalInterface;
    interfaces[1] = &digitalInterface;
    interfaces[2] = &analogInterface;
    interfaces[3] = &analogInterface;

    // Test digital interfaces
    interfaces[0]->init(GREEN_LED, "OUTPUT",6);
    interfaces[0]->write(GREEN_LED, "HIGH",4);
    interfaces[1]->init(RED_LED, "OUTPUT",6);
    interfaces[0]->write(RED_LED, "HIGH",4);

    // Test analog interface

    while (1){

        interfaces[2]->read(A10,buffer, &buffer_length);
        //!TODO: check if pin 19 is actually a PWM pin, verify functionality on a scope/LSA
        interfaces[3]->write(19,"123",3);

        __delay_cycles(100000);
        interfaces[0]->write(GREEN_LED, "LOW", 4);


        interfaces[2]->read(A10,buffer, &buffer_length);
        //!TODO: check if pin 19 is actually a PWM pin, verify functionality on a scope/LSA
        interfaces[3]->write(19,"500",3);

        __delay_cycles(100000);
        interfaces[0]->write(GREEN_LED, "HIGH", 4);



    }



    while(1);
}
