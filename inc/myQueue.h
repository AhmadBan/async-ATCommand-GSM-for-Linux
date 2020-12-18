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
#define MAX_QUEUE 6

typedef struct {
	Command_t* intArray[MAX_QUEUE];
	int front ;
	int rear ;
	int itemCount;
	pthread_mutex_t mu_queue;
	pthread_cond_t cond;
}MyQueue_t;

Command_t* peek(MyQueue_t* q) ;

bool isEmpty(MyQueue_t* q);

bool isFull(MyQueue_t* q);

int size(MyQueue_t* q);

void insert(MyQueue_t* q,Command_t* data);

Command_t* removeData(MyQueue_t* q) ;

#endif /* INC_MYQUEUE_H_ */
