/*
 * disableSMSNotif.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Ahmad
 */

#include "commands/disableSMSNotif.h"
void disableSMSNotif_ctor(Cmd_t* me){


	Cmd_t base={
			DISABLE_SMS_NOTIFICATION_SIG,//id
			0,//procid
			0,//priority
			"AT+CNMI=0,0,0,0,0",//command[50]
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
	*me=base;
	base_ctor(me);

}

