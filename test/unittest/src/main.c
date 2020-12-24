/*
 * main.c
 *
 *  Created on: Dec 16, 2020
 *      Author: Ahmad
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include <myQueue.h>
#include <commands/baseCommand.h>

extern const pCtorFunc ctorsArr[];

extern const u_int8_t ctorCnt;
//add command to queue
MyQueue_t globalQ;
void initQueue(MyQueue_t* myQ){
	myQ->rear=0;
	myQ->front=0;
	myQ->itemCount=0;
	pthread_mutex_init(&myQ->mu_queue, NULL);
	pthread_cond_init(&myQ->cond, NULL);
}
int addtoQueue(Command_t* cmd,MyQueue_t* myQ){
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


initSIM800(){

}

// this is  sms consumer for queue
//It waits for a message in the queue if there is no message then waits for it
//else it get the messages and process them

