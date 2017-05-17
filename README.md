# STL-Threadsafe

A threadsafe implementation of Queue, Stack and Priority Queue.

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
