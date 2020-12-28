/*
 * main.h
 *
 *  Created on: Dec 18, 2020
 *      Author: Ahmad
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include <myQueue.h>
#include <commands/baseCommand.h>

void initQueue(MyQueue_t* myQ);
int addToQueue(Cmd_t* cmd,MyQueue_t* myQ);
Cmd_t* waitForQueue(MyQueue_t* mq);
#endif /* INC_MAIN_H_ */
