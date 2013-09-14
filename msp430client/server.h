
#ifndef _server_h
#define _server_h

// Connection Details
#define IP_FIRST	192
#define IP_SECOND	168
#define IP_THIRD	1
#define IP_FOURTH	100

#define PORT_FIRST	0x1F
#define PORT_SECOND	0x9A

// Commands used to interact with server
#define CMD				"cmd"
#define REGISTER		"reg"
#define CONFIG 			"config"
#define PAYLOAD			"payload"
#define READ			"read"
#define WRITE			"write"
#define CONFIG_OK		"c_ok"
#define RESUME_STREAM	"resume_s"

// Responses from server
#define ACK			"ack"
#define NACK		"nack"




#endif
