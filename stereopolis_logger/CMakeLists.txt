#version minimale de CMake 
cmake_minimum_required(VERSION 2.8.8)

project(MMI)

set(Project_DIR .)

#nom du projet
set(Project_NAME MMI)

# Compilation de l'executable
file( GLOB_RECURSE Project_HEADERS ${Project_DIR}/include/*.h)
file( GLOB_RECURSE Project_SOURCES ${Project_DIR}/src/*.cpp)

include_directories( $(Project_DIR) )

# Ajout de la dépendance de lib externe 'libLogger'
# libLogger dépend de Boost, 
# donc normalement il met en place le setting pour utiliser Boost
add_subdirectory(libLogger)

link_libraries(
	LibLogger
	#############################
	# BOOST libs dependancies 	#
	${Boost_LIBRARIES} 			#
	pthread						#
	#############################
)

add_definitions(-DBOOST_LOG_DYN_LINK)

add_executable(
				${Project_NAME}
				${Project_HEADERS}
				${Project_SOURCES}
)
