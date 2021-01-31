/*
 * main.c
 *
 *  Created on: Dec 16, 2020
 *      Author: Ahmad
 */
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <myQueue.h>
#include <commands/baseCommand.h>
#include <Config.h>
#include "gsm.h"


char portAddress[]="/dev/ttyUSB0";
GSM_t *gsm;

//global instantiation of Queue
MyQueue_t globalQ;
/*
 * this callback executes in case of reading sms error
 * or problem in deleting read sms
 */
int readSMSErrorCallback(Cmd_t* me){
	/*
	 * do something for handling error here
	 * check if me arg has the id equal to zero it means error in reading stage
	 * otherwise error in deleting message occurs
	 */
	return 0;
}
/*
 * this callback executes in case of reading sms sucessfully
 * if it returns 0 the read SMS will be deleted otherwise nothing will be
 * happened
 */
int readSMSSuccessCallback(Cmd_t* me){
	ReadSMS_t* newSMS=(ReadSMS_t*)me;

	printf("SMS Date is %s\n",newSMS->sms.date);
	printf("SMS Hour is %s\n",newSMS->sms.hour);
	printf("SMS is from %s\n",newSMS->sms.phoneNumber);
	printf("SMS status is %s\n",newSMS->sms.status);
	printf("SMS id is %d\n",newSMS->sms.msgId);
	printf("SMS message is %s\n",newSMS->sms.message);

	/*
	 * you can also delete message yourself here
	 * in this case you must return none zero value
	 */
//	DeleteSMSPacket_T* delSMS=malloc(sizeof(DeleteSMSPacket_T));
//	deleteSMS_ctor((Cmd_t*)delSMS);
//	delSMS->msgId=readSMS->sms.msgId;
//	delSMS->super.fpCallBackOnError=yourCallbackFunctionOnError;
//	delSMS->super.fpProc(delSMS);
//	return 1;

	return 0;

}

void sendExampleSMS(GSM_t *gsm){
	char message[]="Ahmad.Baneshi@gmail.com\n My Github github.com/AhmadBan\n My LinkedIn linkedin.com/in/ahmad-banshee/";
	char phone[]="989350542618";
	SMSPacket_T *sendSms=malloc(sizeof(SMSPacket_T));
	SMSSend_ctor((Cmd_t*)sendSms);
	sendSms->super.port=gsm->port;
	memcpy(sendSms->message,message,strlen(message));
	memcpy(sendSms->phoneNumber,phone,strlen(phone));

	addToQueue((Cmd_t*)sendSms,&gsm->mq);
}


// this is  sms consumer for queue
//It waits for a message in the queue if there is no message then waits for it
//else it get the messages and process them


int main(){
	gsm=gsmSetup(portAddress,10);
	sendExampleSMS(gsm);
	while(1){
		printf("test main\n");
		sleep(1);
	}

	return 0;
}
