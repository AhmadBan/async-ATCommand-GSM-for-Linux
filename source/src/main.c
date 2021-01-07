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




/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int open_port(char* portAddress)
{
	int fd; /* File descriptor for the port */
	struct termios options;

	fd = open(portAddress, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{
		/*
		 * Could not open the port.
		 */

		perror("open_port: Unable to open serial port");
	}
	else
		fcntl(fd, F_SETFL, FNDELAY);


	/*
	 * Get the current options for the port...
	 */

	tcgetattr(fd, &options);

	/*
	 * Set the baud rates to 19200...
	 */

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	/*
	 * Enable the receiver and set local mode...
	 */

	options.c_cflag |= (CLOCAL | CREAD);
	///No parity (8N1):
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	options.c_cflag &= ~CRTSCTS;//disable hardware flow control
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//raw input disable echo
	options.c_iflag &= ~(IXON | IXOFF | IXANY);//disable software flow control
	options.c_oflag &= ~OPOST;//select raw output

	/*
	 * Set the new options for the port...
	 */

	tcsetattr(fd, TCSANOW, &options);

	return (fd);
}



//add command to queue
MyQueue_t globalQ;
void initQueue(MyQueue_t* myQ){
	myQ->rear=0;
	myQ->front=0;
	myQ->itemCount=0;
	pthread_mutex_init(&myQ->mu_queue, NULL);
	pthread_cond_init(&myQ->cond, NULL);
}
int addToQueue(Cmd_t* cmd,MyQueue_t* myQ){
	MyQueue_t *mq = myQ;
	pthread_mutex_lock(&mq->mu_queue);
	if(isFull(mq)){
		pthread_mutex_unlock(&mq->mu_queue);
		return -1;
	}
	insert(mq, cmd);
	pthread_mutex_unlock(&mq->mu_queue);
	pthread_cond_signal(&mq->cond);
	return mq->itemCount;
}

Cmd_t* waitForQueue(MyQueue_t* mq){
	pthread_mutex_lock(&mq->mu_queue);
	if(!isEmpty(mq))
	{
		Cmd_t* cmd =(Cmd_t*)peek( mq);

		pthread_mutex_unlock(&mq->mu_queue);
		return cmd;
	}
	else
	{
		pthread_cond_wait(&mq->cond, &mq->mu_queue);
	}
	return NULL;
}
char test[10];
volatile int gsmReady=0;
void getOperator(Cmd_t* operator){
	GetOperator_t* me=(GetOperator_t*)operator;
	if(me->operator[0]!=0){
		memcpy(test,me->operator,strlen(me->operator));
		fflush( stdout );
		printf("operator is %s",me->operator);
		gsmReady=1;
	}
	else{
		gsmReady=0;
	}

}

char portAddress[]="/dev/ttyUSB0";

int initGSM(void){

	//check if GSM connects to network by checking operator name
	GetOperator_t *getOp=malloc(sizeof(GetOperator_t));
	int serialPort=open_port(portAddress);
	if(serialPort==-1){
		free(getOp);
		return -1;
	}
	getOperator_ctor((Cmd_t*)getOp);
	getOp->super.port=serialPort;
	getOp->super.fpCallBackOnSuccess=getOperator;
	do{
		getOp->super.fpProc((Cmd_t*)getOp);
	}
	while(gsmReady!=1);//wait until GSM module find an operator

	//DISABLE CALL AND SMS NOTIFICATION SO IT DOES NOT INTERRUPT ANSWER FROM A COMMAND
	Cmd_t *smsNotifDisable=malloc(sizeof(Cmd_t));
	disableSMSNotif_ctor(smsNotifDisable);
	smsNotifDisable->port=serialPort;
	addToQueue(smsNotifDisable,&globalQ);

	Cmd_t *disableCall=malloc(sizeof(Cmd_t));
	disableCall_ctor(disableCall);
	disableCall->port=serialPort;
	addToQueue(disableCall,&globalQ);

	Cmd_t *textSMS=malloc(sizeof(Cmd_t));
	textModeSMS_ctor(textSMS);
	textSMS->port=serialPort;
	addToQueue(textSMS,&globalQ);

	char message[]="Hello World\n";
	char phone[]="989350542618";
	SMSPacket_T *sendSms=malloc(sizeof(SMSPacket_T));
	SMSSend_ctor(sendSms);
	sendSms->super.port=serialPort;
	memcpy(sendSms->message,message,strlen(message));
	memcpy(sendSms->phoneNumber,phone,strlen(phone));

	addToQueue(sendSms,&globalQ);

}


// this is  sms consumer for queue
//It waits for a message in the queue if there is no message then waits for it
//else it get the messages and process them


int main(){

	MyQueue_t *mq = &globalQ;
	//init all constructors
	if(initGSM()==-1)
		return -1;


	while(1)
	{
		pthread_mutex_lock(&mq->mu_queue);
		if(!isEmpty(mq))
		{
			Cmd_t* cmd =(Cmd_t*)peek( mq);
			if(cmd->fpProc(cmd)!=1)
					free(removeData( mq));//delete processed data from queue

			pthread_mutex_unlock(&mq->mu_queue);

		}
		else
		{
			pthread_cond_wait(&mq->cond, &mq->mu_queue);
		}
	}

	return 0;
}
