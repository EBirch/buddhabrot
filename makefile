all: main.cpp
	mpicxx -std=c++11  main.cpp -g -o buddhabrot -lpthread
