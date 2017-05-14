#include <iostream>
#include <cmath>
#include <sstream>
#include <thread>
#include "queue thread safe.hpp"

constexpr int NUM_WORKERS = 4;
constexpr double EXIT = 0.0;

void worker(std::threadsafe::queue<double> &tasks){
	double output;
	tasks.wait_pop(output);
	while (output != EXIT){
		// busy work
		double operation = std::sqrt(output);

		std::stringstream stream;
		stream << "Sqrt of " << output << " is " << operation << std::endl;

		std::cout << stream.str();

		tasks.wait_pop(output);
	}
}

int main(){
	std::threadsafe::queue<double> tasks;

	std::thread threads[NUM_WORKERS];

	for (int i = 0;i < NUM_WORKERS;i++)
		threads[i] = std::thread(worker,std::ref(tasks));	
	
	for (double n = 1.0;n <= 10; n += 1.0)
		tasks.push(n*n);

	for (int i = 0;i < NUM_WORKERS;i++)
		tasks.push(EXIT);

	for (int i = 0;i < NUM_WORKERS;i++)
		threads[i].join();
}
