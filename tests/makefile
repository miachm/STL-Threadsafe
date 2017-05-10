CC_OPTIONS = -std=c++11
CC_LIBRARY = -lgtest -lgtest_main -lgmock -lgmock_main -pthread
CC_INCLUDE = -I../include/
CC_LINKS_FLAGS = $(CC_LIBRARY) $(CC_OPTIONS) $(CC_INCLUDE)

stack: 
	g++ -o stack $(CC_LINKS_FLAGS) teststack.cpp

