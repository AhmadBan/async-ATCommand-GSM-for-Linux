/*
 * test_factorial.cpp
 *
 *  Created on: Dec 21, 2020
 *      Author: Ahmad
 */


//test_factorial.cpp
extern "C" {
#include "fac.h"
}

# include "gtest/gtest.h"


TEST(IntegerFunctionTest, negative) {
        EXPECT_EQ(1, factorial(-5));
        EXPECT_EQ(1, factorial(-1));
        EXPECT_GT(factorial(-10), 0);
}

TEST(IntegerFunctionTest, DISABLED_zero) {
         EXPECT_EQ(1, factorial(0));
}

TEST(IntegerFunctionTest, postive) {
         EXPECT_EQ(1, factorial(1));
         EXPECT_EQ(2, factorial(2));
         EXPECT_EQ(6, factorial(3));
         EXPECT_EQ(40320, factorial(8));
}
