#!/bin/sh
clang parse.c -oparse && ./parse data.txt >scarygnm_generated.hpp

clang++ -O3 -Wall -Wextra -std=c++23 test.cpp -o test && ./test
