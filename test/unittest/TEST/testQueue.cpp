/*
 * test_factorial.cpp
 *
 *  Created on: Dec 21, 2020
 *      Author: Ahmad
 */


//test_factorial.cpp
extern "C" {
#include "commands/baseCommand.h"
#include "main.h"
#include "myQueue.h"
}

# include "gtest/gtest.h"

TEST(QueueTest, AddOneElementToQueue) {
	MyQueue_t myQ;
	Cmd_t cmd;
	initQueue(&myQ);

	cmd.id=1;
	EXPECT_EQ(1, addToQueue(&cmd, &myQ));
}


TEST(QueueTest, AddTwoElementToQueue) {
	MyQueue_t myQ;
	Cmd_t cmd1,cmd2;
	initQueue(&myQ);

	cmd1.id=1;
	addToQueue(&cmd1, &myQ);
	cmd2.id=2;
	EXPECT_EQ(2, addToQueue(&cmd2, &myQ));
}


TEST(QueueTest, AddTenElementToQueue) {
	MyQueue_t myQ;
	Cmd_t cmd[10];
	initQueue(&myQ);

	for(int i=0;i<9;i++){
		cmd[i].id=i;
		addToQueue(&cmd[i], &myQ);
	}

	EXPECT_EQ(10, addToQueue(&cmd[9], &myQ));
}



TEST(QueueTest, waitForOneElement) {
	MyQueue_t myQ;
	Cmd_t cmd;
	initQueue(&myQ);


	cmd.id=123;
	cmd.priority=1;
	cmd.procId=10;
	addToQueue(&cmd, &myQ);
	Cmd_t *test=waitForQueue(&myQ);


	EXPECT_EQ(123, test->id);
}

TEST(QueueTest, waitForTenElement) {
	MyQueue_t myQ;
	Cmd_t cmd[10];
	initQueue(&myQ);

	for(int i=0;i<10;i++){
		cmd[i].id=i;
		cmd[i].priority=i*2;
		cmd[i].procId=i;
		addToQueue(&cmd[i], &myQ);
	}


	for(int i=0;i<10;i++){
		Cmd_t *test=waitForQueue(&myQ);
		removeData( &myQ);
		EXPECT_EQ(i, test->id);
		EXPECT_EQ(i*2, test->priority);
		EXPECT_EQ(i, test->procId);

	}
}
