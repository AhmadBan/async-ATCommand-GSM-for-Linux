/*
 * smsSendPackets.c
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */


#include <commands/smsSendCommand.h>

SMSPacket_T smsSend;



//Send start command part of a command and take thread to sleep for sendDelayMs.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
 int32_t SMSSend(Cmd_t* me){
	char commandNo[40]={0};
	char sms[512]={0};
	int size=0;
	int res=0;
	SMSPacket_T* currentSMS=(SMSPacket_T*)me;
	size=sprintf(commandNo,"%s=\"%s\"\r",currentSMS->super.command,currentSMS->phoneNumber);

	if(write(me->port, commandNo, size)==-1)
		return -1;
	sleep(me->sendDelayMs);

	return baseCheckPort(me);
}
//send finish command (usually \r ) and wait for sim800 to respond.it also retries to get
//a response
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t SMSReceiveResponse(Cmd_t* me){
	char sms[512]={0};
	char content[100]={0};
	int size=0;
	int32_t res=0;
	SMSPacket_T* currentSMS=(SMSPacket_T*)me;
	size=sprintf(sms,"%s%s",currentSMS->message,currentSMS->super.finishParam);
	if(write(me->port, sms, size)==-1)
		return -1;
	for(int i=0;i<me->retry;i++){//attempt 10 times in worst cases
		sleep(me->receiveDelayMs);
		size=read(me->port, content, 100);
		if(size<0)
			return -1;
		else if(size>0)
			break;

		if(i==me->retry-1)
			me->fpReset(me);//all retry failed so reset sim800l
	}

	//if program reach here then means it received something
	if(strstr(content,"OK")== NULL)
		return 1;
	else
		return 0;
}
void SMSSend_ctor(Cmd_t* me){
	SMSPacket_T *sms=me;
	sms->super.id=SMS_SEND_SIG;
	//psmsSend->super.DelayMs=3;//3 seconds
	Cmd_t base={
			SMS_SEND_SIG,//id
			0,//procid
			0,//priority
			"",//initCommand
			"AT+CMGS",//command[50]
			"\032",//finishParam[2];
			0,//fpInit
			1,//initDelayMs;
			0,	//fpSend
			1,//sendDelayMs
			0,//fpReceive
			"+CMGS: %d\r\n\r\nOK\r\n",//expectedAnswerOnSucess
			"",//expectedAnswerOnError
			5,//receiveDelayMs
			0,//fpProc
			10,//retry
			0//	port
	};

	sms->super=base;
	sms->super.fpInit=baseInit;
	sms->super.fpSend=SMSSend;
	sms->super.fpReceive=SMSReceiveResponse;
	sms->super.fpProc=baseProc;

}