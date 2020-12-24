/*
 * packets.h
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */

#ifndef INC_PACKETS_H_
#define INC_PACKETS_H_
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


enum{
	SMS_SEND_SIG=0,


};

typedef struct Command_t Command_t;

 struct Command_t{

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
	int32_t (*fpInit)(struct Command_t* me);
	uint16_t initDelayMs;
	int32_t (*fpSend)(struct Command_t* me);
	uint16_t sendDelayMs;
	int32_t (*fpReceive)(struct Command_t* me);
	char expectedAnswerOnSucessCommand[100];
	char expectedAnswerOnError[100];
	uint16_t receiveDelayMs;
	int32_t (*fpProc)(struct Command_t* me);
	int8_t retry;
	void (*fpReset)(void);
	int port;
	int32_t (*fpCtor)(struct Command_t* me);
};

typedef  void (*pCtorFunc)(void);
void base_ctor(Command_t *pbase);
int baseCheckPort(struct Command_t* me);


//This function init sim800 to get ready before main command
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseInit(Command_t* me);


//Send start command part of a command and take thread to sleep for sendDelayMs.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseSend(Command_t* me);



//send finish command (usually \r ) and wait for sim800 to respond.it also retries to get
//a response
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseReceive(Command_t* me);


//base procedures to send command and get result
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseProc(Command_t* me);
#endif /* INC_PACKETS_H_ */
