# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha

# Include any dependencies generated for this target.
include Demos/Raytracer/CMakeFiles/AppRaytracer.dir/depend.make

# Include the progress variables for this target.
include Demos/Raytracer/CMakeFiles/AppRaytracer.dir/progress.make

# Include the compile flags for this target's objects.
include Demos/Raytracer/CMakeFiles/AppRaytracer.dir/flags.make

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/flags.make
Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o: Demos/Raytracer/Raytracer.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o"
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/AppRaytracer.dir/Raytracer.o -c /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer/Raytracer.cpp

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AppRaytracer.dir/Raytracer.i"
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer/Raytracer.cpp > CMakeFiles/AppRaytracer.dir/Raytracer.i

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AppRaytracer.dir/Raytracer.s"
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer/Raytracer.cpp -o CMakeFiles/AppRaytracer.dir/Raytracer.s

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.requires:
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.requires

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.provides: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.requires
	$(MAKE) -f Demos/Raytracer/CMakeFiles/AppRaytracer.dir/build.make Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.provides.build
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.provides

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.provides.build: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/flags.make
Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o: Demos/Raytracer/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o"
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/AppRaytracer.dir/main.o -c /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer/main.cpp

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AppRaytracer.dir/main.i"
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer/main.cpp > CMakeFiles/AppRaytracer.dir/main.i

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AppRaytracer.dir/main.s"
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer/main.cpp -o CMakeFiles/AppRaytracer.dir/main.s

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.requires:
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.requires

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.provides: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.requires
	$(MAKE) -f Demos/Raytracer/CMakeFiles/AppRaytracer.dir/build.make Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.provides.build
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.provides

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.provides.build: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o

# Object files for target AppRaytracer
AppRaytracer_OBJECTS = \
"CMakeFiles/AppRaytracer.dir/Raytracer.o" \
"CMakeFiles/AppRaytracer.dir/main.o"

# External object files for target AppRaytracer
AppRaytracer_EXTERNAL_OBJECTS =

Demos/Raytracer/AppRaytracer: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o
Demos/Raytracer/AppRaytracer: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o
Demos/Raytracer/AppRaytracer: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/build.make
Demos/Raytracer/AppRaytracer: Demos/OpenGL/libOpenGLSupport.a
Demos/Raytracer/AppRaytracer: src/BulletDynamics/libBulletDynamics.a
Demos/Raytracer/AppRaytracer: src/BulletCollision/libBulletCollision.a
Demos/Raytracer/AppRaytracer: src/LinearMath/libLinearMath.a
Demos/Raytracer/AppRaytracer: /usr/lib/i386-linux-gnu/libglut.so
Demos/Raytracer/AppRaytracer: /usr/lib/i386-linux-gnu/libGL.so
Demos/Raytracer/AppRaytracer: /usr/lib/i386-linux-gnu/libGLU.so
Demos/Raytracer/AppRaytracer: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable AppRaytracer"
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/AppRaytracer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Demos/Raytracer/CMakeFiles/AppRaytracer.dir/build: Demos/Raytracer/AppRaytracer
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/build

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/requires: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/Raytracer.o.requires
Demos/Raytracer/CMakeFiles/AppRaytracer.dir/requires: Demos/Raytracer/CMakeFiles/AppRaytracer.dir/main.o.requires
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/requires

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/clean:
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer && $(CMAKE_COMMAND) -P CMakeFiles/AppRaytracer.dir/cmake_clean.cmake
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/clean

Demos/Raytracer/CMakeFiles/AppRaytracer.dir/depend:
	cd /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer /home/lubos/Plocha/Open4speed/support/bullet3-Bullet-2.83-alpha/Demos/Raytracer/CMakeFiles/AppRaytracer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Demos/Raytracer/CMakeFiles/AppRaytracer.dir/depend

