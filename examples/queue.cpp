#include <iostream>
#include <cmath>
#include <sstream>
#include <thread>
#include "queue-threadsafe.hpp"

constexpr int NUM_WORKERS = 4;
constexpr double EXIT = 0.0;

void printResult(double value,double operation){
	std::stringstream stream;
	stream << "Sqrt of " << value << " is " << operation << std::endl;

	std::cout << stream.str();
}

void worker(std::threadsafe::queue<double> &tasks){
	double operation;
	tasks.wait_pop(operation);
	while (operation != EXIT){
		double sqrt = std::sqrt(operation);
		printResult(sqrt,operation);
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
}
