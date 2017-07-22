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
