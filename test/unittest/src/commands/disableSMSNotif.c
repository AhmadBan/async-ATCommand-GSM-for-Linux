/*
 * disableSMSNotif.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Ahmad
 */

#include "commands/baseCommand.h"
void disableNotif_ctor(Cmd_t* me){


	Cmd_t base={
			DISABLE_SMS_NOTIFICATION_SIG,//id
			0,//procid
			0,//priority
			"",//initCommand
			"AT+CNMI=0,0,0,0,0",//command[50]
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
	*me=base;
	base_ctor(me);

}

