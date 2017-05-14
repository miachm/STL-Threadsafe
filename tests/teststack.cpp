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

template<typename Producer,typename Consumer>
void launchThreads(Producer producer_function,Consumer consumer_function,const int num_producers,const int num_consumers, const int iterations_producers,const int iterations_consumers){
	std::thread* producers = new std::thread[num_producers];
	std::thread* consumers = new std::thread[num_consumers];
	std::mutex lock;
	std::condition_variable cond;
	bool ready = false;

	for (int i = 0;i < num_producers;i++){
		producers[i] = std::thread([&](int id){
			wait(lock,cond,ready);
			for (int j = 0;j < iterations_producers;j++){
				producer_function(id,j);
			}
		},i);
	}

	for (int i = 0;i < num_consumers;i++){
		consumers[i] = std::thread([&](int id){
			wait(lock,cond,ready);
			for (int j = 0;j < iterations_consumers;j++){
				consumer_function(id,j);
			}
		},i);
	}
	
	ready = true;
	cond.notify_all();
	for (int i = 0;i < num_producers;i++){
		producers[i].join();
	}
	for (int i = 0;i < num_consumers;i++){
		consumers[i].join();
	}

	delete[] producers;
	delete[] consumers;
}

template<typename Producer,typename Consumer>
void launchThreads(Producer producer_function,Consumer consumer_function,const int num_producers,const int num_consumers, const int iterations_producers){
	launchThreads(producer_function,consumer_function,num_producers,num_consumers,iterations_producers,iterations_producers);
}

TEST(Push,ThreadSafety){
	std::threadsafe::stack<int> stack;
	std::threadsafe::stack<int> another_stack;
	constexpr int ITERATIONS = 10;
	constexpr int PRODUCERS = ITERATIONS;
	constexpr int CONSUMERS = ITERATIONS;

	auto producer = [&](int id,int it){stack.push(id);};
	auto consumer = [&](int id,int it){
					int out;
					stack.wait_pop(out);
					ASSERT_LT(out,ITERATIONS);
					ASSERT_GE(out,0);
					another_stack.push(out);
					};

	launchThreads(producer,consumer,PRODUCERS,CONSUMERS,ITERATIONS);

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
	constexpr int ITERATIONS_PRODUCERS = 10;
	constexpr int NUM_PRODUCERS = ITERATIONS_PRODUCERS;
	constexpr int NUM_CONSUMERS = 1;
	constexpr int SIZE = NUM_PRODUCERS/2;
	constexpr int ITERATIONS_CONSUMERS = ITERATIONS_PRODUCERS*NUM_PRODUCERS;
	stack.setLimit(SIZE);

	auto producer = [&](int id,int it){
				stack.push(id);
				ASSERT_LE(stack.size(),SIZE);
			};
	auto consumer = [&](int id,int it){
				int out;
				ASSERT_LE(stack.size(),SIZE);
				stack.wait_pop(out);
			};

	launchThreads(producer,consumer,NUM_PRODUCERS,NUM_CONSUMERS,ITERATIONS_PRODUCERS,ITERATIONS_CONSUMERS);
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
