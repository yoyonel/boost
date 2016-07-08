#!/bin/bash

#url: http://coliru.stacked-crooked.com/
g++ -std=c++11 -Os -Wall -pedantic -pthread $1 -c -E -P
