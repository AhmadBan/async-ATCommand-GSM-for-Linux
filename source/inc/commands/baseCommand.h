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
	DISABLE_SMS_NOTIFICATION_SIG,
	DISABLE_CALL_SIG,
	READ_SMS_SIG,
	SMS_TEXT_MODE_SIG,
	DELETE_SMS_SIG,
	GET_OPERATOR_SIG,
	ECHO_SIG

};

typedef struct Cmd_t Cmd_t;

 struct Cmd_t{

	uint8_t id;
	uint8_t procId;
	uint8_t priority;
	char command[50]; //I think maximum length of a command must not be more than 50 character
	char finishParam[20];
	uint16_t initDelayMs;
	int32_t (*fpRequest)(struct Cmd_t* me);
	uint16_t sendDelayMs;
	int32_t (*fpResponse)(struct Cmd_t* me);
	char expectedAnswerOnSucessCommand[10];
	char expectedAnswerOnError[10];
	uint16_t receiveDelayMs;
	int32_t (*fpProc)(struct Cmd_t* me);
	int8_t retry;
	int32_t (*fpReset)(struct Cmd_t* me);
	int port;
	void (*fpCallBackOnSuccess)(Cmd_t* me);
	void (*fpCallBackOnError)(Cmd_t* me);
};

typedef  void (*pCtorFunc)(void);
void base_ctor(Cmd_t *pbase);
int baseCheckPort(struct Cmd_t* me);




//Send start command part of a command and take thread to sleep for sendDelayMs.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseReq(Cmd_t* me);



//send finish command (usually \r ) and wait for sim800 to respond.it also retries to get
//a response
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseRes(Cmd_t* me);

int32_t baseReset(Cmd_t* me);
//base procedures to send command and get result
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseProc(Cmd_t* me);
#endif /* INC_PACKETS_H_ */
