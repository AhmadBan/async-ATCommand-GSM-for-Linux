/*
 * packets.c
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */

#include <commands/baseCommand.h>

//after each command it checks the port for error.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int baseCheckPort(Cmd_t* me){
	char temp[100];
	int size;
	 char cwd[255];
	   if (getcwd(cwd, sizeof(cwd)) != NULL) {
	       printf("Current working dir: %s\n", cwd);
	   } else {
	       perror("getcwd() error");
	       return 1;
	   }
	size=read(me->port, temp, 100);
	if(size<0){
		perror("File not Found.!");

		return -1;
	}
	else if(size>0)
	{
		if(strstr(temp, "ERROR") != NULL)
			return 1;
	}
	return 0;
}

//This function init sim800 to get ready before main command
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseInit(Cmd_t* me){
	if(write(me->port, me->initCommand, strlen(me->initCommand))==-1)
		return -1;
	sleep(me->initDelayMs);
	return baseCheckPort(me);
}

//Send start command part of a command and take thread to sleep for sendDelayMs.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseSend(Cmd_t* me){

	if(write(me->port, me->command, sizeof(me->command))==-1)
		return -1;
	sleep(me->sendDelayMs);

	return baseCheckPort(me);
}
//send finish command (usually \r ) and wait for sim800 to respond.it also retries to get
//a response
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseReceive(Cmd_t* me){
	char content[100];
	int size=0;
	int32_t res=0;
	res=write(me->port, me->finishParam, strlen(me->finishParam));
	if(res==-1)//write finish command and inform to process
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

//base procedures to send command and get result
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseProc(Cmd_t* me){
	int32_t res;
	res=baseInit(me);
	if(res!=0)
		return res;

	res=baseSend(me);
	if(res!=0)
		return res;

	res=baseReceive(me);
	if(res!=0)
		return res;

	return 0;

}


int32_t baseReset(Cmd_t* me){
	printf("implement a base reset here");
	return 1;
}


//this is the default constructor that can be used in each command
//to initialize function pointers by default functions

void base_ctor(Cmd_t *pbase){

	pbase->fpInit=baseInit;
	pbase->fpSend=baseSend;
	pbase->fpReceive=baseReceive;
	pbase->fpProc=baseProc;


}
