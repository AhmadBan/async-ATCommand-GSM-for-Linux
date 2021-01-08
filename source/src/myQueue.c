/*
 * queue.c
 *
 *  Created on: Dec 18, 2020
 *      Author: Ahmad
 */


#include <myQueue.h>


Cmd_t* peek(MyQueue_t* q) {
   return q->cmdArray[q->front];
}

bool isEmpty(MyQueue_t* q) {
   return q->itemCount == 0;
}

bool isFull(MyQueue_t* q) {
   return q->itemCount == MAX_QUEUE;
}

int size(MyQueue_t* q) {
   return q->itemCount;
}

void insert(MyQueue_t* q,Cmd_t* data) {

   if(!isFull(q)) {

      if(q->rear == MAX_QUEUE) {
         q->rear = 0;

      }

      q->cmdArray[q->rear++] = data;
      q->itemCount++;
   }
}

Cmd_t* removeData(MyQueue_t* q) {
	Cmd_t* data = q->cmdArray[q->front++];

   if(q->front == MAX_QUEUE) {
	   q->front = 0;
   }

   q->itemCount--;
   return data;
}

void initQueue(MyQueue_t* myQ){
	myQ->rear=0;
	myQ->front=0;
	myQ->itemCount=0;
	if(pthread_mutex_init(&myQ->mu_queue, NULL)!=0)
	{
		printf("\n mutex init has failed\n");
	    return;
	}
	 if(pthread_cond_init(&myQ->cond, NULL)!=0){
		 printf("\n cond init has failed\n");
		 return;
	 }
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
