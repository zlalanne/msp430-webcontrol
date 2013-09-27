/*
 * msp430f5529lp_interface.h
 *
 *  Created on: Sep 22, 2013
 *      Author: dd430
 */

#ifndef MSP430F5529LP_INTERFACE_H_
#define MSP430F5529LP_INTERFACE_H_

#define BACK_CHANNEL_UART 1
#define UART0    0

#define SPI0        0
#define I2C0        0
#define LCD0        0
#define RF0         0
#define SPISENSOR0  0

#define NUMBER_OF_INTERFACES    39

extern const interfaceName_t boardInterfaceNames[NUMBER_OF_INTERFACES];

#endif /* MSP430F5529LP_INTERFACE_H_ */
