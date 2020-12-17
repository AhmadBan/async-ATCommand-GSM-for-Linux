/*
 * packets.h
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */

#ifndef INC_PACKETS_H_
#define INC_PACKETS_H_
#include <stdint.h>
enum{
	SMS_SEND_SIG=0,


};

typedef struct Packet_t Packet_t;

struct Packet_t{

	uint8_t id;
	uint8_t procId;
	uint8_t priority;
	char initCommand[50];
	char command[50]; //I think all command is not more than 50 character
	// sim800 supports up to 512 character buffer so if at worse case
	//50 character used for command 512-50=458 characters remain for
	//parameter which I use 450 character
	char commandParam[450];
	char finishParam[10];
	int32_t (*fpInit)(struct Packet_t* this);
	uint16_t initDelayMs;
	int32_t (*fpSend)(struct Packet_t* this);
	uint16_t sendDelayMs;
	int32_t (*fpReceive)(struct Packet_t* this);
	char expectedAnswerOnSucessCommand[100];
	char expectedAnswerOnError[100];
	uint16_t receiveDelayMs;
	int32_t (*fpProc)(struct Packet_t* this);
	int8_t retry;
	void (*fpReset)(void);
	int port;
};




#endif /* INC_PACKETS_H_ */
