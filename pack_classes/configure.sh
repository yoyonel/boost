#!/bin/bash

mkdir -p build
cd build
cmake ../ -DBOOST_ROOT=/home/latty/Prog/boost/boost_1_53_0 -DBoost_NO_SYSTEM_PATHS=ON ../
cd -