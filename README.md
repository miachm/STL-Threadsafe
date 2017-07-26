# STL-Threadsafe

A threadsafe implementation of the standard containers.

You should know the STL library of C++ (map, stack, queue...). This library is a wrapper of these containers adding safety and other mechanisms needed for concurrent programming.

A example of these mechanisms is the queue class, where you can define the behaviour in case the queue is empty

```c++

std::threadsafe::queue<int> queue;
int out;

//...

queue.wait_pop(out); // if the queue is empty, will block

try{
	queue.wait_pop(out,std::chrono::milliseconds(10)); // if the queue is empty, will block 10 milliseconds as maximum
} catch(std::threadsafe::Time_Expired e){
	std::cerr << "Time expired!" << std::endl;
}

if (!queue.try_pop(out)){ // If the queue is empty, will return false
	std::cerr << "Empty queue" << std::endl;
}
```

# Usage

A simple example of producer/consumer, consumer has to take the input, do some maths operations and print it in the stdout. Repeat until that receives a special code.

```c++

void consumer(std::threadsafe::queue<double> &tasks){
	double operation;
	tasks.wait_pop(operation);
	while (operation != EXIT){
		double sqrt = std::sqrt(operation);
		printResult(sqrt);
		tasks.wait_pop(operation);
	}
}

void producer(std::threadsafe::queue<double> &tasks){
	for (int i = 1;i < 10;i++)
		tasks.push(i*i);
	
	tasks.push(EXIT);
}
```
There are more examples of usage in the example's folder.

# API

Stack, Queue and Priority_Queue share the same methods:

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
  
  Besides, Queue has wait_back() and try_back() methods, equivalent to wait_top() and try_top() but looking the last position of the queue.
  
  # Installation
  
  It's a header library. So, the installation is pretty straightforward.
  
  Download/clone the repository, add the include folder to your project and you can use it. Be aware that you need compiler with C++14 support
