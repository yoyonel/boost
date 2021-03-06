cmake_minimum_required(VERSION 2.8)
project(logger_thread_safe)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

if (${WIN32})
    set(BOOST_ROOT C:/BOOST/boost_1_57_0)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib64-msvc-12.0)
    find_package(Boost 1.57 COMPONENTS log log_setup system REQUIRED)
else()
    find_package(Boost 1.57 COMPONENTS log log_setup system thread REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")

add_definitions(-Os -Wall -pedantic -pthread -c -E -P)

add_library(boost_variadic boost_template_variadic.cpp)
