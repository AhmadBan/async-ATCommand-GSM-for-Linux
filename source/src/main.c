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

int serialPort;
char portAddress[]="/dev/ttyUSB0";


//global instantiation of Queue
MyQueue_t globalQ;




void sendExampleSMS(MyQueue_t *mq){
	char message[]="Hello World\n";
	char phone[]="989350542618";
	SMSPacket_T *sendSms=malloc(sizeof(SMSPacket_T));
	SMSSend_ctor((Cmd_t*)sendSms);
	sendSms->super.port=serialPort;
	memcpy(sendSms->message,message,strlen(message));
	memcpy(sendSms->phoneNumber,phone,strlen(phone));

	addToQueue((Cmd_t*)sendSms,mq);
}


// this is  sms consumer for queue
//It waits for a message in the queue if there is no message then waits for it
//else it get the messages and process them


int main(){
	pthread_t thread1;
	MyQueue_t *mq = &globalQ;
	serialPort=gsmSetup(portAddress,mq,&thread1);
	while(1){
		printf("test main\n");

		sleep(1);
	}

	return 0;
}
