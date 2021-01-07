/*
 * testCommand.cpp
 *
 *  Created on: Dec 25, 2020
 *      Author: Ahmad
 */

extern "C" {
#include "commands/baseCommand.h"
#include "main.h"
#include "myQueue.h"
#include<stdio.h>

#include<unistd.h>

#include<stdlib.h>

#include<string.h>

#include <fcntl.h>
}

# include "gtest/gtest.h"

TEST(baseCommandTest, baseCheckReceiveError) {
	int fileDescriptor;
	char filename[]="testFile";
	char content[]="atd09350542618;\r\r\nERROR\r\n";
	Cmd_t cmd;
	remove(filename);
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}
	write(fileDescriptor, content, strlen(content));
	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	//rewind(fileDescriptor);
	cmd.port=fileDescriptor;
	EXPECT_EQ(1, baseCheckPort(&cmd));
}

TEST(baseCommandTest, baseCheckReceiveOK) {
	int fileDescriptor;
	char filename[]="testfile";
	char content[]="atd09350542618;\r\r\nOK\r\n";
	Cmd_t cmd;
	remove(filename);
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}
	write(fileDescriptor, content, strlen(content));
	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	//rewind(fileDescriptor);
	cmd.port=fileDescriptor;
	EXPECT_EQ(0, baseCheckPort(&cmd));
}


TEST(baseCommandTest, baseSend) {
	int fileDescriptor,size=0;
	char filename[]="sendfile";
	char content[100];

	remove(filename);
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}
	Cmd_t cmd;
	char command[]="AT+CMGS\r";
	memcpy(cmd.command,command,strlen(command));
	cmd.command[strlen(command)]=0;
	cmd.port=fileDescriptor;
	cmd.sendDelayMs=1;

	baseSend(&cmd);
	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	size = read(fileDescriptor, content, 100);
	content[size]=0;

	EXPECT_EQ(0, strcmp(content,cmd.command));
}



TEST(baseCommandTest, baseReceiveTest) {
	int fileDescriptor=0;
	char filename[]="testFile";
	char content[]="cat\r\r\nOK\r\n";
	char output[100]={0};
	int ret;
	char finish[]="\r";
	ret=remove(filename);
	Cmd_t cmd;
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}

	write(fileDescriptor, content, strlen(content));
	memcpy(cmd.finishParam,finish,strlen(finish));
	cmd.finishParam[strlen(finish)]=0;
	cmd.port=fileDescriptor;
	cmd.receiveDelayMs=1;
	cmd.retry=3;
	cmd.fpReset=baseReset;
	lseek(fileDescriptor,0,SEEK_SET);
	ret=baseReceive(&cmd);
	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	read(fileDescriptor, output, 1);

	EXPECT_EQ(0,strcmp("\r",output));
	EXPECT_EQ(0,ret);


}

TEST(baseCommandTest, baseResetTest) {
	Cmd_t cmd;

	cmd.fpReset=baseReset;
	cmd.fpReset(&cmd);

	EXPECT_EQ(1,1);

}
