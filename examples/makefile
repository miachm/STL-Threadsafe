CC_OPTIONS = -std=c++11
CC_LIBRARY = -pthread
CC_INCLUDE = -I../include/
CC_LINKS_FLAGS = $(CC_LIBRARY) $(CC_OPTIONS) $(CC_INCLUDE)

all: 
	g++ -o stack $(CC_LINKS_FLAGS) stack.cpp
	g++ -o queue $(CC_LINKS_FLAGS) queue.cpp
	g++ -o priority_queue $(CC_LINKS_FLAGS) priority_queue.cpp

