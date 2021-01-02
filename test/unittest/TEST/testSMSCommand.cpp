

extern "C" {
#include "commands/baseCommand.h"
#include "commands/smsSendCommand.h"
#include "main.h"
#include "myQueue.h"
#include<stdio.h>

#include<unistd.h>

#include<stdlib.h>

#include<string.h>

#include <fcntl.h>

}

# include "gtest/gtest.h"

TEST(SMSCommandTest, SmsCtorTest) {
	SMSPacket_T cmd;
	SMSSend_ctor((Cmd_t*)&cmd);
	EXPECT_EQ(cmd.super.id, SMS_SEND_SIG);
	EXPECT_EQ(cmd.super.priority, 0);
	EXPECT_EQ(cmd.super.procId, 0);
	EXPECT_EQ(strcmp(cmd.super.command,"AT+CMGS"), 0);
	//TODO:and so on for all field

}

TEST(SMSCommandTest, SMSSendTestOK) {
	int fileDescriptor;
	char filename[]="SmSTestFile";
	SMSPacket_T cmd;
	int ret,size;
	char content[]="111111111111111111111111\r\n>";
	char output[60];
	char phoneNumber[]="+989350542618";
	ret=remove(filename);
	SMSSend_ctor((Cmd_t*)&cmd);
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}

	cmd.super.port=fileDescriptor;
	memcpy(cmd.phoneNumber,phoneNumber,strlen(phoneNumber));
	cmd.phoneNumber[strlen(phoneNumber)]=0;
	write(cmd.super.port,content, strlen(content));
	lseek(cmd.super.port,0,SEEK_SET);

	ret=SMSSend((Cmd_t*)&cmd);
	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	size = read(fileDescriptor, output, 24);
	output[size]=0;
	EXPECT_EQ(0,strcmp("AT+CMGS=\"+989350542618\"\r",output));
	EXPECT_EQ(ret,0);


}

TEST(SMSCommandTest, SMSSendTestError) {
	int fileDescriptor;
	char filename[]="SmSTestFile";
	SMSPacket_T cmd;
	int ret,size;
	char content[]="111111111111111111111111";
	char answer[]="\r\nERROR\r\n\r\n";
	char output[60];
	char phoneNumber[]="+989350542618";
	ret=remove(filename);
	SMSSend_ctor((Cmd_t*)&cmd);
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}

	cmd.super.port=fileDescriptor;
	memcpy(cmd.phoneNumber,phoneNumber,strlen(phoneNumber));
	cmd.phoneNumber[strlen(phoneNumber)]=0;
	write(cmd.super.port,content, strlen(content));
	write(cmd.super.port,answer, strlen(answer));
	lseek(cmd.super.port,0,SEEK_SET);

	ret=SMSSend((Cmd_t*)&cmd);
	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	size = read(fileDescriptor, output, 24);
	output[size]=0;
	EXPECT_EQ(0,strcmp("AT+CMGS=\"+989350542618\"\r",output));
	EXPECT_EQ(ret,1);


}



TEST(SMSCommandTest, SMSReceiveTestOK) {
	int fileDescriptor;
	char filename[]="SmSTestFile";
	SMSPacket_T cmd;
	int ret,size;
	char content[]="11111111111111111111111+CMGS: 10\r\n\r\nOK\r\n\r\n";
	char output[60];
	char messaage[]="this is a test message";
	ret=remove(filename);
	SMSSend_ctor((Cmd_t*)&cmd);
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}

	cmd.super.port=fileDescriptor;
	memcpy(cmd.message,messaage,strlen(messaage));
	cmd.message[strlen(messaage)]=0;
	write(cmd.super.port,content, strlen(content));
	lseek(cmd.super.port,0,SEEK_SET);

	ret=SMSReceiveResponse((Cmd_t*)&cmd);

	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	size = read(fileDescriptor, output, 43);
	output[size]=0;
	EXPECT_EQ(0,strcmp("this is a test message\032+CMGS: 10\r\n\r\nOK\r\n\r\n",output));
	EXPECT_EQ(ret,0);


}

TEST(SMSCommandTest, SMSReceiveTestError) {
	int fileDescriptor;
	char filename[]="SmSTestFile";
	SMSPacket_T cmd;
	int ret,size;
	char content[]="11111111111111111111111\r\nERROR\r\n\r\n";
	char output[60];
	char messaage[]="this is a test message";
	ret=remove(filename);
	SMSSend_ctor((Cmd_t*)&cmd);
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}

	cmd.super.port=fileDescriptor;
	memcpy(cmd.message,messaage,strlen(messaage));
	cmd.message[strlen(messaage)]=0;
	write(cmd.super.port,content, strlen(content));
	lseek(cmd.super.port,0,SEEK_SET);

	ret=SMSReceiveResponse((Cmd_t*)&cmd);

	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	size = read(fileDescriptor, output, 50);
	output[size]=0;
	EXPECT_EQ(0,strcmp("this is a test message\032\r\nERROR\r\n\r\n",output));
	EXPECT_EQ(ret,1);

}


