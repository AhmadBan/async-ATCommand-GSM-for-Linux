/*
 * queue.h
 *
 *  Created on: Dec 18, 2020
 *      Author: Ahmad
 */

#ifndef INC_MYQUEUE_H_
#define INC_MYQUEUE_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <commands/baseCommand.h>
#define MAX_QUEUE 10

typedef struct {
	Cmd_t* cmdArray[MAX_QUEUE];
	int front ;
	int rear ;
	int itemCount;
	pthread_mutex_t mu_queue;
	pthread_cond_t cond;
}MyQueue_t;


void initQueue(MyQueue_t* myQ);

int addToQueue(Cmd_t* cmd,MyQueue_t* myQ);

Cmd_t* waitForQueue(MyQueue_t* mq);

Cmd_t* peek(MyQueue_t* q) ;

bool isEmpty(MyQueue_t* q);

bool isFull(MyQueue_t* q);

int size(MyQueue_t* q);

void insert(MyQueue_t* q,Cmd_t* data);

Cmd_t* removeData(MyQueue_t* q) ;

#endif /* INC_MYQUEUE_H_ */
