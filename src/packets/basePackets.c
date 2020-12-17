/*
 * packets.c
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */
#include <stddef.h>
#include <packets/basePackets.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int baseCheckPort(Packet_t* this){
	char temp[100];
	int size;
	size=read(this->port, temp, 100);
	if(size<0)
		return 1;
	else if(size>0)
	{
		if(strstr(temp, "ERROR") != NULL)
			return -1;
	}
	return 0;
}

int32_t baseSend(Packet_t* this){

	if(write(this->port, this->command, sizeof(this->command))==-1)
		return 1;
	sleep(this->sendDelayMs);
	if(baseCheckPort(this)!=0)
		return 1;
	return 0;
}

int32_t baseReceive(Packet_t* this){
	char content[100];
	int size=0;
	if(write(this->port, this->finishParam, sizeof(this->finishParam))==-1)//write finish command and inform to process
		return 1;
	sleep(this->receiveDelayMs);//wait until sim process command
	if(baseCheckPort(this)!=0)
		return 1;

	for(int i=0;i<this->retry;i++){//attempt 10 times in worst cases
		size=read(this->port, content, 100);
		if(size<0)
			return 1;
		else if(size>0)
			break;
		sleep(this->receiveDelayMs);
		if(i==this->retry-1)
			this->fpReset();//all retry failed so reset sim800l
	}

	//if program reach here then means it received something

	if(read(this->port,content,100)==-1)
		return 1;
	if(baseCheckPort(this)!=0)
		return 1;
	return 0;
}
//
int32_t baseInit(Packet_t* this){

	if(write(this->port, this->initCommand, sizeof(this->initCommand))==-1)
		return 1;
	sleep(this->initDelayMs);
	if(baseCheckPort(this)!=0)
		return 1;
	return 0;
}
int32_t baseProc(Packet_t* this){

	if(baseInit(this)==1){
		return 1;
	}
	if(baseSend(this)==1)
		return 1;

	if(baseReceive(this)==1)
		return 1;
	return 0;

}
void base_ctor(Packet_t *pbase,uint8_t prio,uint8_t id,uint8_t delay){
	pbase->id=id;
	pbase->initDelayMs=delay;//3 seconds
	pbase->fpInit=baseInit;
	pbase->fpSend=baseSend;
	pbase->fpReceive=baseReceive;
	pbase->fpProc=baseProc;

}
