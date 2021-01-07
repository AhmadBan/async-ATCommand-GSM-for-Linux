/*
 * getOperator.c
 *
 *  Created on: Jan 3, 2021
 *      Author: Ahmad
 */

#include "commands/getOperator.h"
void getOperator_ctor(Cmd_t* me){
	GetOperator_t *gop=(GetOperator_t *)me;
	Cmd_t base={
			GET_OPERATOR_SIG,//id
			0,//procedure id
			0,//priority
			"AT+COPS?",//command[50]
			"\r",//finishParam[2];
			0,	//fpSend
			1,//sendDelayMs
			0,//fpReceive
			1,//receiveDelayMs
			"\r\nOK\r\n",//expectedAnswerOnSucess
			"\r\nERROR\r\n",//expectedAnswerOnError
			0,//fpProc
			2,//retry
			0,//	port
			0,//user callback on success
			0//user callback on error
	};
	gop->super=base;

		gop->super.fpRequest=baseReq;
		gop->super.fpResponse=getOperatorResponse;
		gop->super.fpProc=baseProc;
		gop->super.fpReset=baseReset;

}

int32_t getOperatorResponse(Cmd_t* me){

	int size=0;
	int temp1,temp2;
	char* pos;
	char command[]="\r";
	char output[40];
	GetOperator_t* op=(GetOperator_t*)me;

	if(write(me->port, command, strlen(command))==-1)
		return -1;
	for(int i=0;i<me->retry;i++){//attempt 10 times in worst cases
		sleep(me->respDelayMs);
		size=read(me->port, output, 39);
		if(size<0)
			return -1;
		else if(size>0)
			break;

		if(i==me->retry-1)
			me->fpReset(me);//all retry failed so reset sim800l
	}

	//if program reach here then means it received something
	if(strstr(output,me->expectedAnswerOnError)!= NULL)
	{
		if(op->super.fpCallBackOnError!=NULL)
				op->super.fpCallBackOnError(op);
		return 1;

	}
	pos=strstr(output,"+COPS:");


	size=sscanf(pos,"+COPS: %d,%d,\"%[^\"]\"\r\n",&temp1,&temp2,op->operator);

	//delete the read message from memory


	return 0;

}
