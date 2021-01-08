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

	fd = open(portAddress, O_RDWR | O_NOCTTY | O_NDELAY|O_NONBLOCK);
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
	options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON); //tty is the name of the struct termios
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
	free(operator);
}


void initGSM(GSM_t* gsm){

	//check if GSM connects to network by checking operator name
	printf("initGSM Function\n");
	printf("getting Operator\n");
	GetOperator_t *getOp=calloc(1,sizeof(GetOperator_t));
	getOperator_ctor((Cmd_t*)getOp);
	getOp->super.port=gsm->port;
	getOp->super.fpCallBackOnSuccess=getOperator;
	do{
		getOp->super.fpProc((Cmd_t*)getOp);
	}
	while(gsmReady!=1);//wait until GSM module find an operator

	//DISABLE CALL AND SMS NOTIFICATION SO IT DOES NOT INTERRUPT ANSWER FROM A COMMAND
	printf("disabling sms notificationr\n");
	Cmd_t *smsNotifDisable=calloc(1,sizeof(Cmd_t));
	disableSMSNotif_ctor(smsNotifDisable);
	addToQueue(smsNotifDisable,&gsm->mq);
	printf("disabling call notificationr\n");
	Cmd_t *disableCall=calloc(1,sizeof(Cmd_t));
	disableCall_ctor(disableCall);
	addToQueue(disableCall,&gsm->mq);
	//enable text mode sms
	printf("change sms mode to text\n");
	Cmd_t *textSMS=calloc(1,sizeof(Cmd_t));
	textModeSMS_ctor(textSMS);
	addToQueue(textSMS,&gsm->mq);



}



GSM_t* gsmSetup(char * portAddress,int polligInSMSPeriod){
	printf("gsmSetup function\n");
	int status;         // Status Variable to store the Status of the thread.
	static GSM_t gsm;
	static pthread_t queueThread,readSMSPollingThread;
	initQueue(&gsm.mq);
	gsm.inputSMSPollingPeriod=polligInSMSPeriod;
	gsm.port=open_port(portAddress);
	if(gsm.port==-1){
		return NULL;
	}
	printf("creating q thread\n");
	status = pthread_create(&queueThread, NULL, gsmQueueExecution, &gsm);
	/*  status = 0 ==> If thread is created Sucessfully.
	           status = 1 ==> If thread is unable to create.   */
	if(!status){
		printf("gsmQueueExecution Created Successfully.\n");
	}else{
		printf("Unable to create the Custom Thread.\n");
		return NULL;
	}
	initGSM(&gsm);
	printf("creating Poll thread\n");
	status = pthread_create(&readSMSPollingThread, NULL, gsmPollingReadSMS, &gsm);
	/*  status = 0 ==> If thread is created Sucessfully.
		           status = 1 ==> If thread is unable to create.   */
	if(!status){
		printf("gsmPollingReadSMS Created Successfully.\n");
	}else{
		printf("Unable to create the Custom Thread.\n");
		return NULL;
	}


	return &gsm;
}


void *gsmQueueExecution(void * gsm){
	printf("gsmQueueExecution function\n");
	GSM_t* lgsm=gsm;
	pthread_mutex_lock(&lgsm->mq.mu_queue);
	while(1){
		if(!isEmpty(&lgsm->mq))
		{
			Cmd_t* cmd =(Cmd_t*)peek( &lgsm->mq);
			cmd->port=lgsm->port;
			printf("gsm thread executing command %d\n",cmd->id);
			if(cmd->fpProc(cmd)!=1)
				free(removeData( &lgsm->mq));//delete processed data from queue
			pthread_mutex_unlock(&lgsm->mq.mu_queue);

		}
		else
		{

			printf("waiting for condition\n");
			pthread_cond_wait(&lgsm->mq.cond, &lgsm->mq.mu_queue);

		}
	}
	pthread_exit(NULL);
}
extern int readSMSSuccessCallback(Cmd_t* me);
extern int readSMSErrorCallback(Cmd_t* me);

void *gsmPollingReadSMS(void *gsm){
	printf("gsmPollingReadSMS function\n");
	GSM_t* lgsm=gsm;
	while(1){
		ReadSMS_t* readsSMS=calloc(1,sizeof(ReadSMS_t));

		readSMS_ctor((Cmd_t*)readsSMS);
		readsSMS->super.fpCallBackOnSuccess=readSMSSuccessCallback;
		readsSMS->super.fpCallBackOnError=readSMSErrorCallback;

		addToQueue((Cmd_t*)readsSMS,&lgsm->mq);
		printf("add sms read event to queue\n");
		sleep(lgsm->inputSMSPollingPeriod);
	}
}


