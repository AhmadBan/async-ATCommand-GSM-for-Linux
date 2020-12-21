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
int baseCheckPort(Command_t* this){
	char temp[100];
	int size;
	size=read(this->port, temp, 100);
	if(size<0)
		return -1;
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
int32_t baseInit(Command_t* this){
	if(write(this->port, this->initCommand, sizeof(this->initCommand))==-1)
		return -1;
	sleep(this->initDelayMs);
	return baseCheckPort(this);
}

//Send start command part of a command and take thread to sleep for sendDelayMs.
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseSend(Command_t* this){

	if(write(this->port, this->command, sizeof(this->command))==-1)
		return -1;
	sleep(this->sendDelayMs);

	return baseCheckPort(this);
}
//send finish command (usually \r ) and wait for sim800 to respond.it also retries to get
//a response
//returns -1 on Port problem
//returns 1 on error
//returns 0 on success
int32_t baseReceive(Command_t* this){
	char content[100];
	int size=0;
	int32_t res=0;
	if(write(this->port, this->finishParam, sizeof(this->finishParam))==-1)//write finish command and inform to process
		return -1;
	sleep(this->receiveDelayMs);//wait until sim process command
	res=baseCheckPort(this);
	if(res!=0)
		return res;


	for(int i=0;i<this->retry;i++){//attempt 10 times in worst cases
		size=read(this->port, content, 100);
		if(size<0)
			return -1;
		else if(size>0)
			break;
		sleep(this->receiveDelayMs);
		if(i==this->retry-1)
			this->fpReset();//all retry failed so reset sim800l
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
int32_t baseProc(Command_t* this){
	int32_t res;
	res=baseInit(this);
	if(res!=0)
		return res;

	res=baseSend(this);
	if(res!=0)
		return res;

	res=baseReceive(this);
	if(res!=0)
		return res;

	return 0;

}

//this is the default constructor that can be used in each command
//to initialize function pointers by default functions

void base_ctor(Command_t *pbase){

	pbase->fpInit=baseInit;
	pbase->fpSend=baseSend;
	pbase->fpReceive=baseReceive;
	pbase->fpProc=baseProc;

}
