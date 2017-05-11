#include "stack thread safe.hpp"
#include "gtest/gtest.h"
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

void wait(std::mutex& lock,std::condition_variable& cond,const bool &ready){
	std::unique_lock<std::mutex> unique_lock(lock);
	while (!ready){
		cond.wait(unique_lock);
	}
}

TEST(Push,ThreadSafety){
	std::threadsafe::stack<int> stack;
	std::threadsafe::stack<int> another_stack;
	constexpr int ITERATIONS = 10;
	std::thread producers[ITERATIONS];
	std::thread consumers[ITERATIONS];
	std::mutex lock;
	std::condition_variable cond;
	bool ready = false;

	for (int i = 0;i < ITERATIONS;i++){
		producers[i] = std::thread([&](int id){
			wait(lock,cond,ready);
			for (int j = 0;j < ITERATIONS;j++){
				stack.push(id);
			}
		},i);
	}

	for (int i = 0;i < ITERATIONS;i++){
		consumers[i] = std::thread([&]{
			wait(lock,cond,ready);
			int out;
			for (int j = 0;j < ITERATIONS;j++){
				stack.wait_pop(out);
				ASSERT_LT(out,ITERATIONS);
				ASSERT_GE(out,0);
				another_stack.push(out);
			}
		});
	}
	
	ready = true;
	cond.notify_all();
	for (int i = 0;i < ITERATIONS;i++){
		producers[i].join();
		consumers[i].join();
	}

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

TEST(Push,UpperBound){
	std::threadsafe::stack<int> stack;
	constexpr int ITERATIONS = 10;
	stack.setLimit(ITERATIONS/2);

	std::thread producers[ITERATIONS];
	std::thread consumer;
	std::mutex lock;
	std::condition_variable cond;
	bool ready = false;

	for (int i = 0;i < ITERATIONS;i++){
		producers[i] = std::thread([&](int id){
			wait(lock,cond,ready);
			for (int j = 0;j < ITERATIONS;j++){
				stack.push(id);
				ASSERT_LE(stack.size(),ITERATIONS/2);
			}
		},i);
	}

	consumer = std::thread([&]{
		wait(lock,cond,ready);
		int out;
		for (int j = 0;j < ITERATIONS*ITERATIONS;j++){
			ASSERT_LE(stack.size(),ITERATIONS/2);
			stack.wait_pop(out);
		}
		
	});
	
	ready = true;
	cond.notify_all();
	for (int i = 0;i < ITERATIONS;i++){
		producers[i].join();
	}
	consumer.join();
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
}

TEST(Try_pop,ThreadSafety){
	std::threadsafe::stack<int> stack;
	std::threadsafe::stack<int> another_stack;
	constexpr int ITERATIONS = 10;
	std::thread producers[ITERATIONS];
	std::thread consumers[ITERATIONS];
	std::mutex lock;
	std::condition_variable cond;
	bool ready = false;

	for (int i = 0;i < ITERATIONS;i++){
		producers[i] = std::thread([&](int id){
			wait(lock,cond,ready);
			for (int j = 0;j < ITERATIONS;j++){
				stack.push(id);
			}
		},i);
	}

	for (int i = 0;i < ITERATIONS;i++){
		consumers[i] = std::thread([&]{
			wait(lock,cond,ready);
			int out;
			for (int j = 0;j < ITERATIONS;j++){
				while (!stack.try_pop(out));
				another_stack.push(out);
			}
		});
	}
	
	ready = true;
	cond.notify_all();
	for (int i = 0;i < ITERATIONS;i++){
		producers[i].join();
		consumers[i].join();
	}

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
