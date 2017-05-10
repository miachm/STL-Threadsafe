#include "stack thread safe.hpp"
#include "gtest/gtest.h"

TEST(Push,HandleBasicOperation){
	std::threadsafe::stack<int> stack;
	
	int out;
	for (int i = 1;i <= 10;i++){
			stack.push(i);
			
			stack.wait_top(out);
			EXPECT_EQ(i,out);
			EXPECT_EQ(i,stack.size());
	}

	for (int i = 10;i >= 1;i--){
			stack.wait_pop(out);
			EXPECT_EQ(i,out);
			EXPECT_EQ(i-1,stack.size());
	}
}
