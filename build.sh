#!/bin/sh

set xe 

g++ -O3 -Wall -Wextra main.cpp -o main -lraylib

g++ -O3 -Wall -Wextra test.cpp -o test -std=c++0x -lpthread -lraylib -ljsoncpp