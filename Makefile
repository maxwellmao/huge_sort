CC = g++
CXXFLAGS = -std=c++98 -Wall -O3
CXXFLAGS += -DNDEBUG

huge_sort: huge_sort.h huge_sort.cpp
	$(CC) $(CXXFLAGS) -o huge_sort huge_sort.cpp

