#include <iostream>
#include <thread>
#include "priority_queue-threadsafe.hpp"

int main(){
	std::threadsafe::priority_queue<int> bids;

	std::thread producer([&]{
		int randomBids[20] = {3,19,11,2,4,12,1,20,14,5,18,10,15,8,17,6,16,7,9,13};
		for (int i = 0;i < 20;i++){
			bids.push(randomBids[i]);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

	try{
		int out;
		while (true){
			bids.wait_pop(out,std::chrono::milliseconds(100));
			std::cout << "Checking new bids" << std::endl;

			std::cout << "\tBid: " << out << std::endl;
			while (bids.try_pop(out)){
				std::cout << "\tBid: " << out << std::endl;
			}

			std::cout << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(3));
		}

	}
	catch (std::threadsafe::Time_Expired e){
		std::cout << "No more bids coming, shutting down..." << std::endl;
	}

	producer.join();
}
