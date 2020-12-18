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
MyQueue_t myQ;

extern const pCtorFunc ctorsArr[];

extern const u_int8_t ctorCnt;
//add command to queue
int addtoQueue(Command_t* cmd){
	MyQueue_t *mq = &myQ;
	pthread_mutex_lock(&mq->mu_queue);
	if(isFull(mq))
		return 1;
	insert(mq, cmd);
	pthread_mutex_unlock(&mq->mu_queue);
	pthread_cond_signal(&mq->cond);
	return 0;
}


// this is  sms consumer for queue
//It waits for a message in the queue if there is no message then waits for it
//else it get the messages and process them
int main(){

	MyQueue_t *mq = &myQ;
	//init all constructors
	for(int i=0;i<ctorCnt;i++){
		ctorsArr[i]();
	}

	while(1)
	{
		pthread_mutex_lock(&mq->mu_queue);
		if(!isEmpty(mq))
		{
			Command_t* cmd =(Command_t*)peek( mq);
			if(cmd->fpProc(cmd)!=1)
			{
				removeData( mq);//delete processed data from queue
			}
			pthread_mutex_unlock(&mq->mu_queue);

		}
		else
		{
			pthread_cond_wait(&mq->cond, &mq->mu_queue);
		}
	}

	return 0;
}
