/*
 * smsSendPackets.c
 *
 *  Created on: Dec 17, 2020
 *      Author: Ahmad
 */


#include <packets/smsSendPackets.h>
Packet_t smsSend;
int32_t smsSendProc(Packet_t* this){
	SMSPacket_T* currentSMS=(SMSPacket_T*)this;


	return 1;
}
void smsSend_ctor(SMSPacket_T *psmsSend,uint8_t prio){
	psmsSend->super.id=SMS_SEND_SIG;
	//psmsSend->super.DelayMs=3;//3 seconds
	Packet_t base={
			SMS_SEND_SIG,
			0,
			0,
			"AT+CMGF=1\r\n",//initCommand
			"AT+CMGS=\"%s\"",//	command[50]
			"%s",//param message
			"\032",//finishParam[2];
			0,//fpInit
			1000,//initDelayMs;
			0,	//fpSend
			1000,//sendDelayMs
			0,//fpReceive
			"+CMGS: %d\r\n\r\nOK\r\n",//expectedAnswerOnSucess
			"",//expectedAnswerOnError
			5000,//receiveDelayMs
			0,//fpProc
			10,//retry
			0//	port
	};
	psmsSend->super=base;
}
