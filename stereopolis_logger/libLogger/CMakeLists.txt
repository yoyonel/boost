cmake_minimum_required(VERSION 3.0)

project(LibLogger)

#############
### BOOST ###
#############

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

#message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
#message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
#message(status "** Boost Libraries: ${Boost_LIBRARIES}")

add_definitions(-DBOOST_LOG_DYN_LINK)

#############

#sources files
set(LibLoggerSRCS
	include/Logging.h
	src/Logging.cpp
	)

include_directories(../.)

add_library(LibLogger ${LibLoggerSRCS})

if (${WIN32})
    target_link_libraries(LibLogger ${Boost_LIBRARIES})
else ()
    target_link_libraries(LibLogger ${Boost_LIBRARIES} pthread)
endif()


