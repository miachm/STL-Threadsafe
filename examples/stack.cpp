#include <iostream>
#include "stack-threadsafe.hpp"

constexpr int NUM_WORKERS = 4;
constexpr int INCR = 3;

void increment(std::threadsafe::stack<int> &stack){
	for (int i = 0;i < INCR;i++){
		int n;
		stack.wait_pop(n);
		stack.push(n+1);
	}
}

int main(){
	std::threadsafe::stack<int> stack;
	stack.push(0);

	std::thread threads[NUM_WORKERS];

	for (int i = 0;i < NUM_WORKERS;i++)
		threads[i] = std::thread(increment,std::ref(stack));

	for (int i = 0;i < NUM_WORKERS;i++)
		threads[i].join();

	// It should print: NUM_WORKERS*INCR
	int n;
	stack.wait_pop(n);
	std::cout << n << std::endl;
}
