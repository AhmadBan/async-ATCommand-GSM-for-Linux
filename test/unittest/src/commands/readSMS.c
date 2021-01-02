/*
 * readSMS.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Ahmad
 */
#include "commands/readSMS.h"
#include <commands/deleteSMS.h>
void readSMS_ctor(Cmd_t* me){


	Cmd_t base={
			READ_SMS_SIG,//id
			0,//procid
			0,//priority
			"",//initCommand
			"AT+CMGL=\"ALL\"",//command[50]
			"AT+CMGD",//finishParam[10];
			0,//fpInit
			1,//initDelayMs;
			0,	//fpSend
			1,//sendDelayMs
			0,//fpReceive
			"+CMGL:",//expectedAnswerOnSucess
			"\r\nERROR\r\n",//expectedAnswerOnError
			1,//receiveDelayMs
			0,//fpProc
			1,//retry
			0//	port
	};
	*me=base;

	me->fpReset=baseReset;
	me->fpInit=baseInit;
	me->fpSend=baseSend;
	me->fpReceive=SMSReceiveAllMsg;
	me->fpProc=baseProc;
}




int32_t SMSReceiveAllMsg(Cmd_t* me){

	int size=0;
	int32_t res=0;
	char command[]="\r";
	char output[30];
	char msg[768]={0};
	msg[767]=0;//make sure last character is null
	ReadSMS_t* readSMS=me;

	if(write(me->port, command, strlen(command))==-1)
		return -1;
	for(int i=0;i<me->retry;i++){//attempt 10 times in worst cases
		sleep(me->receiveDelayMs);
		size=read(me->port, msg, 767);
		if(size<0)
			return -1;
		else if(size>0)
			break;

		if(i==me->retry-1)
			me->fpReset(me);//all retry failed so reset sim800l
	}

	//if program reach here then means it received something
	if(strstr(msg,me->expectedAnswerOnError)!= NULL)
		return 1;
	else{
		//			\r\n
		//			+CMGL: 1,"REC READ","+989350542618","","20/12/10,21:46:48+14"\r\n
		//			Hello world this is 2\n
		//			This is 3\n
		//			\n
		//			\r\n
		int size=0;
		char* pos;
		pos=strstr(msg,"\r\n+CMGL:");
		SMS_t* heapMsg=malloc(sizeof(SMS_t));
		size=sscanf(pos,"\r\n+CMGL:%d,\"%[^\"]\",\"%[^\"]\",\"\",\"%[^\,],%[^\"]\"\r\n",&heapMsg->msgId,heapMsg->status,heapMsg->phoneNumber,heapMsg->date,heapMsg->hour);
		pos=strstr(pos+2,"\r\n");
		size=sscanf(pos+2,"%[^\r]",heapMsg->message);
		if(readSMS->SMSReceivedCallBack!=NULL)
			readSMS->SMSReceivedCallBack(heapMsg);
		//delete the read message from memory
		size=sprintf(output,"%s=%d\r",me->finishParam,heapMsg->msgId);
		free(heapMsg);//free allocated memory
		if(write(me->port, output, size)==-1){
			return -1;
		}
		for(int i=0;i<me->retry;i++){//attempt 10 times in worst cases
			sleep(me->receiveDelayMs);
			size=read(me->port, msg, 20);
			if(size<0)
				return -1;
			else if(size>0)
				break;

			if(i==me->retry-1)
				me->fpReset(me);//all retry failed so reset sim800l
		}

		if(strstr(msg,me->expectedAnswerOnError)!= NULL)
				return 1;
		else
			return 0;

	}
	return 0;
}
