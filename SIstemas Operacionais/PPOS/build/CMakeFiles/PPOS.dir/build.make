# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/169/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/169/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ayrton/CLionProjects/PPOS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ayrton/CLionProjects/PPOS/build

# Include any dependencies generated for this target.
include CMakeFiles/PPOS.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/PPOS.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PPOS.dir/flags.make

CMakeFiles/PPOS.dir/test/main.c.o: CMakeFiles/PPOS.dir/flags.make
CMakeFiles/PPOS.dir/test/main.c.o: ../test/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ayrton/CLionProjects/PPOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/PPOS.dir/test/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PPOS.dir/test/main.c.o -c /home/ayrton/CLionProjects/PPOS/test/main.c

CMakeFiles/PPOS.dir/test/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PPOS.dir/test/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ayrton/CLionProjects/PPOS/test/main.c > CMakeFiles/PPOS.dir/test/main.c.i

CMakeFiles/PPOS.dir/test/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PPOS.dir/test/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ayrton/CLionProjects/PPOS/test/main.c -o CMakeFiles/PPOS.dir/test/main.c.s

CMakeFiles/PPOS.dir/source/queue.c.o: CMakeFiles/PPOS.dir/flags.make
CMakeFiles/PPOS.dir/source/queue.c.o: ../source/queue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ayrton/CLionProjects/PPOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/PPOS.dir/source/queue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PPOS.dir/source/queue.c.o -c /home/ayrton/CLionProjects/PPOS/source/queue.c

CMakeFiles/PPOS.dir/source/queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PPOS.dir/source/queue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ayrton/CLionProjects/PPOS/source/queue.c > CMakeFiles/PPOS.dir/source/queue.c.i

CMakeFiles/PPOS.dir/source/queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PPOS.dir/source/queue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ayrton/CLionProjects/PPOS/source/queue.c -o CMakeFiles/PPOS.dir/source/queue.c.s

# Object files for target PPOS
PPOS_OBJECTS = \
"CMakeFiles/PPOS.dir/test/main.c.o" \
"CMakeFiles/PPOS.dir/source/queue.c.o"

# External object files for target PPOS
PPOS_EXTERNAL_OBJECTS =

PPOS: CMakeFiles/PPOS.dir/test/main.c.o
PPOS: CMakeFiles/PPOS.dir/source/queue.c.o
PPOS: CMakeFiles/PPOS.dir/build.make
PPOS: ../library/lib/libppos_static.a
PPOS: CMakeFiles/PPOS.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ayrton/CLionProjects/PPOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable PPOS"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PPOS.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PPOS.dir/build: PPOS
.PHONY : CMakeFiles/PPOS.dir/build

CMakeFiles/PPOS.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PPOS.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PPOS.dir/clean

CMakeFiles/PPOS.dir/depend:
	cd /home/ayrton/CLionProjects/PPOS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ayrton/CLionProjects/PPOS /home/ayrton/CLionProjects/PPOS /home/ayrton/CLionProjects/PPOS/build /home/ayrton/CLionProjects/PPOS/build /home/ayrton/CLionProjects/PPOS/build/CMakeFiles/PPOS.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PPOS.dir/depend

