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

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected, ulCC3000DHCP,
		OkToDoShutDown, ulCC3000DHCP_configured;

volatile unsigned char ucStopSmartConfig;

bool StartSample = false;

// Keeps track of how many data messages have not been acked. Will stop streaming
// data if this number reaches 10
uint8_t DataMsgCount = 0;

unsigned char printOnce = 1;

#define NETAPP_IPCONFIG_MAC_OFFSET				(20)

volatile long ulSocket;


int recvLine(long sd, char *buf, long len, long flags) {

	bool foundBoundary = false;
	int16_t status = 0;
	int16_t totalLength = 0;
	int32_t bufferLength = len;
	uint16_t currentIndex = 0;

	do {
		status = recv(sd, &buf[currentIndex], bufferLength, flags);
		totalLength += status;

		if(status >= 2) {

			// Check if we hit a boundary
			if((buf[totalLength - 2] == '\r') && (buf[totalLength - 1] == '\n')) {
				foundBoundary = true;
			} else {
				currentIndex += status;
				bufferLength -= status;
			}

		} else if(status == -1 && currentIndex == 0) {
			// Packet timeout, nothing received yet
			foundBoundary = true;

		} else if(status == -1 && currentIndex != 0) {
			// Started receiving some data but now getting a timeout
			// Need to remove the timeout status from the length
			totalLength += 1;
		}

	} while(foundBoundary == false);

	// Remove the \r\n from end of packet
	if(totalLength != -1) {
		totalLength--;
	}

	return totalLength;
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
unsigned char atoc(char data) {
	unsigned char ucRes;

	if ((data >= '0') && (data <= '9')) {
		ucRes = data - '0';
	} else {
		if (data == 'a') {
			ucRes = 0x0a;
		} else if (data == 'b') {
			ucRes = 0x0b;
		} else if (data == 'c') {
			ucRes = 0x0c;
		} else if (data == 'd') {
			ucRes = 0x0d;
		} else if (data == 'e') {
			ucRes = 0x0e;
		} else if (data == 'f') {
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
		printOnce = 1;
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

	turnLedOn(LED1);
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

unsigned short atoshort(char b1, char b2) {
	unsigned short usRes;

	usRes = (atoc(b1)) * 16 | atoc(b2);

	return usRes;
}

unsigned char ascii_to_char(char b1, char b2) {
	unsigned char ucRes;

	ucRes = (atoc(b1)) << 4 | (atoc(b2));

	return ucRes;
}

static void (*CurrentState)(void);

static void streamState(void) {

	int32_t status = 0;
	char rxBuffer[256];
	uint8_t acks;
	jsmntok_t tokens[128];
	jsmn_parser jsonParser;
	jsmnerr_t jsonStatus;

	// Waiting for new data or configuration
	status = recv(ulSocket, rxBuffer, 256, 0);
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
				// Drop to config state - Not implemented
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

		status = recv(ulSocket, rxBuffer, 256, 0);

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
			status = send(ulSocket, CONFIG_OK, 6, 0);
		} while (status != 6);

		SERVER_initInterfaces();

		step = RECV_RESUME;
		break;

	// Waiting for resume streaming
	case RECV_RESUME:

		status = recv(ulSocket, rxBuffer, 256, 0);

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
	char rxBuffer[4];
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
			status = connect(ulSocket, &socketAddress, sizeof(sockaddr));
		} while (status == -1);

		step = SEND_REG;
		break;

	// Send registration request
	case SEND_REG:

		// Sending the register request - 'reg'
		dataLength = 5;
		do {
			status = send(ulSocket, txBuffer, dataLength, 0);
		} while (status != dataLength);

		step = ACK_REG_REQUEST;
		break;

	// Get registration request ACK
	case ACK_REG_REQUEST:

		// Waiting for the register request ack
		do {
			status = recv(ulSocket, rxBuffer, 4, 0);
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
		dataLength = sprintf(&txBuffer[0], "%x:%x:%x:%x:%x:%x\r\n", mac[0], mac[1],
				mac[2], mac[3], mac[4], mac[5]);

		do {
			status = send(ulSocket, txBuffer, dataLength, 0);
		} while (status != dataLength);

		step = ACK_MAC;
		break;

	// Get MAC address ACK
	case ACK_MAC:

		do {
			status = recv(ulSocket, rxBuffer, 4, 0);
		} while (status < 1);

		// Check if we got an ACK and we are now registered
		if (memcmp(ACK, rxBuffer, 3) == 0) {
			CurrentState = configState;

			// Reset the step in case we need to re register
			step = CONNECT_SOCKET;
		} else {
			step = SEND_MAC;
		}
		break;
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
		StartSample = true; // Set the system tick flag
		__bic_SR_register_on_exit(LPM4_bits);
		// Exit LPM on exit
		break;
	default:
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
	wlan_connect(WLAN_SEC_WPA2, (char*) SSID, 7, NULL,
			(unsigned char*) PASSWORD, 10);

	while ((ulCC3000Connected == 0) || (ulCC3000DHCP == 0));

	do {
		ulSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	} while (ulSocket == -1);

	do {
		status = setsockopt(ulSocket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &timeout, sizeof(timeout));
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
				status = send(ulSocket, data, dataLength, 0);
			} while (status != dataLength);
			DataMsgCount++;

			// Heartbeat for sending messages
			P1OUT ^= BIT0;
		}
	}
}

