/*
 * queue.c
 *
 *  Created on: Dec 18, 2020
 *      Author: Ahmad
 */


#include <myQueue.h>


Command_t* peek(MyQueue_t* q) {
   return q->intArray[q->front];
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

void insert(MyQueue_t* q,Command_t* data) {

   if(!isFull(q)) {

      if(q->rear == MAX_QUEUE-1) {
         q->rear = -1;
      }

      q->intArray[++q->rear] = data;
      q->itemCount++;
   }
}

Command_t* removeData(MyQueue_t* q) {
	Command_t* data = q->intArray[q->front++];

   if(q->front == MAX_QUEUE) {
	   q->front = 0;
   }

   q->itemCount--;
   return data;
}
