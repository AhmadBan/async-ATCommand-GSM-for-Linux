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
int addtoQueue(Command_t* cmd,MyQueue_t* myQ);

#endif /* INC_MAIN_H_ */
