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
	std::threadsafe::queue<int> another_queue;
	constexpr int ITERATIONS = 10;
	constexpr int PRODUCERS = ITERATIONS;
	constexpr int CONSUMERS = ITERATIONS;

	auto producer = [&](int id,int it){queue.push(id);};
	auto consumer = [&](int id,int it){
					int out;
					queue.wait_pop(out);
					ASSERT_LT(out,ITERATIONS);
					ASSERT_GE(out,0);
					another_queue.push(out);
					};

	launchThreads(producer,consumer,PRODUCERS,CONSUMERS,ITERATIONS);

	ASSERT_EQ(ITERATIONS*ITERATIONS,another_queue.size());

	int freq_table[ITERATIONS];
	std::fill(freq_table,freq_table+ITERATIONS,0);
	while (!another_queue.empty()){
		int out;
		another_queue.wait_pop(out);
		freq_table[out]++;
	}

	for (int i = 0;i < ITERATIONS;i++)
	{
		ASSERT_EQ(ITERATIONS,freq_table[i]);
	}
}

TEST(Push,UpperBound){
	std::threadsafe::queue<int> queue;
	constexpr int ITERATIONS_PRODUCERS = 10;
	constexpr int NUM_PRODUCERS = ITERATIONS_PRODUCERS;
	constexpr int NUM_CONSUMERS = 1;
	constexpr int SIZE = NUM_PRODUCERS/2;
	constexpr int ITERATIONS_CONSUMERS = ITERATIONS_PRODUCERS*NUM_PRODUCERS;
	queue.setLimit(SIZE);

	auto producer = [&](int id,int it){
				queue.push(id);
				ASSERT_LE(queue.size(),SIZE);
			};
	auto consumer = [&](int id,int it){
				int out;
				ASSERT_LE(queue.size(),SIZE);
				queue.wait_pop(out);
			};

	launchThreads(producer,consumer,NUM_PRODUCERS,NUM_CONSUMERS,ITERATIONS_PRODUCERS,ITERATIONS_CONSUMERS);
}
