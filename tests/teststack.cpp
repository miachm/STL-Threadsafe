#include "stack thread safe.hpp"
#include "gtest/gtest.h"
#include "aux_tests.hpp"
#include <thread>
#include <algorithm>

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

TEST(Push,ThreadSafety){
	std::threadsafe::stack<int> stack;
	testPushThreadSafety(stack);
}

TEST(Push,UpperBound){
	std::threadsafe::stack<int> stack;
	testPushUpperBound(stack);
}

TEST(Try_pop,HandleBasicOperation){
	std::threadsafe::stack<int> stack;
	for (int i = 1;i <= 10;i++){
		stack.push(i);
	}

	int out;
	for (int i = 10;i >= 1;i--){
		ASSERT_EQ(i,stack.size());
		ASSERT_EQ(true,stack.try_pop(out));
		ASSERT_EQ(i,out);
	}
	ASSERT_EQ(0,stack.size());
	ASSERT_EQ(false,stack.try_pop(out));
}

TEST(Try_pop,ThreadSafety){
	std::threadsafe::stack<int> stack;
	std::threadsafe::stack<int> another_stack;
	constexpr int NUM_PRODUCERS = 10;
	constexpr int NUM_CONSUMERS = 10;
	constexpr int ITERATIONS = 10;

	auto producer = [&](int id,int it){
				stack.push(id);
			};
	
	auto consumer = [&](int id,int it){
				int out;
				while (!stack.try_pop(out));
				another_stack.push(out);
			};

	launchThreads(producer,consumer,NUM_PRODUCERS,NUM_CONSUMERS,ITERATIONS);

	ASSERT_EQ(ITERATIONS*ITERATIONS,another_stack.size());

	int freq_table[ITERATIONS];
	std::fill(freq_table,freq_table+ITERATIONS,0);
	while (!another_stack.empty()){
		int out;
		another_stack.wait_pop(out);
		freq_table[out]++;
	}

	for (int i = 0;i < ITERATIONS;i++)
	{
		ASSERT_EQ(ITERATIONS,freq_table[i]);
	}
}

TEST(Wait_pop,HandleBasicOperation){
	std::threadsafe::stack<int> stack;
	for (int i = 1;i <= 10;i++){
		stack.push(i);
	}

	int out;
	for (int i = 10;i >= 1;i--){
		ASSERT_EQ(i,stack.size());
		stack.wait_pop(out);
		ASSERT_EQ(i,out);
	}
	ASSERT_EQ(0,stack.size());
}

TEST(Wait_pop,Timer){
	std::threadsafe::stack<int> stack;
	
	std::thread producer([&]{
		stack.push(1);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		stack.push(2);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		stack.push(3);
	});

	std::thread consumer([&]{
		int output;
		
		try {
			stack.wait_pop(output,std::chrono::milliseconds(5));
			ASSERT_EQ(1,output);
			stack.wait_pop(output,std::chrono::milliseconds(15));
			ASSERT_EQ(2,output);
		} catch(std::threadsafe::Time_Expired e){
			FAIL() << " exception Time_Expired throwed";
		}

		try {
			stack.wait_pop(output,std::chrono::milliseconds(1));
			FAIL() << " exception didn't throw!";
		} catch(std::threadsafe::Time_Expired e){
		}
	});

	producer.join();
	consumer.join();
}

TEST(Try_top,HandleBasicOperation){
	std::threadsafe::stack<int> stack;
	int out;
	ASSERT_EQ(false,stack.try_top(out));
	for (int i = 1;i <= 10;i++){
		stack.push(i);
	}

	for (int i = 10;i >= 1;i--){
		ASSERT_EQ(true,stack.try_top(out));
		ASSERT_EQ(10,stack.size());
		ASSERT_EQ(10,out);
	}
}

TEST(Try_top,ThreadSafety){
	std::threadsafe::stack<int> stack;
	std::threadsafe::stack<int> another_stack;
	constexpr int NUM_PRODUCERS = 1;
	constexpr int NUM_CONSUMERS = 1;
	constexpr int ITERATIONS = 10;

	auto producer = [&](int id,int it){
				stack.push(it);
			};
	
	int n = 0;
	auto consumer = [&](int id,int it){
				int out;
				
				while (!stack.try_top(out) && out < ITERATIONS){
					ASSERT_EQ(out,n);
					n++;
				}
			};

	launchThreads(producer,consumer,NUM_PRODUCERS,NUM_CONSUMERS,ITERATIONS);

}

TEST(Wait_top,HandleBasicOperation){
	std::threadsafe::stack<int> stack;
	int out;
	for (int i = 1;i <= 10;i++){
		stack.push(i);
	}

	for (int i = 10;i >= 1;i--){
		stack.wait_top(out);
		ASSERT_EQ(10,stack.size());
		ASSERT_EQ(10,out);
	}

}

TEST(Wait_top,Timer){
	std::threadsafe::stack<int> stack;
	
	std::thread producer([&]{
		stack.push(1);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		stack.push(2);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		stack.push(3);
	});

	std::thread consumer([&]{
		int output;
		
		try {
			stack.wait_top(output,std::chrono::milliseconds(5));
			ASSERT_EQ(1,output);
			stack.wait_pop(output);

			stack.wait_top(output,std::chrono::milliseconds(15));
			ASSERT_EQ(2,output);
			stack.wait_pop(output);
		} catch(std::threadsafe::Time_Expired e){
			FAIL() << " exception Time_Expired throwed";
		}

		try {
			stack.wait_top(output,std::chrono::milliseconds(1));
			FAIL() << " exception didn't throw!";
		} catch(std::threadsafe::Time_Expired e){
		}
	});

	producer.join();
	consumer.join();
}

TEST(EMPTY,HanbleBasicOperation){
	std::threadsafe::stack<int> stack;
	ASSERT_TRUE(stack.empty());
	stack.push(1);
	ASSERT_FALSE(stack.empty());
}
