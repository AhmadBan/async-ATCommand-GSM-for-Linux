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
TEST(QueueTest, AddTeElementToQueue) {
	MyQueue_t myQ;
	Command_t cmd[10];
	initQueue(&myQ);

	for(int i=0;i<9;i++){
		cmd[0].id=i;
		addtoQueue(&cmd[0], &myQ);
	}

	EXPECT_EQ(10, addtoQueue(&cmd[1], &myQ));
}

TEST(QueueTest, AddOneElementToQueue) {
	MyQueue_t myQ;
	Command_t cmd;
	initQueue(&myQ);

	cmd.id=1;
	EXPECT_EQ(1, addtoQueue(&cmd, &myQ));
}


TEST(QueueTest, AddTwoElementToQueue) {
	MyQueue_t myQ;
	Command_t cmd1,cmd2;
	initQueue(&myQ);

	cmd1.id=1;
	addtoQueue(&cmd1, &myQ);
	cmd2.id=2;
	EXPECT_EQ(2, addtoQueue(&cmd2, &myQ));
}


