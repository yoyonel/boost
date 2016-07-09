#!/bin/bash

mkdir -p build
cd build
cmake ../ -DBOOST_ROOT=/usr/local -DBoost_NO_SYSTEM_PATHS=ON ../
cd -