/*
 * packets.c
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */

#include <commands/baseCommand.h>

#include <termios.h>
//after each command it checks the port for error.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int baseCheckPort(Cmd_t* me){
	char temp[100]={0};
	int size;


	size=read(me->port, temp, 100);

	if(size>0)
	{
		if(strstr(temp, "ERROR") != NULL)
			return 1;
	}
	return 0;
}

//Send start command part of a command and take thread to sleep for sendDelayMs.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseReq(Cmd_t* me){
	if(write(me->port, me->command, strlen(me->command))==-1)
		return -1;
	sleep(me->reqDelayMs);
	return baseCheckPort(me);
}
//send finish command (usually \r ) and wait for sim800 to respond.it also retries to get
//a response
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseRes(Cmd_t* me){
	char content[100]={0};
	int size=0;
	int32_t res=0;
	res=write(me->port, me->finishParam, strlen(me->finishParam));
	if(res==-1)//write finish command and inform to process
		return -1;
	for(int i=0;i<me->retry;i++){//attempt 10 times in worst cases
		sleep(me->respDelayMs);
		size=read(me->port, content, 100);
//		if(size<0)
//			return -1;
		if(size>0)
			break;

		if(i==me->retry-1)
			me->fpReset(me);//all retry failed so reset sim800l
	}

	//if program reach here then means it received something
	if(strstr(content,me->expectedAnswerOnError)!= NULL)
		return 1;
	else

		return 0;
}

//base procedures to send command and get result
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseProc(Cmd_t* me){

	int32_t res;
	int status= tcflush(me->port, TCIFLUSH);
	res=me->fpRequest(me);
	if(res!=0){
		if(me->fpCallBackOnError!=NULL)
			me->fpCallBackOnError(me);
		return res;
	}
	res=me->fpResponse(me);
	if(res!=0){
		if(me->fpCallBackOnError!=NULL)
			me->fpCallBackOnError(me);
		return res;
	}
	if(me->fpCallBackOnSuccess!=NULL)
		me->fpCallBackOnSuccess(me);
	return 0;

}


int32_t baseReset(Cmd_t* me){
	printf("implement a base reset here\n");
	return 1;
}


//this is the default constructor that can be used in each command
//to initialize function pointers by default functions

void base_ctor(Cmd_t *pbase){
	pbase->fpRequest=baseReq;
	pbase->fpResponse=baseRes;
	pbase->fpProc=baseProc;
}
