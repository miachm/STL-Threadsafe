#include "priority_queue-threadsafe.hpp"
#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include <algorithm>
#include "aux_tests.hpp"

std::vector<int> randomNumbers{3,9,4,10,5,1,7,8,2,6}; // Prove the opposite!

TEST(Push,HandleBasicOperation){
	std::threadsafe::priority_queue<int> queue;
	
	int out;
	int max = 0;
	for (int i = 0;i < 10;i++){
			EXPECT_EQ(i,queue.size());
			queue.push(randomNumbers[i]);
			
			queue.wait_top(out);
			if (max < out)
				max = out;
			EXPECT_EQ(out,max);
	}

	for (int i = 10;i >= 1;i--){
			EXPECT_EQ(i,queue.size());
			queue.wait_pop(out);
			EXPECT_EQ(i,out);
	}
}

TEST(Push,ThreadSafety){
	std::threadsafe::priority_queue<int> queue;
	testPushThreadSafety(queue);
}

TEST(Push,UpperBound){
	std::threadsafe::priority_queue<int> queue;
	testPushUpperBound(queue);
}

TEST(Try_pop,HandleBasicOperation){
	std::threadsafe::priority_queue<int> queue;
	for (int i = 0;i < 10;i++){
		queue.push(randomNumbers[i]);
	}

	int out;
	for (int i = 10;i >= 1;i--){
		ASSERT_EQ(i,queue.size());
		ASSERT_EQ(true,queue.try_pop(out));
		ASSERT_EQ(i,out);
	}
	ASSERT_EQ(0,queue.size());
	ASSERT_EQ(false,queue.try_pop(out));
}

TEST(Try_pop,ThreadSafety){
	std::threadsafe::priority_queue<int> queue;
	testPopThreadSafety(queue);
}

TEST(Wait_pop,Timer){
	std::threadsafe::priority_queue<int> queue;
	testTimerPop(queue);
}

TEST(Try_top,HandleBasicOperation){
	std::threadsafe::priority_queue<int> queue;
	int out;
	ASSERT_EQ(false,queue.try_top(out));
	for (int i = 0;i < 10;i++){
		queue.push(randomNumbers[i]);
	}

	for (int i = 1;i <= 10;i++){
		ASSERT_EQ(true,queue.try_top(out));
		ASSERT_EQ(10,queue.size());
		ASSERT_EQ(10,out);
	}
}

TEST(Try_top,ThreadSafety){
	std::threadsafe::priority_queue<int> queue;
	constexpr int NUM_PRODUCERS = 1;
	constexpr int NUM_CONSUMERS = 1;
	constexpr int ITERATIONS = 10;

	auto producer = [&](int id,int it){
				queue.push(it);
			};
	
	int n = 0;
	auto consumer = [&](int id,int it){
				int out;
				
				while (!queue.try_top(out) && out < ITERATIONS){
					ASSERT_EQ(out,n);
					n++;
				}
			};

	launchThreads(producer,consumer,NUM_PRODUCERS,NUM_CONSUMERS,ITERATIONS);

}

TEST(Wait_top,HandleBasicOperation){
	std::threadsafe::priority_queue<int> queue;
	int out;
	for (int i = 0;i < 10;i++){
		queue.push(randomNumbers[i]);
	}

	for (int i = 10;i >= 1;i--){
		queue.wait_top(out);
		ASSERT_EQ(10,queue.size());
		ASSERT_EQ(10,out);
	}

}

TEST(Wait_top,Timer){
	std::threadsafe::priority_queue<int> queue;
	
	std::thread producer([&]{
		queue.push(1);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		queue.push(2);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		queue.push(3);
	});

	std::thread consumer([&]{
		int output;
		
		try {
			queue.wait_top(output,std::chrono::milliseconds(5));
			ASSERT_EQ(1,output);
			queue.wait_pop(output);

			queue.wait_top(output,std::chrono::milliseconds(15));
			ASSERT_EQ(2,output);
			queue.wait_pop(output);
		} catch(std::threadsafe::Time_Expired e){
			FAIL() << " exception Time_Expired throwed";
		}

		try {
			queue.wait_top(output,std::chrono::milliseconds(1));
			FAIL() << " exception didn't throw!";
		} catch(std::threadsafe::Time_Expired e){
		}
	});

	producer.join();
	consumer.join();
}

TEST(EMPTY,HanbleBasicOperation){
	std::threadsafe::priority_queue<int> queue;
	ASSERT_TRUE(queue.empty());
	queue.push(1);
	ASSERT_FALSE(queue.empty());
}
