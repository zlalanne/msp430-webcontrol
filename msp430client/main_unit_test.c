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
#include "BCUart.h"

INTERFACE_generic* interfaces[40];
extern INTERFACE_generic digitalReadInterface;
extern INTERFACE_generic digitalWriteInterface;
extern INTERFACE_generic analogReadInterface;
extern INTERFACE_generic analogWriteInterface;

char SerialBuffer[128];               // This can be any size

static void initHardware(void) {

    pio_init();
    init_spi();



    turnLedOff(LED1);

    turnLedOff(LED2);
}

void main(void){
    char buffer[128], buffer_length;
    char pinStr[128], value[20];
    uint8_t SerialBuffer_length=0, pinStr_length, value_length;
    uint8_t i, j, k, opcode, pinNumber;
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    initHardware();
    interfaceInitializeAll();
    bcUartInit();
    __bis_SR_register(GIE);
    for (i=0;i<128;i++)
    {
        pinStr[i] = 0;
        buffer[i] = 0;
        if (i<20)
            value[i] = 0;
    }

    interfaces[0] = &digitalWriteInterface;
    interfaces[1] = &digitalReadInterface;
    interfaces[2] = &analogReadInterface;
    interfaces[3] = &analogWriteInterface;

    // Test digital interfaces
    interfaces[0]->init(GREEN_LED);
    interfaces[0]->write(GREEN_LED, "LOW",3);
    interfaces[0]->init(RED_LED);
    interfaces[0]->write(RED_LED, "LOW",3);

    interfaces[1]->init(PUSH1);
    interfaces[1]->init(PUSH2);
    // Test analog interface

    while (1){
        opcode = 0; // digitalWrite


        pinNumber = interfaces[opcode]->decode("GREEN_LED", 9);
        interfaces[0]->write(pinNumber, "HIGH", 4);


        pinNumber = interfaces[opcode]->decode("RED_LED", 9);
        interfaces[0]->write(pinNumber, "HIGH", 4);


        opcode = 2;

        pinNumber = interfaces[2]->decode("TEMPSENSOR",10);
        buffer_length = interfaces[2]->read(pinNumber,buffer);


        pinNumber = interfaces[2]->decode("BATTERY",7);
        buffer_length = interfaces[2]->read(pinNumber,buffer);


        opcode = 1;
        pinNumber = interfaces[opcode]->decode("PUSH1", 5);
        buffer_length = interfaces[opcode]->read(pinNumber,buffer);

        pinNumber = interfaces[opcode]->decode("PUSH2", 5);
        buffer_length = interfaces[opcode]->read(pinNumber,buffer);


        opcode = 3;
        pinNumber = interfaces[opcode]->decode("PWM0",4);
        interfaces[opcode]->write(pinNumber,"150",3);

        __delay_cycles(1000000);
        opcode = 0; // digitalWrite


        pinNumber = interfaces[opcode]->decode("GREEN_LED", 9);
        interfaces[0]->write(pinNumber, "LOW", 3);


        pinNumber = interfaces[opcode]->decode("RED_LED", 9);
        interfaces[0]->write(pinNumber, "LOW", 3);
        __delay_cycles(1000000);
    }




}

/// TRASH


//
//    while (1){
//
//        SerialBuffer_length+= bcUartReceiveBytesInBuffer(&SerialBuffer[SerialBuffer_length]);
//        i = 0;
//        while (i< SerialBuffer_length && SerialBuffer[i] != '\n')
//            i++;
//        if (SerialBuffer[i] == '\n')
//        {
//
//            // Op code is first character
//            opcode = SerialBuffer[0]-'0';
//            // Then a space
//            j = 2;
//            // Looking for the PinStr now,
//            while (SerialBuffer[j]!=' ')
//                j++;
//            k = j;
//            pinStr_length = j -2 - 1;
//            strncpy(pinStr,&SerialBuffer[2],pinStr_length);
//
//            while (SerialBuffer[j]!=' ')
//                j++;
//            strncpy(value,&SerialBuffer[k], j-k-1);
//            value_length = j-k-1;
//            for (k=i+1;k<SerialBuffer_length;k++)
//                SerialBuffer[k-i-1] = SerialBuffer[k];
//            for (k=SerialBuffer_length-i;k++;k<128)
//                SerialBuffer[k] = 0;
//
//            pinNumber = interfaces[opcode]->decode(pinStr, pinStr_length);
//
//        }
