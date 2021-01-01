/*
 * DeleteSMS.c
 *
 *  Created on: Jan 1, 2021
 *      Author: Ahmad
 */

#include <commands/deleteSMS.h>

void deleteSMS_ctor(Cmd_t* me){
	DeleteSMSPacket_T* delSMS=me;
	Cmd_t base={
			DISABLE_CALL_SIG,//id
			0,//procid
			0,//priority
			"",//initCommand
			"AT+CMGD=",//command[50]
			"\r",//finishParam[2];
			0,//fpInit
			1,//initDelayMs;
			0,	//fpSend
			1,//sendDelayMs
			0,//fpReceive
			"\r\nOK\r\n",//expectedAnswerOnSucess
			"\r\nERROR\r\n",//expectedAnswerOnError
			1,//receiveDelayMs
			0,//fpProc
			10,//retry
			0//	port
	};
	delSMS->super=base;
	delSMS->super.fpInit=baseInit;
	delSMS->super.fpSend=delCommand;
	delSMS->super.fpReceive=baseReceive(me);
	delSMS->super.fpProc=baseProc;


}


int32_t delCommand(Cmd_t* me){
	char commandNo[40]={0};
	char sms[512]={0};
	int size=0;
	int res=0;
	DeleteSMSPacket_T* delCmd=(DeleteSMSPacket_T*)me;
	size=sprintf(commandNo,"%s=%d",delCmd->super.command,delCmd->msgId);

	if(write(me->port, commandNo, size)==-1)
		return -1;
	sleep(me->sendDelayMs);

	return baseCheckPort(me);
}
