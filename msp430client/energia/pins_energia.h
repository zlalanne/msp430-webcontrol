/*
  ************************************************************************
  *	pins_energia.h
  *
  *	Energia core files for MSP430
  *		Copyright (c) 2012 Robert Wessels. All right reserved.
  *
  *     Contribution: Rei VILO
  *
  ***********************************************************************
  Derived from:
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#ifndef Pins_Energia_h
#define Pins_Energia_h
#ifndef BV
#define BV(x) (1 << (x))
#endif

#if defined(__MSP430_HAS_USCI_A0__) || defined(__MSP430_HAS_USCI_A1__)
static const uint8_t SS      = 8;  /* P2.7 */
static const uint8_t SCK     = 7;  /* P3.2 */
static const uint8_t MOSI    = 15;  /* P3.0 */
static const uint8_t MISO    = 14;  /* P3.1 */
static const uint8_t TWISDA  = 15;  /* P3.0 */
static const uint8_t TWISCL  = 14;  /* P3.1 */
static const uint8_t DEBUG_UARTRXD = 45;  /* Receive  Data (RXD) at P4.5 */
static const uint8_t DEBUG_UARTTXD = 46;  /* Transmit Data (TXD) at P4.4 */
static const uint8_t AUX_UARTRXD = 3;  /* Receive  Data (RXD) at P4.5 */
static const uint8_t AUX_UARTTXD = 4;  /* Transmit Data (TXD) at P4.4 */
#define TWISDA_SET_MODE (PORT_SELECTION0)
#define TWISCL_SET_MODE (PORT_SELECTION0)
#define DEBUG_UARTRXD_SET_MODE (PORT_SELECTION0 | (PM_UCA1RXD << 8) | INPUT)
#define DEBUG_UARTTXD_SET_MODE (PORT_SELECTION0 | (PM_UCA1TXD << 8) | OUTPUT)
#define AUX_UARTRXD_SET_MODE (PORT_SELECTION0 | INPUT)
#define AUX_UARTTXD_SET_MODE (PORT_SELECTION0 | OUTPUT)
#define SPISCK_SET_MODE (PORT_SELECTION0)
#define SPIMOSI_SET_MODE (PORT_SELECTION0)
#define SPIMISO_SET_MODE (PORT_SELECTION0)
#endif

#define DEBUG_UART_MODULE_OFFSET 0x40
#define AUX_UART_MODULE_OFFSET 0x0
#define SERIAL1_AVAILABLE 1

#if defined(__MSP430_HAS_USCI_A1__)
#define USE_USCI_A1
#endif

#define A0   0
#define A1   1
#define A2   2
#define A3   3
#define A4   4
#define A5   5
#define A6   6
#define A7   7
#define A10  10
#define A11   11
#define A12   12
#define A13   13
#define A14   14
#define A15   15

/*
 F5529 LaunchPad pin mapping

                               +--\/--+                                 +--\/--+
                          3.3v |1   21| 5.0v               (TA2.2) P2.5 |40  20| GND
                  (TB0.0) P5.6 |2   22| GND                (TA2.1) P2.4 |39  19| P2.0 (TA1.1)
     (UCA0RXD -> Serial1) P3.4 |3   23| P6.0 (A0)          (TA0.4) P1.5 |38  18| P2.2
     (UCA0TXD -> Serial1) P3.3 |4   24| P6.1 (A1)          (TA0.3) P1.4 |37  17| P7.4 (TB0.2)
                          P1.6 |5   25| P6.2 (A2)          (TA0.2) P1.3 |36  16| RST
                     (A6) P6.6 |6   26| P6.3 (A3)          (TA0.1) P1.2 |35  15| P3.0 (UCB0SIMO) (UCB0SDA)
                          P3.2 |7   27| P6.4 (A4)                  P4.3 |34  14| P3.1 (UCB0SOMI) (UCB0SCL)
                          P2.7 |8   28| P7.0 (A12)                 P4.0 |33  13| P2.6
             (PM_UCB1SCL) P4.2 |9   29| P3.6 (TB0.6)               P3.7 |32  12| P2.3 (TA2.0)
             (PM_UCB1SDA) P4.1 |10  30| P3.5 (TB0.5)               P8.2 |31  11| P8.1
                               +------+                                 +------+

                                 ----+
                                   41| P2.1 (PUSH1)
                                   42| P1.1 (PUSH2)
                                   43| P1.0 (LED1 - RED)
                                   44| P4.7 (LED2 - GREEN)
                                   45| P4.5 (UCA1RXD -> ezFET -> Serial)
                                   46| P4.4 (UCA1TXD -> ezFET -> Serial)
                                 ----+
*/

/* Pin names based on the silkscreen */

/* PIN1 = 3.3v */
#define P6_5  2
#define P3_4  3
#define P3_3  4
#define P1_6  5
#define P6_6  6
#define P3_2  7
#define P2_7  8
#define P4_2  9
#define P4_1  10

#define P8_1  11
#define P2_3  12
#define P2_6  13
#define P3_1  14
#define P3_0  15
/* PIN16 is RST */
#define P7_4  17
#define P2_2  18
#define P2_0  19
/* PIN16 is GND */

/* PIN21 is 5.0v */
/* PIN22 is GND */
#define P6_0  23
#define P6_1  24
#define P6_2  25
#define P6_3  26
#define P6_4  27
#define P7_0  28
#define P3_6  29
#define P3_5  30

#define P8_2  31
#define P3_7  32
#define P4_0  33
#define P4_3  34
#define P1_2  35
#define P1_3  36
#define P1_4  37
#define P1_5  38
#define P2_4  39
#define P2_5  40

/* Extra pins not on the header + temp sensor */
#define P2_1  41
#define P1_1  42
#define P1_0  43
#define P4_7  44
#define P4_5  45
#define P4_4  46

#define PUSH1  41
#define PUSH2  42
#define RED_LED  43
#define GREEN_LED  44
#define TEMPSENSOR  10

#ifdef ARDUINO_MAIN

const uint16_t port_to_input[] = {
	NOT_A_PORT,
	(uint16_t) &P1IN,
	(uint16_t) &P2IN,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3IN,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4IN,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5IN,
#endif
#ifdef __MSP430_HAS_PORT6_R__
	(uint16_t) &P6IN,
#endif
#ifdef __MSP430_HAS_PORT7_R__
	(uint16_t) &P7IN,
#endif
#ifdef __MSP430_HAS_PORT8_R__
	(uint16_t) &P8IN,
#endif
};

const uint16_t port_to_output[] = {
	NOT_A_PORT,
	(uint16_t) &P1OUT,
	(uint16_t) &P2OUT,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3OUT,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4OUT,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5OUT,
#endif
#ifdef __MSP430_HAS_PORT6_R__
	(uint16_t) &P6OUT,
#endif
#ifdef __MSP430_HAS_PORT7_R__
	(uint16_t) &P7OUT,
#endif
#ifdef __MSP430_HAS_PORT8_R__
	(uint16_t) &P8OUT,
#endif
};

const uint16_t port_to_dir[] = {
	NOT_A_PORT,
	(uint16_t) &P1DIR,
	(uint16_t) &P2DIR,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3DIR,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4DIR,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5DIR,
#endif
#ifdef __MSP430_HAS_PORT6_R__
	(uint16_t) &P6DIR,
#endif
#ifdef __MSP430_HAS_PORT7_R__
	(uint16_t) &P7DIR,
#endif
#ifdef __MSP430_HAS_PORT8_R__
	(uint16_t) &P8DIR,
#endif
};

const uint16_t port_to_ren[] = {
	NOT_A_PORT,
	(uint16_t) &P1REN,
	(uint16_t) &P2REN,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3REN,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4REN,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5REN,
#endif
#ifdef __MSP430_HAS_PORT6_R__
	(uint16_t) &P6REN,
#endif
#ifdef __MSP430_HAS_PORT7_R__
	(uint16_t) &P7REN,
#endif
#ifdef __MSP430_HAS_PORT8_R__
	(uint16_t) &P8REN,
#endif
};

const uint16_t port_to_sel0[] = {  /* put this PxSEL register under the group of PxSEL0 */
	NOT_A_PORT,
	(uint16_t) &P1SEL,
	(uint16_t) &P2SEL,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3SEL,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4SEL,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5SEL,
#endif
#ifdef __MSP430_HAS_PORT6_R__
	(uint16_t) &P6SEL,
#endif
#ifdef __MSP430_HAS_PORT7_R__
	(uint16_t) &P7SEL,
#endif
#ifdef __MSP430_HAS_PORT8_R__
	(uint16_t) &P8SEL,
#endif
};

const uint16_t port_to_pmap[] = {
	NOT_A_PORT,	/* PMAP starts at port P1 */
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &P4MAP0,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
};

const uint8_t digital_pin_to_timer[] = {
	NOT_ON_TIMER, /*  dummy */
	NOT_ON_TIMER, /*  1 - 3.3v */
	T0B0,         /*  2 - P5.6 */
	NOT_ON_TIMER, /*  3 - P3.4 */
	NOT_ON_TIMER, /*  4 - P3.3 */
	NOT_ON_TIMER, /*  5 - P1.6 */
	NOT_ON_TIMER, /*  6 - P6.6 */
	NOT_ON_TIMER, /*  7 - P3.2 */
	NOT_ON_TIMER, /*  8 - P2.7 */
	NOT_ON_TIMER, /*  9 - P4.2 */
	NOT_ON_TIMER, /* 10 - P4.1 */
	NOT_ON_TIMER, /* 11 - P8.1 */
	T2A0,         /* 12 - P2.3 */
	NOT_ON_TIMER, /* 13 - P2.6 */
	NOT_ON_TIMER, /* 14 - P3.2 */
	NOT_ON_TIMER, /* 15 - P3.0 */
	NOT_ON_TIMER, /* 16 - RST */
	T0B2,         /* 17 - P7.4 */
	NOT_ON_TIMER, /* 18 - P2.2 */
	T1A1,         /* 19 - P2.0 */
	NOT_ON_TIMER, /* 20 - GND  */
	NOT_ON_TIMER, /* 21 - 5.0v */
	NOT_ON_TIMER, /* 22 - GND */
	NOT_ON_TIMER, /* 23 - P6.0 */
	NOT_ON_TIMER, /* 24 - P6.1 */
	NOT_ON_TIMER, /* 25 - P6.2 */
	NOT_ON_TIMER, /* 26 - P6.3 */
	NOT_ON_TIMER, /* 27 - P6.4 */
	NOT_ON_TIMER, /* 28 - P7.0 */
	T0B6,         /* 29 - P3.6 */
	T0B5,         /* 30 - P3.5 */
	NOT_ON_TIMER, /* 31 - P8.2 */
	NOT_ON_TIMER, /* 32 - P3.7 */
	NOT_ON_TIMER, /* 33 - P4.0 */
	NOT_ON_TIMER, /* 34 - P4.3 */
	T0A1,         /* 35 - P1.2 */
	T0A2,         /* 36 - P1.3 */
	T0A3,         /* 37 - P1.4 */
	T0A4,         /* 38 - P1.5 */
	T2A1,         /* 39 - P2.4 */
	T2A2,         /* 40 - P2.5 */
	NOT_ON_TIMER, /* 41 - P2.1 */
	NOT_ON_TIMER, /* 42 - P1.1 */
	NOT_ON_TIMER, /* 43 - P1.0 */
	T0B1,         /* 44 - P4.7 */
};

const uint8_t digital_pin_to_port[] = {
	NOT_A_PIN, /* dummy */
	NOT_A_PIN, /* 1 */
	P6,        /* 2 */
	P3,        /* 3 */
	P3,        /* 4 */
	P1,        /* 5 */
	P6,        /* 6 */
	P3,        /* 7 */
	P2,        /* 8 */
	P4,        /* 9 */
	P4,        /* 10 */
	P8,        /* 11 */
	P2,        /* 12 */
	P2,        /* 13 */
	P3,        /* 14 */
	P3,        /* 15 */
	NOT_A_PIN, /* 16 */
	P7,        /* 17 */
	P2,        /* 18 */
	P2,        /* 19 */
	NOT_A_PIN, /* 20 */
	NOT_A_PIN, /* 21 */
	NOT_A_PIN, /* 22 */
	P6,        /* 23 */
	P6,        /* 24 */
	P6,        /* 25 */
	P6,        /* 26 */
	P6,        /* 27 */
	P7,        /* 28 */
	P3,        /* 29 */
	P3,        /* 30 */
	P8,        /* 31 */
	P3,        /* 32 */
	P4,        /* 33 */
	P4,        /* 34 */
	P1,        /* 35 */
	P1,        /* 36 */
	P1,        /* 37 */
	P1,        /* 38 */
	P2,        /* 39 */
	P2,        /* 40 */
	P2,        /* 41 */
	P1,        /* 42 */
	P1,        /* 43 */
	P4,        /* 44 */
	P4,        /* 45 */
	P4,        /* 46 */
};

const uint8_t digital_pin_to_bit_mask[] = {
	NOT_A_PIN, /* 0,  pin count starts at 1 */
	NOT_A_PIN, /* 1 */
	BV(5),     /* 2 */
	BV(4),     /* 3 */
	BV(3),     /* 4 */
	BV(6),     /* 5 */
	BV(6),     /* 6 */
	BV(2),     /* 7 */
	BV(7),     /* 8 */
	BV(2),     /* 9 */
	BV(1),     /* 10 */
	BV(1),     /* 11 */
	BV(3),     /* 12 */
	BV(6),     /* 13 */
	BV(1),     /* 14 */
	BV(0),     /* 15 */
	NOT_A_PIN, /* 16 */
	BV(4),     /* 17 */
	BV(2),     /* 18 */
	BV(0),     /* 19 */
	NOT_A_PIN, /* 20 */
	NOT_A_PIN, /* 21 */
	NOT_A_PIN, /* 22 */
	BV(0),     /* 23 */
	BV(1),     /* 24 */
	BV(2),     /* 25 */
	BV(3),     /* 26 */
	BV(4),     /* 27 */
	BV(0),     /* 28 */
	BV(6),     /* 29 */
	BV(5),     /* 30 */
	BV(2),     /* 31 */
	BV(7),     /* 32 */
	BV(0),     /* 33 */
	BV(3),     /* 34 */
	BV(2),     /* 35 */
	BV(3),     /* 36 */
	BV(4),     /* 37 */
	BV(5),     /* 38 */
	BV(4),     /* 39 */
	BV(5),     /* 40 */
	BV(1),     /* 41 */
	BV(1),     /* 42 */
	BV(0),     /* 43 */
	BV(7),     /* 44 */
	BV(5),     /* 45 */
	BV(4),     /* 46 */
};
#endif // #ifdef ARDUINO_MAIN
#endif // #ifndef Pins_Energia_h
