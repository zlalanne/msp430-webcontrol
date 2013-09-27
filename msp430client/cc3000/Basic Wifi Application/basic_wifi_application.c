/*****************************************************************************
*
*  basic_wifi_application.c - CC3000 Slim Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/


#include "wlan.h"
#include "evnt_handler.h"
#include "nvmem.h"
#include "socket.h"
#include "netapp.h"
#include "spi.h"
#include "dispatcher.h"
#include "spi_version.h"
#include "board.h"
#include "application_version.h"
#include "host_driver_version.h"
#include <msp430.h>
#include "security.h"


#define PALTFORM_VERSION						(6)

extern int uart_have_cmd;

#define UART_COMMAND_CC3000_SIMPLE_CONFIG_START                  (0x31)

#define UART_COMMAND_CC3000_CONNECT                              (0x32)

#define UART_COMMAND_SOCKET_OPEN                                 (0x33)

#define UART_COMMAND_SEND_DATA				  	 (0x34)

#define UART_COMMAND_RCV_DATA				  	 (0x35)

#define UART_COMMAND_BSD_BIND					 (0x36)

#define UART_COMMAND_SOCKET_CLOSE			 	 (0x37)

#define UART_COMMAND_IP_CONFIG			 	 	 (0x38)

#define UART_COMMAND_CC3000_DISCONNECT		 	         (0x39)

#define UART_COMMAND_CC3000_DEL_POLICY                           (0x61)

#define UART_COMMAND_SEND_DNS_ADVERTIZE                         (0x62)

#define CC3000_APP_BUFFER_SIZE						(5)

#define CC3000_RX_BUFFER_OVERHEAD_SIZE			(20)

#define DISABLE										(0)

#define ENABLE										(1)

#define SL_VERSION_LENGTH							(11)

#define NETAPP_IPCONFIG_MAC_OFFSET				(20)

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected,ulCC3000DHCP,
OkToDoShutDown, ulCC3000DHCP_configured;

volatile unsigned char ucStopSmartConfig;

volatile long ulSocket;

// Simple Config Prefix
const char aucCC3000_prefix[] = {'T', 'T', 'T'};


// Indications that UART command has finished etc
const unsigned char pucUARTCommandDoneString[] = {'\f', '\r', 'D', 'O', 'N', 'E', '\f', '\r'};
const unsigned char pucUARTCommandSmartConfigDoneString[] = {'\f', '\r','S', 'm', 'a', 'r', 't',' ', 'c', 'o', 'n', 'f', 'i', 'g', ' ',  'D', 'O', 'N', 'E', '\f', '\r'};
const unsigned char pucUARTExampleAppString[] = {'\f', '\r','E', 'x', 'a', 'm', 'p', 'l', 'e', ' ', 'A', 'p', 'p', ':', 'd', 'r', 'i', 'v', 'e', 'r', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', ' ' };
const unsigned char pucUARTNoDataString[] = {'\f', '\r', 'N', 'o', ' ', 'd', 'a', 't', 'a', ' ', 'r', 'e', 'c','e', 'i', 'v', 'e', 'd', '\f', '\r'};
const unsigned char pucUARTIllegalCommandString[] = {'\f', '\r', 'I', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'c', 'o','m', 'm', 'a', 'n', 'd', '\f', '\r'};

//device name used by smart config response
char device_name[] = "home_assistant";

//AES key "smartconfigAES16"
const unsigned char smartconfigkey[] = {0x73,0x6d,0x61,0x72,0x74,0x63,0x6f,0x6e,0x66,0x69,0x67,0x41,0x45,0x53,0x31,0x36};

unsigned char printOnce = 1;

char digits[] = "0123456789";


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//__no_init is used to prevent the buffer initialization in order to prevent hardware WDT expiration    ///
// before entering to 'main()'.                                                                         ///
//for every IDE, different syntax exists :          1.   __CCS__ for CCS v5                             ///
//                                                  2.  __IAR_SYSTEMS_ICC__ for IAR Embedded Workbench  ///
// *CCS does not initialize variables - therefore, __no_init is not needed.                             ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Reception from the air, buffer - the max data length  + headers
#ifdef __CCS__

unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];

#elif __IAR_SYSTEMS_ICC__

__no_init unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];

#endif





//*****************************************************************************
//
//! itoa
//!
//! @param[in]  integer number to convert
//!
//! @param[in/out]  output string
//!
//! @return number of ASCII parameters
//!
//! @brief  Convert integer to ASCII in decimal base
//
//*****************************************************************************
unsigned short itoa(char cNum, char *cString)
{
	char* ptr;
	char uTemp = cNum;
	unsigned short length;

	// value 0 is a special case
	if (cNum == 0)
	{
		length = 1;
		*cString = '0';

		return length;
	}

	// Find out the length of the number, in decimal base
	length = 0;
	while (uTemp > 0)
	{
		uTemp /= 10;
		length++;
	}

	// Do the actual formatting, right to left
	uTemp = cNum;
	ptr = cString + length;
	while (uTemp > 0)
	{
		--ptr;
		*ptr = digits[uTemp % 10];
		uTemp /= 10;
	}

	return length;
}
//*****************************************************************************
//
//! atoc
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Convert nibble to hexdecimal from ASCII
//
//*****************************************************************************
unsigned char
atoc(char data)
{
	unsigned char ucRes;

	if ((data >= 0x30) && (data <= 0x39))
	{
		ucRes = data - 0x30;
	}
	else
	{
		if (data == 'a')
		{
			ucRes = 0x0a;;
		}
		else if (data == 'b')
		{
			ucRes = 0x0b;
		}
		else if (data == 'c')
		{
			ucRes = 0x0c;
		}
		else if (data == 'd')
		{
			ucRes = 0x0d;
		}
		else if (data == 'e')
		{
			ucRes = 0x0e;
		}
		else if (data == 'f')
		{
			ucRes = 0x0f;
		}
	}


	return ucRes;
}


//*****************************************************************************
//
//! atoshort
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Convert 2 nibbles in ASCII into a short number
//
//*****************************************************************************

unsigned short
atoshort(char b1, char b2)
{
	unsigned short usRes;

	usRes = (atoc(b1)) * 16 | atoc(b2);

	return usRes;
}

//*****************************************************************************
//
//! ascii_to_char
//!
//! @param  b1 first byte
//! @param  b2 second byte
//!
//! @return The converted character
//!
//! \brief  Convert 2 bytes in ASCII into one character
//
//*****************************************************************************

unsigned char
ascii_to_char(char b1, char b2)
{
	unsigned char ucRes;

	ucRes = (atoc(b1)) << 4 | (atoc(b2));

	return ucRes;
}


//*****************************************************************************
//
//! sendDriverPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the driver patch:
//!         since there is no patch in the host - it returns 0
//
//*****************************************************************************
char *sendDriverPatch(unsigned long *Length)
{
	*Length = 0;
	return NULL;
}


//*****************************************************************************
//
//! sendBootLoaderPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the boot loader patch:
//!         since there is no patch in the host - it returns 0
//
//*****************************************************************************
char *sendBootLoaderPatch(unsigned long *Length)
{
	*Length = 0;
	return NULL;
}

//*****************************************************************************
//
//! sendWLFWPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the FW patch:
//!         since there is no patch in the host - it returns 0
//
//*****************************************************************************

char *sendWLFWPatch(unsigned long *Length)
{
	*Length = 0;
	return NULL;
}

//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! @param  lEventType Event type
//! @param  data
//! @param  length
//!
//! @return none
//!
//! @brief  The function handles asynchronous events that come from CC3000
//!		      device and operates a LED4 to have an on-board indication
//
//*****************************************************************************

void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
{

	if (lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE)
	{
		ulSmartConfigFinished = 1;
		ucStopSmartConfig     = 1;
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT)
	{
		ulCC3000Connected = 1;

		// Turn on the LED2
		turnLedOn(LED2);
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
	{
		ulCC3000Connected = 0;
		ulCC3000DHCP      = 0;
		ulCC3000DHCP_configured = 0;
		printOnce = 1;

		// Turn off the LED3
		turnLedOff(LED2);
		// Turn off LED3
		turnLedOff(LED3);

	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
	{
		// Notes:
		// 1) IP config parameters are received swapped
		// 2) IP config parameters are valid only if status is OK, i.e. ulCC3000DHCP becomes 1

		// only if status is OK, the flag is set to 1 and the addresses are valid
		if ( *(data + NETAPP_IPCONFIG_MAC_OFFSET) == 0)
		{
			sprintf( (char*)pucCC3000_Rx_Buffer,"IP:%d.%d.%d.%d\f\r", data[3],data[2], data[1], data[0] );

			ulCC3000DHCP = 1;

			turnLedOn(LED3);
		}
		else
		{
			ulCC3000DHCP = 0;

			turnLedOff(LED3);
		}

	}

	if (lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN)
	{
		OkToDoShutDown = 1;
	}

}

//*****************************************************************************
//
//! initDriver
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function initializes a CC3000 device and triggers it to
//!          start operation
//
//*****************************************************************************
int
initDriver(void)
{
	// Init GPIO's
	pio_init();

	// Init SPI
	init_spi();

	DispatcherUARTConfigure();

	// WLAN On API Implementation
	wlan_init( CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);

	// Trigger a WLAN device
	wlan_start(0);

	// Turn on the LED 1 to indicate that we are active and initiated WLAN successfully
	turnLedOn(LED1);
	// Mask out all non-required events from CC3000
	wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);

	// Generate event to CLI: send a version string
	{
		char cc3000IP[50];
		char *ccPtr;
		unsigned short ccLen;

		DispatcherUartSendPacket((unsigned char*)pucUARTExampleAppString, sizeof(pucUARTExampleAppString));

		ccPtr = &cc3000IP[0];
		ccLen = itoa(PALTFORM_VERSION, ccPtr);
		ccPtr += ccLen;
		*ccPtr++ = '.';
		ccLen = itoa(APPLICATION_VERSION, ccPtr);
		ccPtr += ccLen;
		*ccPtr++ = '.';
		ccLen = itoa(SPI_VERSION_NUMBER, ccPtr);
		ccPtr += ccLen;
		*ccPtr++ = '.';
		ccLen = itoa(DRIVER_VERSION_NUMBER, ccPtr);
		ccPtr += ccLen;
		*ccPtr++ = '\f';
		*ccPtr++ = '\r';
		*ccPtr++ = '\0';


		DispatcherUartSendPacket((unsigned char*)cc3000IP, strlen(cc3000IP));
	}

	ucStopSmartConfig   = 0;

	return(0);
}

//*****************************************************************************
//
//! StartSmartConfig
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function triggers a smart configuration process on CC3000.
//!			it exists upon completion of the process
//
//*****************************************************************************

void StartSmartConfig(void)
{
	ulSmartConfigFinished = 0;
	ulCC3000Connected = 0;
	ulCC3000DHCP = 0;
	OkToDoShutDown=0;

	// Reset all the previous configuration
	wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
	wlan_ioctl_del_profile(255);

	//Wait until CC3000 is disconnected
	while (ulCC3000Connected == 1)
	{
		__delay_cycles(100);
		hci_unsolicited_event_handler();
	}

	// Start blinking LED6 during Smart Configuration process
	turnLedOn(LED1);
	wlan_smart_config_set_prefix((char*)aucCC3000_prefix);
	turnLedOff(LED1);

	// Start the SmartConfig start process
	wlan_smart_config_start(1);

	turnLedOn(LED1);

	// Wait for Smartconfig process complete
	while (ulSmartConfigFinished == 0)
	{

		__delay_cycles(6000000);

		turnLedOff(LED1);

		__delay_cycles(6000000);

		turnLedOn(LED1);

	}

	turnLedOn(LED1);

	// create new entry for AES encryption key
	nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);

	// write AES key to NVMEM
	aes_write_key((unsigned char *)(&smartconfigkey[0]));

	// Decrypt configuration information and add profile
	wlan_smart_config_process();

	// Configure to connect automatically to the AP retrieved in the
	// Smart config process
	wlan_ioctl_set_connection_policy(DISABLE, DISABLE, ENABLE);

	// reset the CC3000
	wlan_stop();

	__delay_cycles(6000000);

	DispatcherUartSendPacket((unsigned char*)pucUARTCommandSmartConfigDoneString, sizeof(pucUARTCommandSmartConfigDoneString));

	wlan_start(0);

	// Mask out all non-required events
	wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}

//*****************************************************************************
//
//! DemoHandleUartCommand
//!
//!  @param  usBuffer
//!
//!  @return none
//!
//!  @brief  The function handles commands arrived from CLI
//
//*****************************************************************************
void
DemoHandleUartCommand(unsigned char *usBuffer)
{
	char *pcSsid, *pcData, *pcSockAddrAscii;
	unsigned long ulSsidLen, ulDataLength;
	volatile signed long iReturnValue;
	sockaddr tSocketAddr;
	socklen_t tRxPacketLength;
	unsigned char pucIP_Addr[4];
	unsigned char pucIP_DefaultGWAddr[4];
	unsigned char pucSubnetMask[4];
	unsigned char pucDNS[4];


	// usBuffer[0] contains always 0
	// usBuffer[1] maps the command
	// usBuffer[2..end] optional parameters
	switch(usBuffer[1])
	{
		// Start a smart configuration process
	case UART_COMMAND_CC3000_SIMPLE_CONFIG_START:
		StartSmartConfig();
		break;

		// Start a WLAN Connect process
	case UART_COMMAND_CC3000_CONNECT:
		{
			ulSsidLen = atoc(usBuffer[2]);
			pcSsid = (char *)&usBuffer[3];

#ifndef CC3000_TINY_DRIVER
			wlan_connect(WLAN_SEC_UNSEC, pcSsid, ulSsidLen,NULL, NULL, 0);
#else

			wlan_connect(pcSsid,ulSsidLen);
#endif
		}
		break;

		// Handle open socket command
	case UART_COMMAND_SOCKET_OPEN:
		//wait for DHCP process to finish (if you are using a static IP address please delete the wait for DHCP event - ulCC3000DHCP
		while ((ulCC3000DHCP == 0) || (ulCC3000Connected == 0))
		{
			hci_unsolicited_event_handler();

			__delay_cycles(1000);
		}
		ulSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		break;

		// Handle close socket command
	case UART_COMMAND_SOCKET_CLOSE:
		closesocket(ulSocket);
		ulSocket = 0xFFFFFFFF;
		break;

		// Handle receive data command
	case UART_COMMAND_RCV_DATA:
		iReturnValue = recvfrom(ulSocket, pucCC3000_Rx_Buffer, CC3000_APP_BUFFER_SIZE, 0, &tSocketAddr, &tRxPacketLength);
		if (iReturnValue <= 0)
		{
			// No data received by device
			DispatcherUartSendPacket((unsigned char*)pucUARTNoDataString, sizeof(pucUARTNoDataString));
		}
		else
		{
			// Send data to UART...
			DispatcherUartSendPacket(pucCC3000_Rx_Buffer, CC3000_APP_BUFFER_SIZE);
		}
		break;


		// Handle send data command
	case UART_COMMAND_SEND_DATA:

		// data pointer
		pcData = (char *)&usBuffer[4];

		// data length to send
		ulDataLength = atoshort(usBuffer[2], usBuffer[3]);
		pcSockAddrAscii = (pcData + ulDataLength);

		// the family is always AF_INET
		tSocketAddr.sa_family = atoshort(pcSockAddrAscii[0], pcSockAddrAscii[1]);

		// the destination port
		tSocketAddr.sa_data[0] = ascii_to_char(pcSockAddrAscii[2], pcSockAddrAscii[3]);
		tSocketAddr.sa_data[1] = ascii_to_char(pcSockAddrAscii[4], pcSockAddrAscii[5]);

		// the destination IP address
		tSocketAddr.sa_data[2] = ascii_to_char(pcSockAddrAscii[6], pcSockAddrAscii[7]);
		tSocketAddr.sa_data[3] = ascii_to_char(pcSockAddrAscii[8], pcSockAddrAscii[9]);
		tSocketAddr.sa_data[4] = ascii_to_char(pcSockAddrAscii[10], pcSockAddrAscii[11]);
		tSocketAddr.sa_data[5] = ascii_to_char(pcSockAddrAscii[12], pcSockAddrAscii[13]);

		sendto(ulSocket, pcData, ulDataLength, 0, &tSocketAddr, sizeof(sockaddr));
		break;

		// Handle bind command
	case UART_COMMAND_BSD_BIND:
		tSocketAddr.sa_family = AF_INET;

		// the source port
		tSocketAddr.sa_data[0] = ascii_to_char(usBuffer[2], usBuffer[3]);
		tSocketAddr.sa_data[1] = ascii_to_char(usBuffer[4], usBuffer[5]);

		// all 0 IP address
		memset (&tSocketAddr.sa_data[2], 0, 4);

		bind(ulSocket, &tSocketAddr, sizeof(sockaddr));

		break;

		// Handle IP configuration command
	case UART_COMMAND_IP_CONFIG:

		// Network mask is assumed to be 255.255.255.0
		pucSubnetMask[0] = 0xFF;
		pucSubnetMask[1] = 0xFF;
		pucSubnetMask[2] = 0xFF;
		pucSubnetMask[3] = 0x0;

		pucIP_Addr[0] = ascii_to_char(usBuffer[2], usBuffer[3]);
		pucIP_Addr[1] = ascii_to_char(usBuffer[4], usBuffer[5]);
		pucIP_Addr[2] = ascii_to_char(usBuffer[6], usBuffer[7]);
		pucIP_Addr[3] = ascii_to_char(usBuffer[8], usBuffer[9]);

		pucIP_DefaultGWAddr[0] = ascii_to_char(usBuffer[10], usBuffer[11]);
		pucIP_DefaultGWAddr[1] = ascii_to_char(usBuffer[12], usBuffer[13]);
		pucIP_DefaultGWAddr[2] = ascii_to_char(usBuffer[14], usBuffer[15]);
		pucIP_DefaultGWAddr[3] = ascii_to_char(usBuffer[16], usBuffer[17]);


		// Currently no implementation of DHCP in hte demo
		pucDNS[0] = 0;
		pucDNS[1] = 0;
		pucDNS[2] = 0;
		pucDNS[3] = 0;

		netapp_dhcp((unsigned long *)pucIP_Addr, (unsigned long *)pucSubnetMask, (unsigned long *)pucIP_DefaultGWAddr, (unsigned long *)pucDNS);

		break;

		// Handle WLAN disconnect command
	case UART_COMMAND_CC3000_DISCONNECT:
		wlan_disconnect();
		break;


		// Handle erase policy command
	case UART_COMMAND_CC3000_DEL_POLICY:
		wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
		break;


		// Handle send DNS Discovery command
	case UART_COMMAND_SEND_DNS_ADVERTIZE:
		if(ulCC3000DHCP)
		{
			mdnsAdvertiser(1,device_name,strlen(device_name));
		}

		break;

	default:
		DispatcherUartSendPacket((unsigned char*)pucUARTIllegalCommandString, sizeof(pucUARTIllegalCommandString));
		break;

	}

	// Send a response  - the command handling has finished
	DispatcherUartSendPacket((unsigned char *)(pucUARTCommandDoneString), sizeof(pucUARTCommandDoneString));
}



//*****************************************************************************
//
//! main
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The main loop is executed here
//
//*****************************************************************************

main(void)
{
	ulCC3000DHCP = 0;
	ulCC3000Connected = 0;
	ulSocket = 0;
	ulSmartConfigFinished=0;


	WDTCTL = WDTPW + WDTHOLD;

	//  Board Initialization start
	initDriver();

	// Initialize the UART RX Buffer
	memset(g_ucUARTBuffer, 0xFF, UART_IF_BUFFER);
	uart_have_cmd =0;


	// Loop forever waiting  for commands from PC...

	while(1)
	{
		if ( uart_have_cmd )
		{
			///Process the command in RX buffer
			DemoHandleUartCommand(g_ucUARTBuffer);
			uart_have_cmd = 0;
			memset(g_ucUARTBuffer, 0xFF, UART_IF_BUFFER);
		}

		// complete smart config process:
		// 1. if smart config is done
		// 2. CC3000 established AP connection
		// 3. DHCP IP is configured
		// then send mDNS packet to stop external SmartConfig application
		if ((ucStopSmartConfig == 1) && (ulCC3000DHCP == 1))
		{
			unsigned char loop_index = 0;

			while (loop_index < 3)
			{
				mdnsAdvertiser(1,device_name,strlen(device_name));
				loop_index++;
			}

			ucStopSmartConfig = 0;
		}

		if( (ulCC3000DHCP == 1) && (ulCC3000Connected == 1)  && (printOnce == 1) )
		{
			printOnce = 0;
			DispatcherUartSendPacket((unsigned char*)pucCC3000_Rx_Buffer, strlen((char const*)pucCC3000_Rx_Buffer));
		}

	}
}

