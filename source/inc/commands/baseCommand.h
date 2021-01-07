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

	uint8_t id;//unique number for every command
	uint8_t procId;//reserved
	uint8_t priority;//reserved
	char command[50]; //I think maximum length of a command must not be more than 50 character
	char finishParam[20];//contains parameter for command
	int32_t (*fpRequest)(struct Cmd_t* me);//function pointer for method that send request command
	uint16_t reqDelayMs;//delay for executing command in GSM module
	int32_t (*fpResponse)(struct Cmd_t* me);//function pointer for method that process response from gsm
	uint16_t respDelayMs;//delay for GSM to send responses
	char expectedAnswerOnSucessCommand[10];//part of expected answer if GSM processed request successfully
	char expectedAnswerOnError[10];//part of expected answer if GSM processed request unsuccessfully
	int32_t (*fpProc)(struct Cmd_t* me);//function pointer for method that manages request , responses
	int8_t retry;//number of attempts if request fails
	int32_t (*fpReset)(struct Cmd_t* me);//function pointer for method that reset GSM
	int port;//serial port file Descriptor
	void (*fpCallBackOnSuccess)(Cmd_t* me);//function pointer that executes if request return successful (it is assigned by user)
	void (*fpCallBackOnError)(Cmd_t* me);//function pointer that executes if request return unsuccessful (it is assigned by user)
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
