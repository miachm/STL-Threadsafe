#include "queue thread safe.hpp"
#include "gtest/gtest.h"
#include <thread>
#include <algorithm>
#include "aux_tests.hpp"

TEST(Push,HandleBasicOperation){
	std::threadsafe::queue<int> queue;
	
	int out;
	for (int i = 1;i <= 10;i++){
			queue.push(i);
			
			queue.wait_back(out);
			EXPECT_EQ(i,out);

			queue.wait_top(out);
			EXPECT_EQ(out,1);
			EXPECT_EQ(i,queue.size());
	}

	for (int i = 1;i <= 10;i++){
			queue.wait_pop(out);
			EXPECT_EQ(i,out);
			EXPECT_EQ(10-i,queue.size());
	}
}

TEST(Push,ThreadSafety){
	std::threadsafe::queue<int> queue;
	testPushThreadSafety(queue);
}

TEST(Push,UpperBound){
	std::threadsafe::queue<int> queue;
	testPushUpperBound(queue);
}

TEST(Try_pop,HandleBasicOperation){
	std::threadsafe::queue<int> queue;
	for (int i = 1;i <= 10;i++){
		queue.push(i);
	}

	int out;
	for (int i = 1;i <= 10;i++){
		ASSERT_EQ(10-i+1,queue.size());
		ASSERT_EQ(true,queue.try_pop(out));
		ASSERT_EQ(i,out);
	}
	ASSERT_EQ(0,queue.size());
	ASSERT_EQ(false,queue.try_pop(out));
}

TEST(Try_pop,ThreadSafety){
	std::threadsafe::queue<int> queue;
	testPopThreadSafety(queue);
}
