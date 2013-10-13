#include "board.h"
#include "server.h"
#include "server_protocol.h"
#include "interface.h"

#include "spi.h"
#include "wlan.h"
#include "hci.h"
#include "netapp.h"
#include "socket.h"
#include "nvmem.h"
#include "jsmn.h"
#include "driverlib.h"

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define NETAPP_IPCONFIG_MAC_OFFSET				(20)
#define HEADERS_SIZE_DATA       (SPI_HEADER_SIZE + 5)
#define HCI_CMND_SEND_ARG_LENGTH	(16)

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected, ulCC3000DHCP,
		OkToDoShutDown, ulCC3000DHCP_configured;
volatile unsigned char ucStopSmartConfig;

// Flag used for indicating when to sample
static bool StartSample = false;

// Keeps track of how many data messages have not been acked. Will stop streaming
// data if this number reaches 10
static uint8_t DataMsgCount = 0;

// Socket for communicating with backend server
static volatile long Socket;

// Function pointer that points to the current state the MSP430 is in
static void (*CurrentState)(void);

// States the MSP430 can be in
static void streamState(void);
static void configState(void);
static void registerState(void);


/**
 * recvLine
 *
 * Modified version of the BSD socket recv function that buffers the
 * data and returns a string only once a line ending boundary is found
 */
int recvLine(long sd, char *buf, long len, long flags) {

	static char buffer[MAX_RX_BUFFER];
	static uint16_t bufferUsed = 0;

	char *lineBegin = &buffer[0];
	char *lineEnd;

	uint16_t bufferRemain = MAX_RX_BUFFER - bufferUsed;

	bool foundBoundary = false;
	int16_t rv = 0;
	int16_t totalLength = 0;

	do {

		// Check if there is boundary in the current data
		lineEnd = strstr(buffer, "\r\n");

		if(lineEnd == NULL){

			bufferRemain = MAX_RX_BUFFER - bufferUsed;
			if(bufferRemain > 0) {
				rv = recv(sd, &buffer[bufferUsed], bufferRemain, flags);
			} else {
				// Get stuck here for now
				while(1);
			}

			if (rv <= 0) {
				// Error or no data, exit the function
				foundBoundary = true;
				totalLength = 0;
			} else {
				bufferUsed += rv;
				bufferRemain -= rv;
			}

		} else {

			// Copy the data to the buffer
			if((lineEnd - lineBegin) < len) {

				// Remove the newline characters
				*lineEnd = 0;
				*(lineEnd + 1) = 0;

				totalLength = lineEnd - lineBegin;
				memcpy(buf, lineBegin, totalLength);
				memmove(&buffer[0], lineEnd + 2, bufferUsed - (totalLength + 2));
				bufferUsed -= (totalLength + 2);
			} else {

				lineEnd = lineBegin + len + 1;
				totalLength = len;
				memcpy(buf, lineBegin, totalLength);
				memmove(&buffer[0], lineEnd, bufferUsed - totalLength);
				bufferUsed -= (totalLength);
			}

			foundBoundary = true;
		}

	} while(foundBoundary == false);

	return totalLength;
}

static int sendLine(long sd, const char *buf, long len, long flags) {

	char buffer[CC3000_TX_BUFFER_SIZE];

	bool dataSent = false;

	int16_t snd;
	int32_t dataLeft = len;
	int32_t chunkSize;
	int32_t dataLength;
	int32_t totalLength = 0;
	uint16_t bufferIndex = 0;

	do {

		if(dataLeft < CC3000_TX_BUFFER_SIZE - HCI_CMND_SEND_ARG_LENGTH - HEADERS_SIZE_DATA - 3) {
			dataLength = dataLeft;
			chunkSize = dataLeft + 2;
			memcpy(&buffer[0], &buf[bufferIndex], dataLength);
			buffer[dataLength] = '\r';
			buffer[dataLength + 1] = '\n';

			// Final packet chunk
			dataSent = true;
		} else {
			dataLength = CC3000_TX_BUFFER_SIZE - HCI_CMND_SEND_ARG_LENGTH - HEADERS_SIZE_DATA - 1;
			chunkSize = dataLength;
			memcpy(&buffer[0], &buf[bufferIndex], dataLength);
		}

		snd = send(sd, &buffer[0], chunkSize, flags);

		if(snd <= 0) {
			// Error occurred
			dataSent = true;
			totalLength = snd;
		} else if(snd == chunkSize) {
			dataLeft -= dataLength;
			totalLength += dataLength;
			bufferIndex += dataLength;
		} else {
			// CC3000 didn't send the whole data?
			while(1);
		}

	} while(dataSent == false);

	return totalLength;
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
//!		      device
//
//*****************************************************************************
void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length) {

	if (lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE) {
		ulSmartConfigFinished = 1;
		ucStopSmartConfig = 1;
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT) {
		ulCC3000Connected = 1;
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT) {
		ulCC3000Connected = 0;
		ulCC3000DHCP = 0;
		ulCC3000DHCP_configured = 0;
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_DHCP) {
		// Notes:
		// 1) IP config parameters are received swapped
		// 2) IP config parameters are valid only if status is OK, i.e. ulCC3000DHCP becomes 1

		// only if status is OK, the flag is set to 1 and the addresses are valid
		if (*(data + NETAPP_IPCONFIG_MAC_OFFSET)== 0){
			ulCC3000DHCP = 1;
		} else {
			ulCC3000DHCP = 0;
		}
	}

	if (lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN) {
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
static char *sendDriverPatch(unsigned long *Length) {
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
static char *sendBootLoaderPatch(unsigned long *Length) {
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
static char *sendWLFWPatch(unsigned long *Length) {
	*Length = 0;
	return NULL;
}

static void initHardware(void) {

	pio_init();
	init_spi();

	// WLAN On API Implementation
	wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch,
			sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable,
			WlanInterruptDisable, WriteWlanPin);

	// Trigger a WLAN device
	wlan_start(0);

	// Mask out all non-required events from CC3000
	wlan_set_event_mask(
			HCI_EVNT_WLAN_KEEPALIVE | HCI_EVNT_WLAN_UNSOL_INIT
					| HCI_EVNT_WLAN_ASYNC_PING_REPORT);

}

/**
 * System Tick Initialization. Fires a timer interrupt at 10Hz (every 100ms) to start sampling
 */
static void systickInit(void) {

    // Configure Timer
    // Set to 10Hz counter (ACLK/8 = 32768/8 = 4096)  (4096/10 ~= 410)
    TIMER_A_configureUpMode(SYSTICK_BASE,
    	TIMER_A_CLOCKSOURCE_ACLK,
    	TIMER_A_CLOCKSOURCE_DIVIDER_8,
    	409,
    	TIMER_A_TAIE_INTERRUPT_ENABLE,
    	TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
        TIMER_A_DO_CLEAR);
}

static void streamState(void) {

	int32_t status = 0;
	char rxBuffer[256];
	uint8_t acks;
	jsmntok_t tokens[128];
	jsmn_parser jsonParser;
	jsmnerr_t jsonStatus;

	// Waiting for new data or configuration
	status = recvLine(Socket, rxBuffer, 256, 0);
	if(status > 0) {

		// Parse JSON
		jsmn_init(&jsonParser);
		rxBuffer[status] = '\0';
		jsonStatus = jsmn_parse(&jsonParser, rxBuffer, tokens, 128);

		if(jsonStatus == JSMN_SUCCESS) {

			// Determine command
			switch(SERVER_parseStreamData(rxBuffer, tokens)) {
			case 'a':
				// Data ACK
				acks = SERVER_getACKs(rxBuffer, tokens);
				DataMsgCount -= acks;
				break;
			case 'w':
				// Write Data
				SERVER_writeData(rxBuffer, tokens);
				break;
			case 'd':
				// Drop to configuration state
				if(SERVER_dropToConfig(rxBuffer, tokens) == true) {
					CurrentState = configState;
					TIMER_A_stop(SYSTICK_BASE);
					StartSample = false;

					// Tell backend we are dropping to configuration
					sendLine(Socket, DROPCONFIGOK, sizeof(DROPCONFIGOK) - 1, 0);
				}
				break;
			case 'r':
				// Resume Streaming
				// TODO: Not implemented yet
				break;
			}
		}
	}
}

static void configState(void) {

	char rxBuffer[256];
	jsmn_parser jsonParser;
	jsmntok_t tokens[128];
	int32_t status = 0;
	jsmnerr_t jsonStatus;
	bool parseStatus;

	typedef enum {
		RECV_CONFIG,
		SEND_CONFIG_OK,
		RECV_RESUME
	} step_t;

	static step_t step = RECV_CONFIG;

	switch(step) {

	// Trying to receive the configuration
	case RECV_CONFIG:

		status = recvLine(Socket, rxBuffer, 256, 0);

		if (status > 0) {
			jsmn_init(&jsonParser);

			// Parse JSON
			rxBuffer[status] = '\0';
			jsonStatus = jsmn_parse(&jsonParser, rxBuffer, tokens, 128);

			// Checking to make sure it's a configuration
			if(jsonStatus == JSMN_SUCCESS) {
				parseStatus = SERVER_parseConfig(rxBuffer, tokens);
			} else {
				parseStatus = false;
			}

			// Putting the configuration in place
			if(parseStatus == true) {
				parseStatus = SERVER_setConfig(rxBuffer, tokens);
			}

			if (parseStatus == true) {
				step = SEND_CONFIG_OK;
			}
		}
		break;

	// Sending that the configuration is valid
	case SEND_CONFIG_OK:

		do {
			status = sendLine(Socket, CONFIG_OK, 4, 0);
		} while (status != 4);

		SERVER_initInterfaces();

		step = RECV_RESUME;
		break;

	// Waiting for resume streaming
	case RECV_RESUME:

		status = recvLine(Socket, rxBuffer, 256, 0);

		if(status > 0) {

			// Parse JSON
			rxBuffer[status] = '\0';
			jsonStatus = jsmn_parse(&jsonParser, rxBuffer, tokens, 128);

			if (jsonStatus == JSMN_SUCCESS) {
				parseStatus = SERVER_resumeStream(rxBuffer, tokens);
			} else {
				parseStatus = false;
			}

			if(parseStatus == true) {
				DataMsgCount = 0;
				CurrentState = streamState;

				// Start Timer
				TIMER_A_clear(SYSTICK_BASE);
				TIMER_A_startCounter(SYSTICK_BASE,
					TIMER_A_UP_MODE);

				// Start at RECV_CONFIG if we get reconfigured
				step = RECV_CONFIG;
			}
		}
		break;
	}
}

/**
 * Register State
 */
static void registerState(void) {

	char txBuffer[64] = REGISTER;
	uint8_t mac[6];
	uint16_t dataLength;
	char rxBuffer[8];
	int32_t status = 0;
	sockaddr socketAddress;

	// Steps for this state
	typedef enum {
		CONNECT_SOCKET,
		SEND_REG,
		ACK_REG_REQUEST,
		SEND_MAC,
		ACK_MAC
	} step_t;

	static step_t step = CONNECT_SOCKET;

	// Determine which step of registration we are on
	switch(step) {

	// Connect socket to server
	case CONNECT_SOCKET:
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
			status = connect(Socket, &socketAddress, sizeof(sockaddr));
		} while (status == -1);

		step = SEND_REG;
		break;

	// Send registration request
	case SEND_REG:

		// Sending the register request - 'reg'
		dataLength = 3;
		do {
			status = sendLine(Socket, txBuffer, dataLength, 0);
		} while (status != dataLength);

		step = ACK_REG_REQUEST;
		break;

	// Get registration request ACK
	case ACK_REG_REQUEST:

		// Waiting for the register request ack
		do {
			status = recvLine(Socket, rxBuffer, 8, 0);
		} while (status < 1);

		// Check if we got an ACK
		if (memcmp(ACK, rxBuffer, 3) != 0) {
			step = SEND_REG;
		} else {
			step = SEND_MAC;
		}
		break;

	// Send the MAC address
	case SEND_MAC:

		do {
			status = nvmem_get_mac_address(mac);
		} while (status != 0);

		// Get the MAC address and convert to ASCII string
		dataLength = sprintf(&txBuffer[0], "%x:%x:%x:%x:%x:%x", mac[0], mac[1],
				mac[2], mac[3], mac[4], mac[5]);

		do {
			status = sendLine(Socket, txBuffer, dataLength, 0);
		} while (status != dataLength);

		step = ACK_MAC;
		break;

	// Get MAC address ACK
	case ACK_MAC:

		do {
			status = recvLine(Socket, rxBuffer, 8, 0);
		} while (status < 1);

		// Check if we got an ACK and we are now registered
		if (memcmp(ACK, rxBuffer, 3) == 0) {
			CurrentState = configState;

			// Reset the step in case we need to re-register
			step = CONNECT_SOCKET;
		} else {
			step = SEND_MAC;
		}
		break;
	}
}


int main(void) {

	char data[MAX_DATA] = DATA_HEADER;
	uint16_t dataLength;
	int32_t status = 0;

	// Timeout for receive in ms
	int32_t timeout = 100;

	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

	initHardware();
	systickInit();

	// Disconnect from anything already connected
	wlan_disconnect();

	// Connecting to network
	wlan_connect(WLAN_SEC_UNSEC, (char*) SSID, 6, NULL,
			(unsigned char*) PASSWORD, 0);

	while ((ulCC3000Connected == 0) || (ulCC3000DHCP == 0));

	P1OUT |= BIT0;

	do {
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	} while (Socket == -1);

	do {
		status = setsockopt(Socket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &timeout, sizeof(timeout));
	} while(status == -1);

	CurrentState = registerState;

	while (1) {
		CurrentState();

		if((CurrentState == &streamState) && (StartSample == true) && (DataMsgCount < 10)) {

			// Start the sample
			StartSample = false;
			dataLength = SERVER_sendData(data);

			// Send the data
			do {
				status = sendLine(Socket, data, dataLength, 0);
			} while (status != dataLength);
			DataMsgCount++;

			// Heartbeat for sending messages
			P1OUT ^= BIT0;
		}
	}
}

#pragma vector = SYSTICK_VECTOR
__interrupt void SYSTICK(void) {
	switch (__even_in_range(TA0IV, 0x0E)) {
	case 0x02:
		break; // Vector 0x0: CCR1
	case 0x04:
		break; // Vector 0x02: CCR2
	case 0x06:
		break; // Vector 0x04: CCR3
	case 0x08:
		break; // Vector 0x06: CCR4
	case 0x0A:
		break; // Vector 0x08: CCR5
	case 0x0C:
		break; // Vector 0x0A: CCR6
	case 0x0E: // Vector 0x0E: TAxCTL TAIFG
		StartSample = true; // Start a new sample
		__bic_SR_register_on_exit(LPM4_bits);
		// Exit LPM on exit
		break;
	default:
		break;
	}
}

