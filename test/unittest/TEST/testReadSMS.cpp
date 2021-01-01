/*
 * testReadSMS.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: Ahmad
 */
//example read sms
/*
 \r\n
+CMGL: 1,"REC READ","+989350542618","","20/12/10,21:46:48+14"\r\n
Hello world this is 2\n
This is 3\n
\n
\r\n
\r\n
+CMGL: 2,"REC READ","+989350542618","","20/12/10,21:53:16+14"\r\n
This is a big sms ififjxhxhxzyh 8duduxuxuxu 8f8fidiiiuuxuxu f8f7dududuxud 9fifdiuuytrfhhgfd hhjkoutrf hhjhffrrfhj.   Yuujjbvcryikjbv hduduxxuxuxufuuu.  8\r\n
\r\n
+CMGL: 3,"REC READ","+989350542618","","20/12/10,21:53:17+14"\r\n
fuf8dcixifdu7z7zuxu 9f8fixudyxyzyxu giducxxixuxuxuxid. Ogicixuxuxuyuxuxux icicuxufuxu.  Icuxuxuxyx6\n
Uxux7x7xuuy eaqw.  10 do fyiudwrggv. Ggujyuihghuvuuuc\r\n
\r\n
+CMGL: 4,"REC READ","+989350542618","","20/12/10,21:53:18+14"\r\n
 64rthbbbf\r\n
\r\n
+CMGL: 5,"REC READ","+989350542618","","20/12/10,21:55:12+14"\r\n
This is second big sms ififjxhxhxzyh 8duduxuxuxu 8f8fidiiiuuxuxu f8f7dududuxud 9fifdiuuytrfhhgfd hhjkoutrf hhjhffrrfhj.   Yuujjbvcryikjbv hduduxxuxuxufuu\r\n
\r\n
+CMGL: 6,"REC READ","+989350542618","","20/12/10,21:55:13+14"\r\n
u.  8fuf8dcixifdu7z7zuxu 9f8fixudyxyzyxu giducxxixuxuxuxid. Ogicixuxuxuyuxuxux icicuxufuxu.  Icuxuxuxyx6\n
Uxux7x7xuuy eaqw.  10 do fyiudwrggv. Ggujyuihghu\r\n
\r\n
+CMGL: 7,"REC READ","+989350542618","","20/12/10,21:55:14+14"\r\n
vuuuc 64rthbbbfgfucux\r\n
\r\n
+CMGL: 8,"REC READ","+989350542618","","20/12/15,21:31:32+14"\r\n
0043006800650063006B0020006E006500770020006D006500730073006100670065000A000A062706310633062706440020062706330020062706450020062706330020062C062F06CC062F\r\n
\r\n
+CMGL: 9,"REC UNREAD","+989350542618","","20/12/17,10:53:47+14"\r\n
0031003200330034003500360037003800390030000A006100620063006400650066006700680069006A006B006C006D006E006F0070007100720073007400750076007700780079007A000A004100420043004400450046004700480049004A004B004C004D004E004F0050005100520053005400550056005700580059005A000A00400023\r\n
\r\n
+CMGL: 10,"REC UNREAD","+989350542618","","20/12/17,10:53:48+14"\r\n
0024005F0026002D002B00280029002F002A00220027003A003B0021003F000A007E0060007C2022221A03C000F700D700B6220600A300A220AC00A5005E00B0003D007B007D005C002500A900AE21222713005B005D000A00DC00D600C41E9E000A00FC00F600E400DF\r\n
\r\n
OK\r\n

 */

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
#include "commands/readSMS.h"


}


# include "gtest/gtest.h"

TEST(SMSCommandTest, SMSReadTestCtor) {

	ReadSMS_t cmd;

	readSMS_ctor(&cmd.super);
	EXPECT_EQ(cmd.super.id, READ_SMS_SIG);
	EXPECT_EQ(cmd.super.priority, 0);
	EXPECT_EQ(cmd.super.procId, 0);
	EXPECT_EQ(strcmp(cmd.super.command,"AT+CMGL=\"ALL\""), 0);
	EXPECT_EQ(strcmp(cmd.super.finishParam,"AT+CMGD"), 0);


}

SMS_t testSms;
void testCallback(SMS_t* sms){
	testSms=*sms;
}
TEST(SMSCommandTest, SMSReadTestReceive) {
	int fileDescriptor;
	char filename[]="readSmsFile";
	ReadSMS_t cmd;
	int ret,size;
	char content[]="0\r\n+CMGL: 1,\"REC READ\",\"+989350542618\",\"\",\"20/12/10,21:46:48+14\"\r\nHello world this is 2\nThis is 3\n\n\r\n0000000000\r\nOK\r\n";
	char output[60];
	ret=remove(filename);
	readSMS_ctor(&cmd.super);
	cmd.SMSReceivedCallBack=testCallback;
	fileDescriptor  = open(filename, O_RDWR | O_CREAT);
	if (fileDescriptor == -1) {
		perror("File not Found.!");
		exit(1);
	}

	cmd.super.port=fileDescriptor;


	write(cmd.super.port,content, strlen(content));
	lseek(cmd.super.port,0,SEEK_SET);

	ret=SMSReceiveAllMsg((Cmd_t*)&cmd);
	close(fileDescriptor);
	fileDescriptor = open(filename, O_RDONLY);
	size = read(fileDescriptor, output, 24);
	output[size]=0;
	EXPECT_EQ(0,strcmp("AT+CMGS=\"+989350542618\"\r",output));
	EXPECT_EQ(ret,0);


}




