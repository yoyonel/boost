#!/bin/bash

#url: http://coliru.stacked-crooked.com/
#g++ -std=c++11 -Os -Wall -pedantic -pthread $1 -c -E -P

cd build
make boost_template_variadic.s
subl CMakeFiles/boost_variadic.dir/boost_template_variadic.cpp.s
cd -
