/*
 * readSMS.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Ahmad
 */
#include "commands/readSMS.h"

void readSMS_ctor(Cmd_t* me){


	Cmd_t base={
			READ_SMS_SIG,//id
			0,//procid
			0,//priority
			"AT+CMGL=\"ALL\"",//command[50]
			"AT+CMGD",//finishParam[10];
			0,	//fpSend
			1,//sendDelayMs
			0,//fpReceive
			1,//receiveDelayMs
			"+CMGL:",//expectedAnswerOnSucess
			"\r\nERROR\r\n",//expectedAnswerOnError
			0,//fpProc
			1,//retry
			0,//	port
			0,//user callback on success
			0//user callback on error
	};
	*me=base;

	me->fpReset=baseReset;
	me->fpRequest=baseReq;
	me->fpResponse=readSMSResponse;
	me->fpProc=readSMSProc;
}




int32_t readSMSResponse(Cmd_t* me){

	int size=0;

	char command[]="\r";
	char output[30];
	char msg[768]={0};
	msg[767]=0;//make sure last character is null
	ReadSMS_t* readSMS=(ReadSMS_t*)me;

	if(write(me->port, command, strlen(command))==-1){
		if(readSMS->super.fpCallBackOnError!=NULL)
			readSMS->super.fpCallBackOnError((Cmd_t*)readSMS);

		return -1;
	}
	for(int i=0;i<me->retry;i++){//attempt 10 times in worst cases
		sleep(me->respDelayMs);
		size=read(me->port, msg, 767);
		//		if(size<0)
		//		{
		//			if(readSMS->super.fpCallBackOnError!=NULL)
		//				readSMS->super.fpCallBackOnError((Cmd_t*)readSMS);
		//
		//			return -1;
		//		}
		if(size>0)
			break;

		if(i==me->retry-1)
			me->fpReset(me);//all retry failed so reset sim800l
	}

	//if program reach here then means it received something
	if(strstr(msg,me->expectedAnswerOnError)!= NULL){
		if(readSMS->super.fpCallBackOnError!=NULL)
			readSMS->super.fpCallBackOnError((Cmd_t*)readSMS);

		return 1;
	}
	//			\r\n
	//			+CMGL: 1,"REC READ","+989350542618","","20/12/10,21:46:48+14"\r\n
	//			Hello world this is 2\n
	//			This is 3\n
	//			\n
	//			\r\n

	char* pos;
	pos=strstr(msg,"\r\n+CMGL:");
	if(pos!=NULL)
		size=sscanf(pos,"\r\n+CMGL:%d,\"%[^\"]\",\"%[^\"]\",\"\",\"%[^,],%[^\"]\"\r\n",&readSMS->sms.msgId,readSMS->sms.status,readSMS->sms.phoneNumber,readSMS->sms.date,readSMS->sms.hour);
	else
		return 0;
	pos=strstr(pos+2,"\r\n");
	if(pos!=NULL)
		size=sscanf(pos+2,"%[^\r]",readSMS->sms.message);
	else
		return 0;
	/*
	 * TODO:to delete Read SMS we can not use queue to send a delete command
	 * rather we better delete the SMS while we read it synchronously because a
	 * another READ_SMS_SIG command may be already in queue in a higher order
	 * before we send the delete command and thus we may read a SMS twice and
	 * that might be interpreted as we received it twice unless one considers
	 * Date and Time in readSMSCallback to prevent it.
	 */


	return 0;
}


int32_t readSMSProc(Cmd_t* me){

	int32_t res,callBackReturn=0;
	ReadSMS_t* readSMS=(ReadSMS_t*)me;
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
		callBackReturn=me->fpCallBackOnSuccess(me);
	if(callBackReturn==0){
			DeleteSMSPacket_T* delSMS=malloc(sizeof(DeleteSMSPacket_T));
			deleteSMS_ctor((Cmd_t*)delSMS);
			delSMS->super.port=readSMS->super.port;
			delSMS->msgId=readSMS->sms.msgId;
			delSMS->super.fpCallBackOnError=readSMS->super.fpCallBackOnError;
			delSMS->super.fpProc(delSMS);
		}
	return 0;

}


