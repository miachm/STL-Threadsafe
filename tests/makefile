CC_OPTIONS = -std=c++11
CC_LIBRARY = -lgtest -lgtest_main -lgmock -lgmock_main -pthread
CC_INCLUDE = -I../include/
CC_LINKS_FLAGS = $(CC_LIBRARY) $(CC_OPTIONS) $(CC_INCLUDE)

stack: 
	g++ -o stack $(CC_LINKS_FLAGS) teststack.cpp

queue:
	g++ -o queue $(CC_LINKS_FLAGS) testqueue.cpp
priorityqueue:
	g++ -o priorityqueue $(CC_LINKS_FLAGS) testpriorityqueue.cpp
