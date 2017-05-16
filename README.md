# STL-Threadsafe

A threadsafe implementation of Queue, Stack and Priority Queue.

```c++

#include <iostream>
#include <cmath>
#include <sstream>
#include <thread>
#include "queue thread safe.hpp"

constexpr int NUM_WORKERS = 4;
constexpr double EXIT = 0.0;

void printResult(double value){
	std::stringstream stream;
	stream << "Sqrt of " << value << " is " << operation << std::endl;

	std::cout << stream.str();
}

void worker(std::threadsafe::queue<double> &tasks){
	double operation;
	tasks.wait_pop(operation);
	while (operation != EXIT){
		double sqrt = std::sqrt(operation);
		printResult(sqrt);
		tasks.wait_pop(operation);
	}
}

void launchThreads(std::thread threads[],std::threadsafe::queue<double> &tasks){
	for (int i = 0;i < NUM_WORKERS;i++)
		threads[i] = std::thread(worker,std::ref(tasks));	
}

void addWork(std::threadsafe::queue<double> &tasks){
	for (double n = 1.0;n <= 10; n += 1.0)
		tasks.push(n*n);
}

void wait(std::thread threads[],std::threadsafe::queue<double> &tasks){
	for (int i = 0;i < NUM_WORKERS;i++)
		tasks.push(EXIT);

	for (int i = 0;i < NUM_WORKERS;i++)
		threads[i].join();
}

int main(){
	std::threadsafe::queue<double> tasks;
	std::thread threads[NUM_WORKERS];
	
	launchThreads(threads,tasks);
	addWork(tasks);
	wait(threads,tasks);
}`
```

The next operations are supported:

  - Push: Push an element in the container.
  - try_pop(output): Try pop out an element and put in the output variable. Return false if the container is empty.
  - wait_pop(output): Pop out an element, blocks if the container is empty.
  - wait_pop(output,time): Pop out an element blocks if the container is empty. If the time expires, it will throw an Time_Expired exception.
  - try_top(output): Try a look up in the top of the container. Return false if the container is empty.
  - wait_top(output): Try a look up, it blocks if the container is empty.
  - wait_top(output,time): Try a look up, it blocks if the container is empty. If the time expires, it will throw an Time_Expired.
  - setLimit(tam): Set an upper bound in the container. If a thread try to push a full container, it will block.
  - size(): Returns the number of elements.
  - empty(): Returns true if the container is empty.
  
  # Usage
  
  It's a header library. You only has to include the headers in your project.
