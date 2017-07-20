#include "queue thread safe.hpp"
#include "gtest/gtest.h"
#include <thread>
#include <algorithm>

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
