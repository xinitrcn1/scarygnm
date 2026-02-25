#!/bin/sh -ex

clang parse.c -oparse && ./parse data.txt >scarygnm_generated.hpp

#clang++ -O3 -target amdgcn-amd shaders/texel_fs.cpp -oshaders/texel_fs.elf
#objdump -SC shaders/texel_fs.elf

clang++ -O0 -g -Wall -Wextra -std=c++23 test.cpp -o test
# ./test
gdb ./test
