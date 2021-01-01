/*
 * smsSendPAckets.h
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */

#ifndef INC_PACKETS_SMSSENDPACKETS_H_
#define INC_PACKETS_SMSSENDPACKETS_H_
/*
 * Sending SMS needs 2 more parameters than the base command structure so we create
 * SMSPacket_t Derived from Command_t
 */
#include <commands/baseCommand.h>
typedef struct{
	Cmd_t super;
	char phoneNumber[20];//the phone number is not more than 20 character
	char message[512];//sim800 can not support more than 512 character
}SMSPacket_T;

void SMSSend_ctor(Cmd_t* me);
int32_t SMSSend(Cmd_t* me);
int32_t SMSReceiveResponse(Cmd_t* me);
#endif /* INC_PACKETS_SMSSENDPACKETS_H_ */
