/*
 * gsm.c
 *
 *  Created on: Jan 7, 2021
 *      Author: Ahmad
 */
#include "gsm.h"
volatile int gsmReady=0;
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

void getOperator(Cmd_t* operator){
	GetOperator_t* me=(GetOperator_t*)operator;
	if(me->operator[0]!=0){
		//fflush( stdout );
		printf("operator is %s\n",me->operator);
		gsmReady=1;
	}
	else{
		gsmReady=0;
	}

}


static int initGSM(char * portAddress,MyQueue_t *mq){

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
	addToQueue(smsNotifDisable,mq);

	Cmd_t *disableCall=malloc(sizeof(Cmd_t));
	disableCall_ctor(disableCall);
	disableCall->port=serialPort;
	addToQueue(disableCall,mq);
	//enable text mode sms
	Cmd_t *textSMS=malloc(sizeof(Cmd_t));
	textModeSMS_ctor(textSMS);
	textSMS->port=serialPort;
	addToQueue(textSMS,mq);

	return serialPort;

}



int gsmSetup(char * portAddress,MyQueue_t *mq,pthread_t *thread){

	int status;         // Status Variable to store the Status of the thread.
	initQueue(mq);

	status = pthread_create(thread, NULL, gsmLoop, mq);
	/*  status = 0 ==> If thread is created Sucessfully.
	           status = 1 ==> If thread is unable to create.   */
	if(!status){
		printf("Custom Created Successfully.\n");
	}else{
		printf("Unable to create the Custom Thread.\n");
		return -1;
	}
	return initGSM(portAddress,mq);
}


void *gsmLoop(void *myQ){
	MyQueue_t* mq=myQ;
		pthread_mutex_lock(&mq->mu_queue);
		while(1){
		if(!isEmpty(mq))
		{
			Cmd_t* cmd =(Cmd_t*)peek( mq);
			printf("gsm thread executing command %d\n",cmd->id);
			if(cmd->fpProc(cmd)!=1)
				free(removeData( mq));//delete processed data from queue
			pthread_mutex_unlock(&mq->mu_queue);

		}
		else
		{

				printf("waiting for condition\n");
				pthread_cond_wait(&mq->cond, &mq->mu_queue);

		}
	}
	pthread_exit(NULL);
}
