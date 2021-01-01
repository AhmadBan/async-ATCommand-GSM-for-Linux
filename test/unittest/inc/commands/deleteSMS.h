/*
 * DeleteSMS.h
 *
 *  Created on: Jan 1, 2021
 *      Author: Ahmad
 */

#ifndef INC_COMMANDS_DELETESMS_H_
#define INC_COMMANDS_DELETESMS_H_
#include "commands/baseCommand.h"
typedef struct{
	Cmd_t super;
	int msgId;//Id of inbox message
}DeleteSMSPacket_T;
int32_t delCommand(Cmd_t* me);
void deleteSMS_ctor(Cmd_t* me);

#endif /* INC_COMMANDS_DELETESMS_H_ */
