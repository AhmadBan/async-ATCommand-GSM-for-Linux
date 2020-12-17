/*
 * smsSendPAckets.h
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */
#include <packets/basePackets.h>
#ifndef INC_PACKETS_SMSSENDPACKETS_H_
#define INC_PACKETS_SMSSENDPACKETS_H_

typedef struct{
	Packet_t super;
	char phoneNumber[20];//the phone number is not more than 20 character
	char message[512];//sim800 can not support more than 512 character
}SMSPacket_T;


#endif /* INC_PACKETS_SMSSENDPACKETS_H_ */
