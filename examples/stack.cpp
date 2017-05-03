#include <iostream>
#include "stack thread safe.hpp"

constexpr int NUM_WORKERS = 4;

void consume(std::threadsafe::stack<int> &stack){
	for (int i = 0;i < 3;i++){
		int n;
		stack.wait_pop(n);
		stack.push(n*2);
	}
}

int main(){
	std::threadsafe::stack<int> stack;
	stack.push(1);

	std::thread threads[NUM_WORKERS];

	for (int i = 0;i < NUM_WORKERS;i++){
		threads[i] = std::thread(consume,std::ref(stack));
	}

	for (int i = 0;i < NUM_WORKERS;i++){
		threads[i].join();
	}

	int n;
	stack.wait_pop(n);
	std::cout << n << std::endl;
}
