# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/latty/Prog/boost/stereopolis_logger

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/latty/Prog/boost/stereopolis_logger/build

# Include any dependencies generated for this target.
include CMakeFiles/MMI.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MMI.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MMI.dir/flags.make

CMakeFiles/MMI.dir/src/MMI.cpp.o: CMakeFiles/MMI.dir/flags.make
CMakeFiles/MMI.dir/src/MMI.cpp.o: ../src/MMI.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/latty/Prog/boost/stereopolis_logger/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/MMI.dir/src/MMI.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/MMI.dir/src/MMI.cpp.o -c /home/latty/Prog/boost/stereopolis_logger/src/MMI.cpp

CMakeFiles/MMI.dir/src/MMI.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MMI.dir/src/MMI.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/latty/Prog/boost/stereopolis_logger/src/MMI.cpp > CMakeFiles/MMI.dir/src/MMI.cpp.i

CMakeFiles/MMI.dir/src/MMI.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MMI.dir/src/MMI.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/latty/Prog/boost/stereopolis_logger/src/MMI.cpp -o CMakeFiles/MMI.dir/src/MMI.cpp.s

CMakeFiles/MMI.dir/src/MMI.cpp.o.requires:
.PHONY : CMakeFiles/MMI.dir/src/MMI.cpp.o.requires

CMakeFiles/MMI.dir/src/MMI.cpp.o.provides: CMakeFiles/MMI.dir/src/MMI.cpp.o.requires
	$(MAKE) -f CMakeFiles/MMI.dir/build.make CMakeFiles/MMI.dir/src/MMI.cpp.o.provides.build
.PHONY : CMakeFiles/MMI.dir/src/MMI.cpp.o.provides

CMakeFiles/MMI.dir/src/MMI.cpp.o.provides.build: CMakeFiles/MMI.dir/src/MMI.cpp.o

# Object files for target MMI
MMI_OBJECTS = \
"CMakeFiles/MMI.dir/src/MMI.cpp.o"

# External object files for target MMI
MMI_EXTERNAL_OBJECTS =

MMI: CMakeFiles/MMI.dir/src/MMI.cpp.o
MMI: CMakeFiles/MMI.dir/build.make
MMI: libLogger/libLibLogger.a
MMI: /usr/local/lib/libboost_log.so
MMI: /usr/local/lib/libboost_log_setup.so
MMI: /usr/local/lib/libboost_system.so
MMI: /usr/local/lib/libboost_thread.so
MMI: /usr/lib/x86_64-linux-gnu/libpthread.so
MMI: CMakeFiles/MMI.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable MMI"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MMI.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MMI.dir/build: MMI
.PHONY : CMakeFiles/MMI.dir/build

CMakeFiles/MMI.dir/requires: CMakeFiles/MMI.dir/src/MMI.cpp.o.requires
.PHONY : CMakeFiles/MMI.dir/requires

CMakeFiles/MMI.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MMI.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MMI.dir/clean

CMakeFiles/MMI.dir/depend:
	cd /home/latty/Prog/boost/stereopolis_logger/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/latty/Prog/boost/stereopolis_logger /home/latty/Prog/boost/stereopolis_logger /home/latty/Prog/boost/stereopolis_logger/build /home/latty/Prog/boost/stereopolis_logger/build /home/latty/Prog/boost/stereopolis_logger/build/CMakeFiles/MMI.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MMI.dir/depend

