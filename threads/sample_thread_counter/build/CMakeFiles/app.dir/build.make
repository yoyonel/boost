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
CMAKE_SOURCE_DIR = /home/latty/Prog/boost/sample_thread_counter

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/latty/Prog/boost/sample_thread_counter/build

# Include any dependencies generated for this target.
include CMakeFiles/app.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/app.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/app.dir/flags.make

CMakeFiles/app.dir/app.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/app.cpp.o: ../app.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/latty/Prog/boost/sample_thread_counter/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/app.dir/app.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/app.dir/app.cpp.o -c /home/latty/Prog/boost/sample_thread_counter/app.cpp

CMakeFiles/app.dir/app.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/app.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/latty/Prog/boost/sample_thread_counter/app.cpp > CMakeFiles/app.dir/app.cpp.i

CMakeFiles/app.dir/app.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/app.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/latty/Prog/boost/sample_thread_counter/app.cpp -o CMakeFiles/app.dir/app.cpp.s

CMakeFiles/app.dir/app.cpp.o.requires:
.PHONY : CMakeFiles/app.dir/app.cpp.o.requires

CMakeFiles/app.dir/app.cpp.o.provides: CMakeFiles/app.dir/app.cpp.o.requires
	$(MAKE) -f CMakeFiles/app.dir/build.make CMakeFiles/app.dir/app.cpp.o.provides.build
.PHONY : CMakeFiles/app.dir/app.cpp.o.provides

CMakeFiles/app.dir/app.cpp.o.provides.build: CMakeFiles/app.dir/app.cpp.o

# Object files for target app
app_OBJECTS = \
"CMakeFiles/app.dir/app.cpp.o"

# External object files for target app
app_EXTERNAL_OBJECTS =

app: CMakeFiles/app.dir/app.cpp.o
app: CMakeFiles/app.dir/build.make
app: liblogger_thread_safe.a
app: /usr/local/lib/libboost_log.so
app: /usr/local/lib/libboost_log_setup.so
app: /usr/local/lib/libboost_system.so
app: /usr/local/lib/libboost_thread.so
app: /usr/lib/x86_64-linux-gnu/libpthread.so
app: CMakeFiles/app.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable app"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/app.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/app.dir/build: app
.PHONY : CMakeFiles/app.dir/build

CMakeFiles/app.dir/requires: CMakeFiles/app.dir/app.cpp.o.requires
.PHONY : CMakeFiles/app.dir/requires

CMakeFiles/app.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/app.dir/cmake_clean.cmake
.PHONY : CMakeFiles/app.dir/clean

CMakeFiles/app.dir/depend:
	cd /home/latty/Prog/boost/sample_thread_counter/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/latty/Prog/boost/sample_thread_counter /home/latty/Prog/boost/sample_thread_counter /home/latty/Prog/boost/sample_thread_counter/build /home/latty/Prog/boost/sample_thread_counter/build /home/latty/Prog/boost/sample_thread_counter/build/CMakeFiles/app.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/app.dir/depend

