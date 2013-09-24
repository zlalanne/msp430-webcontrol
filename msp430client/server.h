
#ifndef _server_h
#define _server_h

#define	MAX_CONFIGS 	16
#define MAX_PIN 		8
#define MAX_OPCODE		2
#define MAX_NAME		32
#define MAX_DATA		512

// Connection Details
static const char SSID[] = "ZL15801";
static const unsigned char PASSWORD[] = "15e9d02578";

#define IP_FIRST	192
#define IP_SECOND	168
#define IP_THIRD	0
#define IP_FOURTH	43

#define PORT_FIRST	0x1F
#define PORT_SECOND	0x9A

// Commands used to interact with server
#define DATA_HEADER			"{\"cmd\":\"data\",\"interfaces\":{"
#define DATA_HEADER_LEN		28
#define DATA_FOOTER			"}}"
#define DATA_FOOTER_LEN		2
#define CMD				"cmd"
#define REGISTER		"reg"
#define CONFIG 			"config"
#define PAYLOAD			"payload"
#define PIN				"pin"
#define OPCODE			"opcode"

#define CONFIG_OK		"c_ok"
#define RESUME_STREAM	"resume"

// Responses from server
#define ACK			"ack"
#define NACK		"nack"




#endif