/*
 * readSMS.h
 *
 *  Created on: Dec 31, 2020
 *      Author: Ahmad
 */

#ifndef INC_COMMANDS_READSMS_H_
#define INC_COMMANDS_READSMS_H_
#include <commands/baseCommand.h>
#include <commands/deleteSMS.h>
typedef struct{
	int msgId;
	char date[15];
	char hour[15];
	char status[15];
	char phoneNumber[20];//the phone number is not more than 20 character
	char message[512];//sim800 can not support more than 512 character
}SMS_t;

typedef struct{
	Cmd_t super;
	void (*SMSReceivedCallBack)(SMS_t* sms);
}ReadSMS_t;
void readSMS_ctor(Cmd_t* me);
int32_t SMSReceiveAllMsg(Cmd_t* me);
#endif /* INC_COMMANDS_READSMS_H_ */
