#pragma once

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

template <typename Container>
void testCoherency(Container& another_container,const int ITERATIONS){
	ASSERT_EQ(ITERATIONS*ITERATIONS,another_container.size());
	int freq_table[ITERATIONS];
	std::fill(freq_table,freq_table+ITERATIONS,0);
	while (!another_container.empty()){
		int out;
		another_container.wait_pop(out);
		freq_table[out]++;
	}

	for (int i = 0;i < ITERATIONS;i++)
	{
		ASSERT_EQ(ITERATIONS,freq_table[i]);
	}
}

template<typename Container>
void testPushThreadSafety(Container& container){
	Container another_container;
	constexpr int ITERATIONS = 10;
	constexpr int PRODUCERS = ITERATIONS;
	constexpr int CONSUMERS = ITERATIONS;

	auto producer = [&](int id,int it){container.push(id);};
	auto consumer = [&](int id,int it){
					int out;
					container.wait_pop(out);
					ASSERT_LT(out,ITERATIONS);
					ASSERT_GE(out,0);
					another_container.push(out);
					};

	launchThreads(producer,consumer,PRODUCERS,CONSUMERS,ITERATIONS);

	testCoherency(another_container,ITERATIONS);
}

template<typename Container>
void testPopThreadSafety(Container& container){
	Container another_container;
	constexpr int NUM_PRODUCERS = 10;
	constexpr int NUM_CONSUMERS = 10;
	constexpr int ITERATIONS = 10;

	auto producer = [&](int id,int it){
				container.push(id);
			};
	
	auto consumer = [&](int id,int it){
				int out;
				while (!container.try_pop(out));
				another_container.push(out);
			};

	launchThreads(producer,consumer,NUM_PRODUCERS,NUM_CONSUMERS,ITERATIONS);

	testCoherency(another_container,ITERATIONS);
}

template<typename Container>
void testPushUpperBound(Container& container){
	constexpr int ITERATIONS_PRODUCERS = 10;
	constexpr int NUM_PRODUCERS = ITERATIONS_PRODUCERS;
	constexpr int NUM_CONSUMERS = 1;
	constexpr int SIZE = NUM_PRODUCERS/2;
	constexpr int ITERATIONS_CONSUMERS = ITERATIONS_PRODUCERS*NUM_PRODUCERS;
	container.setLimit(SIZE);

	auto producer = [&](int id,int it){
				container.push(id);
				ASSERT_LE(container.size(),SIZE);
			};
	auto consumer = [&](int id,int it){
				int out;
				ASSERT_LE(container.size(),SIZE);
				container.wait_pop(out);
			};

	launchThreads(producer,consumer,NUM_PRODUCERS,NUM_CONSUMERS,ITERATIONS_PRODUCERS,ITERATIONS_CONSUMERS);
}
