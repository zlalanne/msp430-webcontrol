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

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected,ulCC3000DHCP,
OkToDoShutDown, ulCC3000DHCP_configured;

volatile unsigned char ucStopSmartConfig;

unsigned char printOnce = 1;

#define NETAPP_IPCONFIG_MAC_OFFSET				(20)


volatile long ulSocket;

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


static void initHardware(void) {

	pio_init();
	init_spi();


	// WLAN On API Implementation
	wlan_init( CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);

	// Trigger a WLAN device
	wlan_start(0);

	turnLedOn(LED1);
	// Mask out all non-required events from CC3000
	wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}

static const char SSID[] =  "MSP430";
static const unsigned char PASSWORD[] = "";

unsigned short
atoshort(char b1, char b2)
{
	unsigned short usRes;

	usRes = (atoc(b1)) * 16 | atoc(b2);

	return usRes;
}

unsigned char
ascii_to_char(char b1, char b2)
{
	unsigned char ucRes;

	ucRes = (atoc(b1)) << 4 | (atoc(b2));

	return ucRes;
}

static void (*CurrentState)(void);

static void streamState(void){

	__no_operation();

}
static void configState(void){

	char rxBuffer[256];
	jsmn_parser jsonParser;
	jsmntok_t tokens[128];
	int32_t status = 0;
	jsmnerr_t jsonStatus;
	bool parseStatus;

	jsmn_init(&jsonParser);

	// Wait for configuration
	do{
		status = recv(ulSocket, rxBuffer, 256, 0);

		if(status != -1){

			// Parse JSON
			rxBuffer[status] = '\0';
			jsonStatus = jsmn_parse(&jsonParser, rxBuffer, tokens, 128);

			if(jsonStatus == JSMN_SUCCESS){
				parseStatus = SERVER_parseConfig(rxBuffer, tokens);
			}

			if(parseStatus == true){
				parseStatus = SERVER_setConfig(rxBuffer, tokens);
			}

			if(parseStatus == true){
				do {
					status = send(ulSocket, CONFIG_OK, 4, 0);
				} while(status != 4);

				CurrentState = streamState;
			}
		}
	} while(status == -1);

	// Wait until resume streaming
	do{
		status = recv(ulSocket, rxBuffer, 256, 0);

		// Parse JSON
		rxBuffer[status] = '\0';
		jsonStatus = jsmn_parse(&jsonParser, rxBuffer, tokens, 128);

		if(jsonStatus == JSMN_SUCCESS){
			parseStatus = SERVER_resumeStream(rxBuffer, tokens);
		}

		if(parseStatus == false) {
			status = -1;
		}

	} while(status == -1);


}

static void registerState(void){

	char txBuffer[64] = REGISTER;
	uint8_t mac[6];
	uint16_t dataLength;
	char rxBuffer[4];
	int32_t status = 0;
	uint8_t i;

	sockaddr socketAddress;

	// The family is always AF_INET
	socketAddress.sa_family = AF_INET;

	// The destination port
	socketAddress.sa_data[0] = PORT_FIRST;
	socketAddress.sa_data[1] = PORT_SECOND;

	// The destination IP address
	socketAddress.sa_data[2] = IP_FIRST;
	socketAddress.sa_data[3] = IP_SECOND;
	socketAddress.sa_data[4] = IP_THIRD;
	socketAddress.sa_data[5] = IP_FOURTH;

	do {
		status = connect(ulSocket,  &socketAddress, sizeof(sockaddr));
	} while(status == -1);

	dataLength = 3;
	do {
		status = send(ulSocket, txBuffer, dataLength, 0);
	} while(status != dataLength);

	do {
		status = recv(ulSocket, rxBuffer, 4, 0);
	} while(status == -1);

	// Check if we got something besides an ACK
	if(strncmp(ACK, rxBuffer, 3) != 0) {
		return;
	}

	do {
		status = nvmem_get_mac_address(mac);
	} while(status != 0);

	// Get the MAC address and convert to ASCII string
	dataLength = 0;
	for(i = 0; i < 6; i++){
		status = sprintf(&txBuffer[dataLength], "%x", mac[i]);
		dataLength += status;
		txBuffer[dataLength] = ':';
		dataLength++;
	}

	// Trim off the last ":"
	dataLength--;

	do {
		status = send(ulSocket, txBuffer, dataLength, 0);
	} while(status != dataLength);

	do {
		status = recv(ulSocket, rxBuffer, 4, 0);
	} while(status == -1);

	// Check if we got an ACK and we are now registered
	if(strncmp(ACK, rxBuffer, 3) == 0) {
		CurrentState = configState;
	}
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    initHardware();

    wlan_connect(WLAN_SEC_UNSEC, (char*) SSID, 6, NULL, (unsigned char*) PASSWORD, 0);

	// Turn on the LED2
	turnLedOn(LED2);

    while((ulCC3000Connected == 0) || (ulCC3000DHCP == 0));

    do{
    	ulSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    } while(ulSocket == -1);

    CurrentState = registerState;

	while(1) {
		CurrentState();
	}
}

