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
