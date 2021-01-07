/*
 * DeleteSMS.c
 *
 *  Created on: Jan 1, 2021
 *      Author: Ahmad
 */

#include <commands/deleteSMS.h>

void deleteSMS_ctor(Cmd_t* me){
	DeleteSMSPacket_T* delSMS=(DeleteSMSPacket_T*)me;
	Cmd_t base={
			DELETE_SMS_SIG,//id
			0,//procid
			0,//priority
			"AT+CMGD=",//command[50]
			"\r",//finishParam[2];
			0,	//fpSend
			1,//sendDelayMs
			0,//fpReceive
			1,//receiveDelayMs
			"\r\nOK\r\n",//expectedAnswerOnSucess
			"\r\nERROR\r\n",//expectedAnswerOnError

			0,//fpProc
			10,//retry
			0,//	port
			0,//user callback on success
			0//user callback on error
	};
	delSMS->super=base;
	delSMS->super.fpRequest=delCommand;
	delSMS->super.fpResponse=baseRes;
	delSMS->super.fpProc=baseProc;


}


int32_t delCommand(Cmd_t* me){
	char commandNo[70]={0};
	int size=0;

	DeleteSMSPacket_T* delCmd=(DeleteSMSPacket_T*)me;
	size=sprintf(commandNo,"%s=%d",delCmd->super.command,delCmd->msgId);
	if(write(me->port, commandNo, size)==-1)
		return -1;
	sleep(me->reqDelayMs);
	return baseCheckPort(me);
}
