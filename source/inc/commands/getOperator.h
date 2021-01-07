/*
 * getOperator.h
 *
 *  Created on: Jan 3, 2021
 *      Author: Ahmad
 */

#ifndef INC_COMMANDS_GETOPERATOR_H_
#define INC_COMMANDS_GETOPERATOR_H_
#include "commands/baseCommand.h"

typedef struct{
	Cmd_t super;
	char* operator[30];

}GetOperator_t;
void getOperator_ctor(Cmd_t* me);
int32_t getOperatorResponse(Cmd_t* me);
#endif /* INC_COMMANDS_GETOPERATOR_H_ */
